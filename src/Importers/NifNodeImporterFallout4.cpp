#include "include/Importers/NifNodeImporterFallout4.h"
#include <queue>
#include <obj/BSSubIndexTriShape.h>
#include <obj/BSSkin__Instance.h>

bool NifNodeImporterFallout4::isUsedInBoneData(NiNodeRef target_node)
{
	if (this->translatorData->importedSceneRoot != NULL)
	{
		queue<NiAVObjectRef> nodes_to_scan;
		NiAVObjectRef initial_root = DynamicCast<NiAVObject>(this->translatorData->importedSceneRoot);
		nodes_to_scan.push(initial_root);

		while (!nodes_to_scan.empty())
		{
			NiAVObjectRef av_object = nodes_to_scan.front();
			nodes_to_scan.pop();

			BSShapeRef bs_shape = DynamicCast<BSShape>(av_object);

			if (bs_shape != NULL)
			{
				if (bs_shape->IsSkin())
				{
					if (bs_shape->IsSameType(BSSubIndexTriShape::TYPE))
					{
						BSSubIndexTriShapeRef bs_sub_index_tri_shape = DynamicCast<BSSubIndexTriShape>(bs_shape);
						BSSkin__InstanceRef skinInst = DynamicCast<BSSkin__Instance>(bs_sub_index_tri_shape->GetSkin());
						vector<Ref<NiNode>> bones = skinInst->GetBones();

						for (int bone_index = 0; bone_index < bones.size(); bone_index++)
						{
							if (bones[bone_index] == target_node)
							{
								return true;
							}
						}
					}

					if (bs_shape->IsSameType(BSTriShape::TYPE))
					{
						BSTriShapeRef bs_tri_shape = DynamicCast<BSTriShape>(bs_shape);
						BSSkin__InstanceRef skinInst = DynamicCast<BSSkin__Instance>(bs_tri_shape->GetSkin());
						vector<Ref<NiNode>> bones = skinInst->GetBones();

						for (int bone_index = 0; bone_index < bones.size(); bone_index++)
						{
							if (bones[bone_index] == target_node)
							{
								return true;
							}
						}
					}
				}
			}

			NiNodeRef node = DynamicCast<NiNode>(av_object);

			if (node != NULL)
			{
				vector<Ref<NiAVObject>> ni_av_objects = node->GetChildren();

				for (int child_index = 0; child_index < ni_av_objects.size(); child_index++)
				{
					nodes_to_scan.push(ni_av_objects[child_index]);
				}
			}
		}
	}

	return false;
}

