#include "include/Exporters/NifMeshExporterFallout4.h"

NifMeshExporterFallout4::NifMeshExporterFallout4()
{
}

NifMeshExporterFallout4::NifMeshExporterFallout4(NifNodeExporterRef node_exporter, NifTranslatorOptionsRef translator_options, NifTranslatorDataRef translator_data, NifTranslatorUtilsRef translator_utils)
{
	this->translatorOptions = translator_options;
	this->translatorData = translator_data;
	this->translatorUtils = translator_utils;
	this->nodeExporter = node_exporter;
}

bool NifMeshExporterFallout4::CreateComplexFace(MStatus& stat, MFnMesh meshFn, vector<Color4> niColors, MItMeshPolygon itPoly, ComplexFace& cf)
{
	int poly_vert_count = itPoly.polygonVertexCount(&stat);

	if (stat != MS::kSuccess)
	{
		throw runtime_error("Failed to get vertex count.");
	}

	//Ignore polygons with less than 3 vertices
	if (poly_vert_count < 3)
	{
		return true;
	}

	//Assume all faces use material 0 for now
	cf.propGroupIndex = 0;

	for (int i = 0; i < poly_vert_count; ++i)
	{
		ComplexPoint cp;

		cp.vertexIndex = itPoly.vertexIndex(i);
		cp.normalIndex = itPoly.normalIndex(i);
		if (niColors.size() > 0)
		{
			int color_index;
			stat = meshFn.getFaceVertexColorIndex(itPoly.index(), i, color_index);
			if (stat != MS::kSuccess)
			{
				//out << stat.errorString().asChar() << endl;
				throw runtime_error("Failed to get vertex color.");
			}
			cp.colorIndex = color_index;
		}

		//Get the UV set names used by this particular vertex
		MStringArray vertUvSetNames;
		itPoly.getUVSetNames(vertUvSetNames);
		for (unsigned int j = 0; j < vertUvSetNames.length(); ++j)
		{
			TexCoordIndex tci;
			tci.texCoordSetIndex = 0;
			int uv_index;
			itPoly.getUVIndex(i, uv_index, &vertUvSetNames[j]);
			tci.texCoordIndex = uv_index;
			cp.texCoordIndices.push_back(tci);
		}
		cf.points.push_back(cp);
	}

	return false;
}

