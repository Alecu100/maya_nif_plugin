#include "include/Importers/NifMeshImporterFallout4.h"


NifMeshImporterFallout4::NifMeshImporterFallout4()
{
}

NifMeshImporterFallout4::NifMeshImporterFallout4(NifTranslatorOptionsRef translatorOptions, NifTranslatorDataRef translatorData, NifTranslatorUtilsRef translatorUtils)
{
	this->translatorOptions = translatorOptions;
	this->translatorData = translatorData;
	this->translatorUtils = translatorUtils;
}

MObject NifMeshImporterFallout4::connectSubSegment(MObject blind_data_object, int blind_data_id)
{
	MStatus status;
	MFnMesh blind_data_mesh(blind_data_object);
	MString mel_command = "polyBlindData -id ";
	mel_command += blind_data_id;
	mel_command += " -associationType \"face\" -ldn \"subSegmentValue\" -ind 1";
	status = MGlobal::executeCommand(mel_command);

	blind_data_mesh.updateSurface();

	MDGModifier dg_modifier;
	MFnDependencyNode sub_segment_node;
	sub_segment_node.create("bsSubSegment");

	MPlug input_message_dismember = sub_segment_node.findPlug("targetFaces");
	MPlug input_message_shape = sub_segment_node.findPlug("targetShape");

	MPlug mesh_out_message = blind_data_mesh.findPlug("message");
	dg_modifier.connect(mesh_out_message, input_message_shape);
	dg_modifier.doIt();

	MPlugArray connections;
	MPlug input_mesh = blind_data_mesh.findPlug("inMesh");
	MPlug out_mesh;
	MPlug type_id;
	MFnDependencyNode node_out_mesh;

	input_mesh.connectedTo(connections, true, false);
	out_mesh = connections[0];
	node_out_mesh.setObject(out_mesh.node());
	type_id = node_out_mesh.findPlug("typeId");

	while (type_id.asInt() != blind_data_id)
	{
		input_mesh = node_out_mesh.findPlug("inMesh");
		connections.clear();
		input_mesh.connectedTo(connections, true, false);
		out_mesh = connections[0];
		node_out_mesh.setObject(out_mesh.node());
		type_id = node_out_mesh.findPlug("typeId");
		if (type_id.isNull())
		{
			break;
			node_out_mesh.setObject(MObject());
		}
	}

	if (!node_out_mesh.object().isNull())
	{
		MPlug message = node_out_mesh.findPlug("message");
		dg_modifier.connect(message, input_message_dismember);
		dg_modifier.doIt();

		return sub_segment_node.object();
	}

	return MObject();
}