void NifNodeImporterFallout4::ImportNodes(NiAVObjectRef niAVObj, map<NiAVObjectRef, MDagPath>& objs, MObject parent)
{
	MObject obj;

	//out << "Importing " << niAVObj << endl;

	////Stop at a non-node
	//if ( node == NULL )
	//	return;

	//Check if this node is a skinned NiTriBasedGeom.  If so, parent it to the scene instead
	//of whereever the NIF file has it.

	bool is_skin = false;

	vector<NiAVObjectRef> nodesToTest;

	if (niAVObj->IsDerivedType(NiNode::TYPE))
	{
		NiNodeRef nnr = DynamicCast<NiNode>(niAVObj);
		if (nnr->IsSplitMeshProxy())
		{
			//Test all children
			nodesToTest = nnr->GetChildren();
		}
	}
	else if (niAVObj->IsDerivedType(BSShape::TYPE))
	{
		//This is a shape, so test it.
		nodesToTest.push_back(niAVObj);
	}

	for (size_t i = 0; i < nodesToTest.size(); ++i)
	{
		BSShapeRef bs_shape = DynamicCast<BSShape>(nodesToTest[i]);
		if (bs_shape && bs_shape->IsSkin())
		{
			is_skin = true;
			break;
		}
	}

	if (is_skin)
	{
		parent = MObject::kNullObj;
	}

	//This must be a node, so process its basic attributes	
	MFnTransform transFn;
	MString name = this->translatorUtils->MakeMayaName(niAVObj->GetName());
	string strName = name.asChar();

	int flags = niAVObj->GetFlags();


	NiNodeRef niNode = DynamicCast<NiNode>(niAVObj);

	//Determine whether this node is an IK joint
	bool is_joint = false;
	if (niNode != NULL)
	{
		if (this->translatorOptions->jointMatch != "" && strName.find(this->translatorOptions->jointMatch) != string::npos)
		{
			is_joint = true;
		}
		else if (this->isUsedInBoneData(niNode))
		{
			is_joint = true;
		}

		if (this->translatorOptions->importAllNodesAsJoints == true) {
			is_joint = true;
		}
	}

	//Check if the user wants us to try to combine new skins with an
	//existing skeleton
	if (this->translatorOptions->importCombineSkeletons)
	{
		//Check if joint already exits in scene.  If so, use it.
		if (is_joint)
		{
			obj = this->translatorUtils->GetExistingJoint(name.asChar());
		}
	}

	if (obj.isNull() == false)
	{
		//A matching object was found
		transFn.setObject(obj);
	}
	else
	{
		if (is_joint)
		{
			// This is a bone, create an IK joint parented to the given parent
			MFnIkJoint IKjointFn;
			obj = IKjointFn.create(parent);

			//Set Transform Fn to work on the new IK joint
			transFn.setObject(obj);
		}
		else
		{
			//Not a bone, create a transform node parented to the given parent
			obj = transFn.create(parent);
		}

		//--Set the Transform Matrix--//
		Matrix44 transform;

		if (is_skin)
		{
			transform = niAVObj->GetWorldTransform();
		}
		else
		{
			transform = niAVObj->GetLocalTransform();
		}

		//put matrix into a float array
		float trans_arr[4][4];
		transform.AsFloatArr(trans_arr);

		transFn.set(MTransformationMatrix(MMatrix(trans_arr)));
		transFn.setRotationOrder(MTransformationMatrix::kXYZ, false);

		//Set name
		MFnDependencyNode dnFn;
		dnFn.setObject(obj);
		dnFn.setName(name);
	}

	//Add the newly created object to the objects list
	MDagPath path;
	transFn.getPath(path);
	objs[niAVObj] = path;

	//Check if this object has a bounding box
	if (niAVObj->HasBoundingBox())
	{
		//Get bounding box info
		BoundingBox bb = niAVObj->GetBoundingBox();

		//Create a transform node to move the bounding box around
		MFnTransform tranFn;
		tranFn.create(obj);
		tranFn.setName("BoundingBox");
		Matrix44 bbTrans(bb.translation, bb.rotation, 1.0f);
		//out << "bbTrans:" << endl << bbTrans << endl;
		//out << "Translation:  " << bb.translation << endl;
		//out << "Rotation:  " << bb.rotation << endl;

		//put matrix into a float array
		float bb_arr[4][4];
		bbTrans.AsFloatArr(bb_arr);
		MStatus stat = tranFn.set(MTransformationMatrix(MMatrix(bb_arr)));
		if (stat != MS::kSuccess)
		{
			//out << stat.errorString().asChar() << endl;
			throw runtime_error("Failed to set bounding box transforms.");
		}
		tranFn.setRotationOrder(MTransformationMatrix::kXYZ, false);

		//Create an implicit box parented to the node we just created
		MFnDagNode dagFn;
		dagFn.create("implicitBox", tranFn.object());
		dagFn.findPlug("sizeX").setValue(bb.radius.x);
		dagFn.findPlug("sizeY").setValue(bb.radius.y);
		dagFn.findPlug("sizeZ").setValue(bb.radius.z);
	}

	//Check to see if this is a mesh
	if (niAVObj->IsDerivedType(BSShape::TYPE))
	{
		//This is a mesh, so add it to the mesh list
		this->translatorData->importedMeshes.push_back(pair<NiAVObjectRef, MObject>(niAVObj, obj));
	}

	if (niNode != NULL)
	{
		//Check to see if this is a mesh proxy
		if (niNode->IsSplitBSMeshProxy())
		{
			//out << niNode << " is a split mesh proxy." << endl;
			//Since this is a mesh proxy, treat it like a mesh and do not
			//call this function on any children
			this->translatorData->importedMeshes.push_back(pair<NiAVObjectRef, MObject>(niAVObj, obj));
		}
		else
		{
			//Call this function for any children
			vector<NiAVObjectRef> children = niNode->GetChildren();
			for (unsigned int i = 0; i < children.size(); ++i)
			{
				this->ImportNodes(children[i], objs, obj);
			}
		}
	}
}

NifNodeImporterFallout4::~NifNodeImporterFallout4()
{
}

NifNodeImporterFallout4::NifNodeImporterFallout4()
{
}

NifNodeImporterFallout4::NifNodeImporterFallout4(NifTranslatorOptionsRef translatorOptions, NifTranslatorDataRef translatorData, NifTranslatorUtilsRef translatorUtils) : NifNodeImporter(translatorOptions, translatorData, translatorUtils)
{
}

std::string NifNodeImporterFallout4::asString(bool verbose /*= false */) const
{
	stringstream out;

	out << NifNodeImporter::asString(verbose);
	out << "NifNodeImporterFallout4" << endl;

	return out.str();
}

const Type& NifNodeImporterFallout4::GetType() const
{
	return TYPE;
}

const Type NifNodeImporterFallout4::TYPE("NifTranslatorNodeFallout4Importer", &NifTranslatorFixtureItem::TYPE);