void NifMeshExporterFallout4::ExportMesh(MObject dagNode)
{
	//out << "NifTranslator::ExportMesh {";
	BSComplexShape bs_complex_shape;
	MStatus stat;
	MObject mesh;

	//Find Mesh child of given transform object
	MFnDagNode nodeFn(dagNode);

	bs_complex_shape.SetName(this->translatorUtils->MakeNifName(nodeFn.name()));


	for (int i = 0; i != nodeFn.childCount(); ++i)
	{
		// get a handle to the child
		if (nodeFn.child(i).hasFn(MFn::kMesh))
		{
			MFnMesh tempFn(nodeFn.child(i));
			//No history items
			if (!tempFn.isIntermediateObject())
			{
				//out << "Found a mesh child." << endl;
				mesh = nodeFn.child(i);
				break;
			}
		}
	}

	MFnMesh visibleMeshFn(mesh, &stat);
	if (stat != MS::kSuccess)
	{
		//out << stat.errorString().asChar() << endl;
		throw runtime_error("Failed to create visibleMeshFn.");
	}

	//out << visibleMeshFn.name().asChar() << ") {" << endl;
	MFnMesh meshFn;
	MObject dataObj;
	MPlugArray inMeshPlugArray;
	MPlug childPlug;
	MPlug geomPlug;
	MPlug inputPlug;

	// this will hold the returned vertex positions
	MPointArray vts;

	//For now always use the visible mesh
	meshFn.setObject(mesh);

	//out << "Use the function set to get the points" << endl;
	// use the function set to get the points
	stat = meshFn.getPoints(vts);
	if (stat != MS::kSuccess)
	{
		//out << stat.errorString().asChar() << endl;
		throw runtime_error("Failed to get points.");
	}

	//Maya won't store any information about objects with no vertices.  Just skip it.
	if (vts.length() == 0)
	{
		MGlobal::displayWarning("An object in this scene has no vertices.  Nothing will be exported.");
		return;
	}

	vector<WeightedVertex> nif_vts(vts.length());
	for (int i = 0; i != vts.length(); ++i)
	{
		nif_vts[i].position.x = float(vts[i].x);
		nif_vts[i].position.y = float(vts[i].y);
		nif_vts[i].position.z = float(vts[i].z);
	}

	//Set vertex info later since it includes skin weights
	//cs.SetVertices( nif_vts );

	//out << "Use the function set to get the colors" << endl;
	MColorArray myColors;
	meshFn.getFaceVertexColors(myColors);

	//out << "Prepare NIF color vector" << endl;
	vector<Color4> niColors(myColors.length());
	for (unsigned int i = 0; i < myColors.length(); ++i)
	{
		niColors[i] = Color4(myColors[i].r, myColors[i].g, myColors[i].b, myColors[i].a);
	}
	bs_complex_shape.SetColors(niColors);


	// this will hold the returned vertex positions
	MFloatVectorArray nmls;

	//out << "Use the function set to get the normals" << endl;
	// use the function set to get the normals
	stat = meshFn.getNormals(nmls, MSpace::kTransform);
	if (stat != MS::kSuccess)
	{
		//out << stat.errorString().asChar() << endl;
		throw runtime_error("Failed to get normals");
	}

	//out << "Prepare NIF normal vector" << endl;
	vector<Vector3> nif_nmls(nmls.length());
	for (int i = 0; i != nmls.length(); ++i)
	{
		nif_nmls[i].x = float(nmls[i].x);
		nif_nmls[i].y = float(nmls[i].y);
		nif_nmls[i].z = float(nmls[i].z);
	}
	bs_complex_shape.SetNormals(nif_nmls);

	//out << "Use the function set to get the UV set names" << endl;
	MStringArray uvSetNames;
	MString baseUVSet;
	MFloatArray myUCoords;
	MFloatArray myVCoords;
	bool has_uvs = false;

	// get the names of the uv sets on the mesh
	meshFn.getUVSetNames(uvSetNames);

	vector<TexCoordSet> nif_uvs;

	if (uvSetNames.length() > 0)
	{
		//Get the UVs
		meshFn.getUVs(myUCoords, myVCoords, &uvSetNames[0]);

		//Store the data
		vector<TexCoord> tex_coords;
		tex_coords.resize(myUCoords.length());
		for (unsigned int j = 0; j < myUCoords.length(); ++j)
		{
			tex_coords[j].u = myUCoords[j];
			//Flip the V coords
			tex_coords[j].v = 1.0f - myVCoords[j];
		}

		baseUVSet = uvSetNames[0];

		bs_complex_shape.SetTexCoordSet(tex_coords);
	}


	// this will hold references to the shaders used on the meshes
	MObjectArray shaders;

	// this is used to hold indices to the materials returned in the object array
	MIntArray face_indices;

	//out << "Get the connected shaders" << endl;
	// get the shaders used by the i'th mesh instance
	// Assume this is not instanced for now
	// TODO support instancing properly
	stat = visibleMeshFn.getConnectedShaders(0, shaders, face_indices);

	if (stat != MS::kSuccess)
	{
		//out << stat.errorString().asChar() << endl;
		throw runtime_error("Failed to get connected shader list.");
	}

	vector<ComplexFace> nif_faces;

	//Add shaders to propGroup array
	vector<vector<NiPropertyRef>> propGroups;
	for (unsigned int shader_num = 0; shader_num < shaders.length(); ++shader_num)
	{
		//Maya sometimes lists shaders that are not actually attached to any face.  Disregard them.
		bool shader_is_used = false;
		for (size_t f = 0; f < face_indices.length(); ++f)
		{
			if (face_indices[f] == shader_num)
			{
				shader_is_used = true;
				break;
			}
		}

		if (shader_is_used == false)
		{
			//Shader isn't actually used, so continue to the next one.
			continue;
		}

		//out << "Found attached shader:  ";
		//Attach all properties previously associated with this shader to
		//this NiTriShape
		MFnDependencyNode fnDep(shaders[shader_num]);

		//Find the shader that this shading group connects to
		MPlug p = fnDep.findPlug("surfaceShader");
		MPlugArray plugs;
		p.connectedTo(plugs, true, false);
		for (unsigned int i = 0; i < plugs.length(); ++i)
		{
			if (plugs[i].node().hasFn(MFn::kLambert))
			{
				fnDep.setObject(plugs[i].node());
				break;
			}
		}

		//out << fnDep.name().asChar() << endl;
		vector<NiPropertyRef> niProps = this->translatorData->shaders[fnDep.name().asChar()];

		propGroups.push_back(niProps);
	}
	bs_complex_shape.SetPropGroups(propGroups);

	//out << "Export vertex and normal data" << endl;
	// attach an iterator to the mesh
	MItMeshPolygon itPoly(mesh, &stat);
	if (stat != MS::kSuccess)
	{
		throw runtime_error("Failed to create polygon iterator.");
	}

	MPlugArray connected_segment_plugs;
	MObjectArray segment_nodes;
	meshFn.findPlug("message").connectedTo(connected_segment_plugs, false, true);

	for (int x = 0; x < connected_segment_plugs.length(); x++)
	{
		MFnDependencyNode dependency_node(connected_segment_plugs[x].node());
		if (dependency_node.typeName() == "bsSegment")
		{
			segment_nodes.append(dependency_node.object());
		}
	}

	if (segment_nodes.length() == 0)
	{
		// Create a list of faces with vertex IDs, and duplicate normals so they have the same ID
		for (; !itPoly.isDone(); itPoly.next())
		{
			ComplexFace cf;
			if (CreateComplexFace(stat, meshFn.object(), niColors, itPoly, cf)) continue;
			nif_faces.push_back(cf);
		}
	}
	else
	{
		MStatus status;
		int blind_data_value;
		int segment_polygon_offset = 0;
		int segment_polygon_count;
		map<int, bool> processed_faces_indices;
		vector<Segment> segments;

		for (int segment_node_index = 0; segment_node_index < segment_nodes.length(); segment_node_index++)
		{
			Segment segment;
			segment.polygonOffset = segment_polygon_offset;
			segment_polygon_count = 0;

			MFnDependencyNode segment_fn(segment_nodes[segment_node_index]);
			MPlug sub_segments_plug = segment_fn.findPlug("subSegments");
			MIntArray sub_segments_plugs_indices;
			MItMeshPolygon it_polygons(meshFn.object());
			sub_segments_plug.getExistingArrayAttributeIndices(sub_segments_plugs_indices);

			for (int sub_segment_plug_index = 0; sub_segment_plug_index < sub_segments_plugs_indices.length(); sub_segment_plug_index++)
			{
				MPlug sub_segment_connection = sub_segments_plug.elementByLogicalIndex(sub_segments_plugs_indices[sub_segment_plug_index]);
				MPlugArray sub_segments_connections;

				sub_segment_connection.connectedTo(sub_segments_connections, false, true);

				if (sub_segments_connections.length() > 1)
				{
					for (int sub_segment_index = 0; sub_segment_index < sub_segments_connections.length(); sub_segment_index++)
					{
						MFnDependencyNode sub_segment_node(sub_segments_connections[sub_segment_index].node());
						MPlug target_face_plug = sub_segment_node.findPlug("targetFaces");
						MPlugArray blind_data_connections;

						target_face_plug.connectedTo(blind_data_connections, false, true);

						if (blind_data_connections.length() > 0)
						{
							int sub_segment_polygon_count = 0;
							MFnDependencyNode blind_data_node(blind_data_connections[0].node());

							int blind_data_id = blind_data_node.findPlug("typeId").asInt();
							for (it_polygons.reset(); !it_polygons.isDone(); it_polygons.next())
							{
								if (it_polygons.polygonVertexCount() >= 3)
								{
									status = meshFn.getIntBlindData(it_polygons.index(), MFn::Type::kMeshPolygonComponent, blind_data_id, "subSegmentValue", blind_data_value);
									if (status == MStatus::kSuccess && blind_data_value == 1 &&
										meshFn.hasBlindDataComponentId(it_polygons.index(), MFn::Type::kMeshPolygonComponent, blind_data_id))
									{
										if (processed_faces_indices.find(it_polygons.index()) == processed_faces_indices.end())
										{
											ComplexFace cf;
											if (CreateComplexFace(stat, meshFn.object(), niColors, it_polygons, cf)) continue;
											nif_faces.push_back(cf);

											processed_faces_indices[it_polygons.index()] = true;
											segment_polygon_count++;
											sub_segment_polygon_count++;
										}
									}
								}
							}

							SubSegment sub_segment;
							sub_segment.polygonCount = sub_segment_polygon_count;
							sub_segment.polygonOffset = segment_polygon_offset;
							segment.subSegments.push_back(sub_segment);

							segment_polygon_offset += sub_segment_polygon_count;
						}
					}
				}
				else if (sub_segments_connections.length()  == 1)
				{
					segment_polygon_count = 0;
					MFnDependencyNode sub_segment_node(sub_segments_connections[0].node());
					MPlug target_face_plug = sub_segment_node.findPlug("targetFaces");
					MPlugArray blind_data_connections;

					target_face_plug.connectedTo(blind_data_connections, false, true);

					if (blind_data_connections.length() > 0)
					{

						MFnDependencyNode blind_data_node(blind_data_connections[0].node());

						int blind_data_id = blind_data_node.findPlug("typeId").asInt();
						for (it_polygons.reset(); !it_polygons.isDone(); it_polygons.next())
						{
							if (it_polygons.polygonVertexCount() >= 3)
							{
								status = meshFn.getIntBlindData(it_polygons.index(), MFn::Type::kMeshPolygonComponent, blind_data_id, "subSegmentValue", blind_data_value);
								if (status == MStatus::kSuccess && blind_data_value == 1 &&
									meshFn.hasBlindDataComponentId(it_polygons.index(), MFn::Type::kMeshPolygonComponent, blind_data_id))
								{
									if (processed_faces_indices.find(it_polygons.index()) == processed_faces_indices.end())
									{
										ComplexFace cf;
										if (CreateComplexFace(stat, meshFn.object(), niColors, it_polygons, cf)) continue;
										nif_faces.push_back(cf);

										processed_faces_indices[it_polygons.index()] = true;
										segment_polygon_count++;
									}
								}
							}
						}

						segment_polygon_offset += segment_polygon_count;
					}
				}
			}

			segment.polygonCount = segment_polygon_count;
			segments.push_back(segment);
		}
	}


	//Set shader/face association
	if (nif_faces.size() != face_indices.length())
	{
		throw runtime_error("Num faces found do not match num faces reported.");
	}
	for (unsigned int face_index = 0; face_index < nif_faces.size(); ++face_index)
	{
		nif_faces[face_index].propGroupIndex = face_indices[face_index];
	}

	bs_complex_shape.SetFaces(nif_faces);

	//--Skin Processing--//

	//Look up any skin clusters
	if (this->translatorData->meshClusters.find(visibleMeshFn.fullPathName().asChar()) != this->translatorData->meshClusters.end())
	{
		const vector<MObject>& clusters = this->translatorData->meshClusters[visibleMeshFn.fullPathName().asChar()];
		if (clusters.size() > 1)
		{
			throw runtime_error("Objects with multiple skin clusters affecting them are not currently supported.  Try deleting the history and re-binding them.");
		}

		vector<MObject>::const_iterator cluster = clusters.begin();
		if (cluster->isNull() != true)
		{
			MFnSkinCluster clusterFn(*cluster);


			//out << "Processing skin..." << endl;
			//Get path to visible mesh
			MDagPath meshPath;
			visibleMeshFn.getPath(meshPath);

			//out << "Getting a list of all verticies in this mesh" << endl;
			//Get a list of all vertices in this mesh
			MFnSingleIndexedComponent compFn;
			MObject vertices = compFn.create(MFn::kMeshVertComponent);
			MItGeometry gIt(meshPath);
			MIntArray vertex_indices(gIt.count());
			for (int vert_index = 0; vert_index < gIt.count(); ++vert_index)
			{
				vertex_indices[vert_index] = vert_index;
			}
			compFn.addElements(vertex_indices);

			//out << "Getting Influences" << endl;
			//Get influences
			MDagPathArray myBones;
			clusterFn.influenceObjects(myBones, &stat);

			//out << "Creating a list of NiNodeRefs of influences." << endl;
			//Create list of NiNodeRefs of influences
			vector<NiNodeRef> niBones(myBones.length());
			for (unsigned int bone_index = 0; bone_index < niBones.size(); ++bone_index)
			{
				const char* boneName = myBones[bone_index].fullPathName().asChar();
				if (this->translatorData->nodes.find(myBones[bone_index].fullPathName().asChar()) == this->translatorData->nodes.end())
				{
					//There is a problem; one of the joints was not exported.  Abort.
					throw runtime_error("One of the joints necessary to export a bound skin was not exported.");
				}
				niBones[bone_index] = this->translatorData->nodes[myBones[bone_index].fullPathName().asChar()];
			}

			//out << "Getting weights from Maya" << endl;
			//Get weights from Maya
			MDoubleArray myWeights;
			unsigned int bone_count = myBones.length();
			stat = clusterFn.getWeights(meshPath, vertices, myWeights, bone_count);
			if (stat != MS::kSuccess)
			{
				//out << stat.errorString().asChar() << endl;
				throw runtime_error("Failed to get vertex weights.");
			}

			//out << "Setting skin influence list in ComplexShape" << endl;
			//Set skin information in ComplexShape
			bs_complex_shape.SetSkinInfluences(niBones);

			//out << "Adding weights to ComplexShape vertices" << endl;
			//out << "Number of weights:  " << myWeights.length() << endl;
			//out << "Number of bones:  " << myBones.length() << endl;
			//out << "Number of Maya vertices:  " << gIt.count() << endl;
			//out << "Number of NIF vertices:  " << int(nif_vts.size()) << endl;
			unsigned int weight_index = 0;
			SkinInfluence sk;
			for (unsigned int vert_index = 0; vert_index < nif_vts.size(); ++vert_index)
			{
				for (unsigned int bone_index = 0; bone_index < myBones.length(); ++bone_index)
				{
					//out << "vert_index:  " << vert_index << "  bone_index:  " << bone_index << "  weight_index:  " << weight_index << endl;	
					// Only bother with weights that are significant
					if (myWeights[weight_index] > 0.0f)
					{
						sk.influenceIndex = bone_index;
						sk.weight = float(myWeights[weight_index]);

						nif_vts[vert_index].weights.push_back(sk);
					}
					++weight_index;
				}
			}
		}
	}

	//out << "Setting vertex info" << endl;
	//Set vertex info now that any skins have been processed
	bs_complex_shape.SetVertices(nif_vts);

	//ComplexShape is now complete, so split it

	//Get parent
	NiNodeRef parNode = this->translatorUtils->GetDAGParent(dagNode);
	Matrix44 transform = Matrix44::IDENTITY;
	vector<NiNodeRef> influences = bs_complex_shape.GetSkinInfluences();
	if (influences.size() > 0)
	{
		//This is a skin, so we use the common ancestor of all influences
		//as the parent
		vector<NiAVObjectRef> objects;
		for (size_t i = 0; i < influences.size(); ++i)
		{
			objects.push_back(StaticCast<NiAVObject>(influences[i]));
		}

		//Get world transform of existing parent
		Matrix44 oldParWorld = parNode->GetWorldTransform();

		//Set new parent node
		parNode = FindCommonAncestor(objects);

		transform = oldParWorld * parNode->GetWorldTransform().Inverse();
	}

	//Get transform using temporary NiAVObject
	NiAVObjectRef tempAV = new NiAVObject;
	this->nodeExporter->ExportAV(tempAV, dagNode);

	NiAVObjectRef avObj;
	if (this->translatorOptions->exportTangentSpace == "falloutskyrimtangentspace")
	{
		//out << "Split ComplexShape from " << meshFn.name().asChar() << endl;
		avObj = bs_complex_shape.Split(parNode, tempAV->GetLocalTransform() * transform, this->translatorOptions->exportBonesPerSkinPartition,
		                               this->translatorOptions->exportAsTriStrips, true, this->translatorOptions->exportMinimumVertexWeight, 16);
	}
	else
	{
		avObj = bs_complex_shape.Split(parNode, tempAV->GetLocalTransform() * transform, this->translatorOptions->exportBonesPerSkinPartition,
		                               this->translatorOptions->exportAsTriStrips, false, this->translatorOptions->exportMinimumVertexWeight);
	}


	//out << "Get the NiAVObject portion of the root of the split" <<endl;
	//Get the NiAVObject portion of the root of the split
	avObj->SetName(tempAV->GetName());
	avObj->SetVisibility(tempAV->GetVisibility());
	avObj->SetFlags(tempAV->GetFlags());

	//If polygon mesh is hidden, hide tri_shape
	MPlug vis = visibleMeshFn.findPlug(MString("visibility"));
	bool visibility;
	vis.getValue(visibility);


	NiNodeRef splitRoot = DynamicCast<NiNode>(avObj);
	if (splitRoot != NULL)
	{
		//Root is a NiNode with NiTriBasedGeom children.
		vector<NiAVObjectRef> children = splitRoot->GetChildren();
		for (unsigned c = 0; c < children.size(); ++c)
		{
			//Set the default collision propogation flag to "use triangles"
			children[c]->SetFlags(2);
			// Make the mesh invisible if necessary
			if (visibility == false)
			{
				children[c]->SetVisibility(false);
			}
		}
	}
	else
	{
		//Root must be a NiTriBasedGeom.  Make it invisible if necessary
		if (visibility == false)
		{
			avObj->SetVisibility(false);
		}
	}
}

string NifMeshExporterFallout4::asString(bool verbose /*= false */) const
{
	stringstream out;

	out << NifMeshExporter::asString(verbose) << endl;
	out << "NifMeshExporterFallout4" << endl;

	return out.str();
}

const Type& NifMeshExporterFallout4::GetType() const
{
	return TYPE;
}

const Type NifMeshExporterFallout4::TYPE("NifMeshExporterFallout4", &NifMeshExporter::TYPE);