MDagPath NifMeshImporterFallout4::ImportMesh(NiAVObjectRef root, MObject parent)
{
	//out << "ImportMesh() begin" << endl;

	//Make invisible if this or any children are invisible
	bool visible = true;
	if (root->GetVisibility() == false)
	{
		visible = false;
	}
	else
	{
		NiNodeRef rootNode = DynamicCast<NiNode>(root);
		if (rootNode != NULL)
		{
			vector<NiAVObjectRef> children = rootNode->GetChildren();
			for (unsigned i = 0; i < children.size(); ++i)
			{
				if (children[i]->GetVisibility() == false)
				{
					visible = false;
					break;
				}
			}
		}
	}

	if (visible == false)
	{
		MFnDagNode parFn(parent);
		MPlug vis = parFn.findPlug(MString("visibility"));
		vis.setValue(false);
	}

	//out << "Try out ComplexShape::Merge on" << root << endl;
	BSComplexShape bs_complex_shape;

	bs_complex_shape.Merge(root);

	vector<WeightedVertex> nif_verts = bs_complex_shape.GetVertices();
	unsigned NumVertices = unsigned(nif_verts.size());
	//out << "Num Vertices:  " << NumVertices << endl;

	MPointArray maya_verts(NumVertices);

	for (unsigned i = 0; i < NumVertices; ++i)
	{
		maya_verts[i] = MPoint(nif_verts[i].position.x, nif_verts[i].position.y, nif_verts[i].position.z, 0.0f);
	}

	//out << "Getting polygons..." << endl;
	//Get Polygons
	int NumPolygons = 0;
	vector<ComplexFace> niRawFaces = bs_complex_shape.GetFaces();

	MIntArray maya_poly_counts;
	MIntArray maya_connects;
	vector<ComplexFace> niFaces;
	map<int, bool> removedFacesIndexes;

	//float max_dif = 0;
	//int count_dif = 0;

	for (unsigned i = 0; i < niRawFaces.size(); ++i)
	{
		//Only append valid triangles
		if (niRawFaces[i].points.size() != 3)
		{
			//not a triangle
			continue;
		}

		const ComplexFace& f = niRawFaces[i];

		unsigned p0 = f.points[0].vertexIndex;
		unsigned p1 = f.points[1].vertexIndex;
		unsigned p2 = f.points[2].vertexIndex;

		if (p0 == p1 || p0 == p2 || p1 == p2)
		{
			//Invalid triangle
			removedFacesIndexes[i] = true;
			continue;
		}

		maya_connects.append(p0);
		maya_connects.append(p1);
		maya_connects.append(p2);
		maya_poly_counts.append(3);
		niFaces.push_back(f);
	}
	niRawFaces.clear();

	//NumPolygons = triangles.size();
	NumPolygons = niFaces.size();
	//out << "Num Polygons:  " << NumPolygons << endl;

	//MIntArray maya_connects( connects, NumPolygons * 3 );

	//Create Mesh with empty default UV set at first
	MDagPath meshPath;
	MFnMesh meshFn;

	MStatus stat;

	meshFn.create(NumVertices, maya_poly_counts.length(), maya_verts, maya_poly_counts, maya_connects, parent, &stat);

	if (stat != MS::kSuccess)
	{
		//out << stat.errorString().asChar() << endl;
		throw runtime_error("Failed to create mesh.");
	}

	meshFn.getPath(meshPath);

	//out << "Importing vertex colors..." << endl;
	//Import Vertex Colors
	vector<Color4> nif_colors = bs_complex_shape.GetColors();
	if (nif_colors.size() > 0)
	{
		//Create vertex list
		MIntArray face_list;
		MIntArray vert_list;
		MColorArray maya_colors;

		for (unsigned f = 0; f < niFaces.size(); ++f)
		{
			//Make sure all of the points in this face have color
			if (
				niFaces[f].points[0].colorIndex == CS_NO_INDEX ||
				niFaces[f].points[1].colorIndex == CS_NO_INDEX ||
				niFaces[f].points[2].colorIndex == CS_NO_INDEX
			)
			{
				continue;
			}

			face_list.append(f);
			face_list.append(f);
			face_list.append(f);

			vert_list.append(niFaces[f].points[0].vertexIndex);
			vert_list.append(niFaces[f].points[1].vertexIndex);
			vert_list.append(niFaces[f].points[2].vertexIndex);

			Color4 color1 = nif_colors[niFaces[f].points[0].colorIndex];
			Color4 color2 = nif_colors[niFaces[f].points[1].colorIndex];
			Color4 color3 = nif_colors[niFaces[f].points[2].colorIndex];

			maya_colors.append(MColor(color1.r, color1.g, color1.b, color1.a));
			maya_colors.append(MColor(color2.r, color2.g, color2.b, color2.a));
			maya_colors.append(MColor(color3.r, color3.g, color3.b, color3.a));
		}

		MStatus stat = meshFn.setFaceVertexColors(maya_colors, face_list, vert_list);
		if (stat != MS::kSuccess)
		{
			//out << stat.errorString().asChar() << endl;
			throw runtime_error("Failed to set Colors.");
		}
	}

	//out << "Creating a list of the UV sets..." << endl;
	//Create a list of the UV sets used by the complex shape if any

	vector<TexCoord> uv_set = bs_complex_shape.GetTexCoordSet();

	if (uv_set.size() > 0)
	{
		meshFn.clearUVs();

		//Arrays for maya
		MFloatArray u_arr(uv_set.size()), v_arr(uv_set.size());

		//out << "uv_set_list.size():  " << uv_set_list.size() << endl;
		//out << "i:  " << i << endl;


		for (unsigned j = 0; j < uv_set.size(); ++j)
		{
			u_arr[j] = uv_set[j].u;
			v_arr[j] = 1.0f - uv_set[j].v;
		}

		//Assign the UVs to the object
		MString uv_set_name("map1");

		//out << "Set UVs...  u_arr:  " << u_arr.length() << " v_arr:  " << v_arr.length() << " uv_set_name " << uv_set_name.asChar() << endl;
		MStatus stat = meshFn.setUVs(u_arr, v_arr, &uv_set_name);
		if (stat != MS::kSuccess)
		{
			//out << stat.errorString().asChar() << endl;
			throw runtime_error("Failed to set UVs.");
		}

		//out << "Create list of which UV to assign to each polygon vertex";
		maya_poly_counts.clear();
		maya_connects.clear();
		//Create list of which UV to assign to each polygon vertex
		for (unsigned f = 0; f < niFaces.size(); ++f)
		{
			if (
				niFaces[f].points[0].vertexIndex == niFaces[f].points[1].vertexIndex ||
				niFaces[f].points[0].vertexIndex == niFaces[f].points[2].vertexIndex ||
				niFaces[f].points[1].vertexIndex == niFaces[f].points[2].vertexIndex
			)
			{
				//Invalid triangle
				continue;
			}

			//Make sure all of the points in this face have color
			unsigned match[3];
			int matches_found = 0;
			for (unsigned p = 0; p < 3; ++p)
			{
				//Figure out which index we're using, if any
				for (unsigned t = 0; t < niFaces[f].points[p].texCoordIndices.size(); ++t)
				{
					//out << "niFaces[" << f << "].points[" << p << "].texCoordIndices[" << t << "].texCoordSetIndex:  " << niFaces[f].points[p].texCoordIndices[t].texCoordSetIndex << endl;
					if (niFaces[f].points[p].texCoordIndices[t].texCoordSetIndex == 0)
					{
						//out << "Match found at " << t << endl;
						++matches_found;
						match[p] = t;
						break;
					}
				}
			}

			//out << " Matches found:  " << matches_found << endl;
			if (matches_found != 3)
			{
				//This face is not mapped, 0 points
				maya_poly_counts.append(0);
				continue;
			}

			unsigned tcIndices[3] = {
				niFaces[f].points[0].texCoordIndices[match[0]].texCoordIndex,
				niFaces[f].points[1].texCoordIndices[match[1]].texCoordIndex,
				niFaces[f].points[2].texCoordIndices[match[2]].texCoordIndex
			};


			if (tcIndices[0] == CS_NO_INDEX || tcIndices[0] == CS_NO_INDEX || tcIndices[0] == CS_NO_INDEX)
			{
				//This face is not mapped, 0 points
				maya_poly_counts.append(0);
				continue;
			}

			maya_poly_counts.append(3); //3 points in this face

			//out << "Texture Coord indices for set " << i << " face " << f << endl;

			maya_connects.append(tcIndices[0]);
			maya_connects.append(tcIndices[1]);
			maya_connects.append(tcIndices[2]);
		}

		stat = meshFn.assignUVs(maya_poly_counts, maya_connects, &uv_set_name);
		if (stat != MS::kSuccess)
		{
			//out << stat.errorString().asChar() << endl;
			throw runtime_error("Failed to assign UVs.");
		}
	}

	//See if the user wants us to import the normals
	if (this->translatorOptions->importNormals)
	{
		//out << "Getting Normals..." << endl;
		// Load Normals
		vector<Vector3> nif_normals = bs_complex_shape.GetNormals();
		//Create vertex list
		MIntArray face_list;
		MIntArray vert_list;
		MVectorArray maya_normals;
		if (nif_normals.size() != 0)
		{
			for (unsigned f = 0; f < niFaces.size(); ++f)
			{
				//Make sure all of the points in this face have a normal
				if (
					niFaces[f].points[0].normalIndex == CS_NO_INDEX ||
					niFaces[f].points[1].normalIndex == CS_NO_INDEX ||
					niFaces[f].points[2].normalIndex == CS_NO_INDEX
				)
				{
					continue;
				}

				face_list.append(f);
				face_list.append(f);
				face_list.append(f);

				vert_list.append(niFaces[f].points[0].vertexIndex);
				vert_list.append(niFaces[f].points[1].vertexIndex);
				vert_list.append(niFaces[f].points[2].vertexIndex);

				Vector3 norm1 = nif_normals[niFaces[f].points[0].normalIndex];
				Vector3 norm2 = nif_normals[niFaces[f].points[1].normalIndex];
				Vector3 norm3 = nif_normals[niFaces[f].points[2].normalIndex];

				maya_normals.append(MVector(norm1.x, norm1.y, norm1.z));
				maya_normals.append(MVector(norm2.x, norm2.y, norm2.z));
				maya_normals.append(MVector(norm3.x, norm3.y, norm3.z));
			}

			MStatus stat = meshFn.setFaceVertexNormals(maya_normals, face_list, vert_list);

			//MStatus stat = meshFn.setVertexNormals( maya_normals, vert_list );
			if (stat != MS::kSuccess)
			{
				//out << stat.errorString().asChar() << endl;
				throw runtime_error("Failed to set Normals.");
			}
		}
	}

	//out << "Importing Materials and textures" << endl;
	vector<vector<NiPropertyRef>> propGroups = bs_complex_shape.GetPropGroups();

	//Create a selection list for each prop group
	vector<MSelectionList> sel_lists(propGroups.size());
	//sel_list.add( meshPath );

	//If there is only one property group, use it on the whole mesh
	if (propGroups.size() == 1)
	{
		sel_lists[0].add(meshPath);
	}
	else
	{
		//Add the faces affected by each property group to the corresponding
		//selection list
		MItMeshPolygon gIt(meshPath);
		for (; !gIt.isDone(); gIt.next())
		{
			//out << "gIt.index():  " << gIt.index() << endl;
			unsigned prop_index = niFaces[gIt.index()].propGroupIndex;
			if (prop_index != CS_NO_INDEX)
			{
				//out << "Appending face " << gIt.index() << " to selection list " << prop_index << endl;
				sel_lists[prop_index].add(meshPath, gIt.polygon());
			}
		}
	}

	for (unsigned i = 0; i < propGroups.size(); ++i)
	{
		//--Look for Materials--//
		MObject grpOb;

		//out << "Looking for previously imported shaders..." << endl;

		vector<NiPropertyRef> property_group = propGroups[i];

		if (property_group.size() > 0)
		{
			MObject material_object;

			for (int x = 0; x < this->translatorData->importedMaterials.size(); x++)
			{
				int coincidences = 0;
				vector<NiPropertyRef> current_property_group = this->translatorData->importedMaterials[x].first;

				for (int y = 0; y < property_group.size(); y++)
				{
					for (int z = 0; z < current_property_group.size(); z++)
					{
						if (property_group[y] == current_property_group[z])
						{
							coincidences++;
						}

						string property_xx = property_group[y]->GetType().GetTypeName();
						string property_yy = current_property_group[z]->GetType().GetTypeName();
					}
				}

				if (coincidences == property_group.size())
				{
					material_object = this->translatorData->importedMaterials[x].second;
					break;
				}
			}

			vector<NifTextureConnectorRef> texture_connectors;

			for (int x = 0; x < this->translatorData->importedTextureConnectors.size(); x++)
			{
				int coincidences = 0;
				vector<NiPropertyRef> current_property_group = this->translatorData->importedTextureConnectors[x].first;

				for (int y = 0; y < property_group.size(); y++)
				{
					for (int z = 0; z < current_property_group.size(); z++)
					{
						if (property_group[y] == current_property_group[z])
						{
							coincidences++;
						}
					}
				}

				if (coincidences == property_group.size())
				{
					texture_connectors = this->translatorData->importedTextureConnectors[x].second;
					break;
				}
			}

			this->translatorUtils->ConnectShader(material_object, texture_connectors, meshPath, sel_lists[i]);
		}
	}

	//out << "Bind skin if any" << endl;
	//--Bind Skin if any--//

	vector<NiNodeRef> bone_nodes = bs_complex_shape.GetSkinInfluences();
	if (bone_nodes.size() != 0)
	{
		//Build up the MEL command string
		string cmd = "skinCluster -tsb ";

		//out << "Add list of bones to the command" << endl;
		//Add list of bones to the command
		for (unsigned int i = 0; i < bone_nodes.size(); ++i)
		{
			cmd.append(this->translatorData->importedNodes[StaticCast<NiAVObject>(bone_nodes[i])].partialPathName().asChar());
			cmd.append(" ");
		}

		//out << "Add mesh path to the command" << endl;
		//Add mesh path to the command
		cmd.append(meshPath.partialPathName().asChar());

		//out << "Execute command" << endl;
		//Execute command to create skin cluster bound to specific bones
		MStringArray result;
		MGlobal::executeCommand(cmd.c_str(), result);
		MFnSkinCluster clusterFn;

		if (this->translatorOptions->importNormalizedWeights == false)
		{
			MGlobal::executeCommand("setAttr " + clusterFn.name() + "\.normalizeWeights 0");
		}

		MSelectionList selList;
		selList.add(result[0]);
		MObject skinOb;
		selList.getDependNode(0, skinOb);
		clusterFn.setObject(skinOb);

		//out << "Get a list of all vertices in this mesh" << endl;
		//Get a list of all verticies in this mesh
		MFnSingleIndexedComponent compFn;
		MObject vertices = compFn.create(MFn::kMeshVertComponent);
		MItGeometry gIt(meshPath);
		MIntArray vertex_indices(gIt.count());
		for (int j = 0; j < gIt.count(); ++j)
		{
			vertex_indices[j] = j;
		}
		compFn.addElements(vertex_indices);

		//out << "Get weight data from NIF" << endl;
		//Get weight data from NIF
		vector<vector<float>> nif_weights(bone_nodes.size());

		//out << "Set skin weights & bind pose for each bone" << endl;
		//initialize 2D array to zeros
		for (unsigned int i = 0; i < bone_nodes.size(); ++i)
		{
			nif_weights[i].resize(nif_verts.size());
			for (unsigned j = 0; j < nif_verts.size(); ++j)
			{
				nif_weights[i][j] = 0.0f;
			}
		}


		//out << "Put data in proper slots in the 2D array" << endl;
		//Put data in proper slots in the 2D array
		for (unsigned v = 0; v < nif_verts.size(); ++v)
		{
			for (unsigned w = 0; w < nif_verts[v].weights.size(); ++w)
			{
				SkinInfluence& si = nif_verts[v].weights[w];

				nif_weights[si.influenceIndex][v] = si.weight;
			}
		}

		//out << "Build Maya influence list" << endl;
		//Build Maya influence list
		MIntArray influence_list(bone_nodes.size());
		for (unsigned int i = 0; i < bone_nodes.size(); ++i)
		{
			influence_list[i] = i;
		}

		//out << "Build Maya weight list" << endl;
		//Build Maya weight list
		MFloatArray weight_list(gIt.count() * int(bone_nodes.size()));
		int k = 0;
		for (int i = 0; i < gIt.count(); ++i)
		{
			for (int j = 0; j < int(bone_nodes.size()); ++j)
			{
				//out << "Bone:  " << bone_nodes[j] << "\tWeight:  " << nif_weights[j][i] << endl;
				weight_list[k] = nif_weights[j][i];
				++k;
			}
		}

		//out << "Send the weights to Maya" << endl;
		//Send the weights to Maya
		clusterFn.setWeights(meshPath, vertices, influence_list, weight_list, this->translatorOptions->importNormalizedWeights);
	}

	//out << "ImportMesh() end" << endl;

	//some kind of weird bug in Maya 2012
	//vertices may have their positions changed when creating a mesh so they have to be repositioned
	//may be in other Maya versions
#if MAYA_API_VERSION == 201200
	MItMeshVertex vert_iter(parent);

	int j = 0;

	while (!vert_iter.isDone()) {
		vert_iter.setPosition(maya_verts[j]);
		vert_iter.next();
		j++;
	}
#endif

	vector<Segment> segments = bs_complex_shape.GetSegments();

	if (segments.size() > 0)
	{
		MStatus status;
		MStringArray long_name;
		MStringArray short_name;
		MStringArray format_name;
		MObject proper_shape;

		MFnDagNode parent_node(parent);
		MFnDagNode check;
		for (int x = 0; x < parent_node.childCount(); x++)
		{
			MObject child = parent_node.child(x);
			check.setObject(child);
			if (child.hasFn(MFn::kMesh) && !check.isIntermediateObject())
			{
				proper_shape = child;
			}
		}

		MFnMesh blind_data_mesh;
		if (!proper_shape.isNull())
		{
			blind_data_mesh.setObject(proper_shape);
		}

		blind_data_mesh.updateSurface();

		long_name.append("subSegmentValue");
		short_name.append("sSV");
		format_name.append("int");

		int blind_data_id = 0;


		for (int segment_index = 0; segment_index < segments.size(); segment_index++)
		{
			vector<SubSegment> subSegments = segments[segment_index].subSegments;
			vector<MObject> created_sub_segments;

			if (subSegments.size() > 0)
			{
				for (int sub_segment_index = 0; sub_segment_index < subSegments.size(); sub_segment_index++)
				{
					int face_index = 0;
					MItMeshPolygon polygon_it(blind_data_mesh.object());

					MSelectionList selected_faces;
					selected_faces.add(meshPath);

					while (face_index < subSegments[sub_segment_index].polygonOffset)
					{
						if (removedFacesIndexes.find(face_index) == removedFacesIndexes.end())
						{
							polygon_it.next();
						}

						face_index++;
					}

					for (int z = 0; z < subSegments[sub_segment_index].polygonCount; z++)
					{
						if (removedFacesIndexes.find(face_index) == removedFacesIndexes.end())
						{
							selected_faces.add(meshPath, polygon_it.currentItem());
							polygon_it.next();
						}

						face_index++;
					}

					do
					{
						blind_data_id++;
						status = blind_data_mesh.createBlindDataType(blind_data_id, long_name, short_name, format_name);
					}
					while (status == MStatus::kFailure);

					status = MGlobal::clearSelectionList();
					status = MGlobal::setActiveSelectionList(selected_faces);

					MObject sub_segment_node = connectSubSegment(blind_data_mesh.object(), blind_data_id);

					if (!sub_segment_node.isNull())
					{
						created_sub_segments.push_back(sub_segment_node);
					}
				}
			}
			else
			{
				int face_index = 0;
				MItMeshPolygon polygon_it(blind_data_mesh.object());

				MSelectionList selected_faces;
				selected_faces.add(meshPath);

				while (face_index < segments[segment_index].polygonOffset)
				{
					if (removedFacesIndexes.find(face_index) == removedFacesIndexes.end())
					{
						polygon_it.next();
					}

					face_index++;
				}

				for (int z = 0; z < segments[segment_index].polygonCount; z++)
				{

					if (removedFacesIndexes.find(face_index) == removedFacesIndexes.end())
					{
						selected_faces.add(meshPath, polygon_it.currentItem());
						polygon_it.next();
					}

					face_index++;
				}

				do
				{
					blind_data_id++;
					status = blind_data_mesh.createBlindDataType(blind_data_id, long_name, short_name, format_name);
				}
				while (status == MStatus::kFailure);

				status = MGlobal::clearSelectionList();
				status = MGlobal::setActiveSelectionList(selected_faces);

				MObject sub_segment_node = connectSubSegment(blind_data_mesh.object(), blind_data_id);

				if (!sub_segment_node.isNull())
				{
					created_sub_segments.push_back(sub_segment_node);
				}
			}

			MDGModifier dg_modifier;
			MFnDependencyNode segment_node;
			segment_node.create("bsSegment");
			MPlug input_message_shape = segment_node.findPlug("targetShape");
			MPlug input_message_sub_segments = segment_node.findPlug("subSegments");

			MPlug mesh_out_message = blind_data_mesh.findPlug("message");
			dg_modifier.connect(mesh_out_message, input_message_shape);
			dg_modifier.doIt();

			for (int w = 0; w < created_sub_segments.size(); w++)
			{
				MFnDependencyNode sub_segment_node(created_sub_segments[w]);
				MPlug sub_segment_out_message = sub_segment_node.findPlug("message");

				MPlug sub_segment_in_message = input_message_sub_segments.elementByLogicalIndex(w);

				status = dg_modifier.connect(sub_segment_out_message, sub_segment_in_message);
			}
		}
	}

	return meshPath;
}

string NifMeshImporterFallout4::asString(bool verbose /*= false */) const
{
	stringstream out;

	out << NifMeshImporter::asString(verbose) << endl;
	out << "NifMeshImporterFallout4" << endl;

	return out.str();
}

const Type& NifMeshImporterFallout4::GetType() const
{
	return TYPE;
}

const Type NifMeshImporterFallout4::TYPE("NifMeshImporterFallout4", &NifMeshImporter::TYPE);
