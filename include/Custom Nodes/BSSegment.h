#ifndef _BSSEGMENT_H
#define _BSSEGMENT_H

#include <maya/MDagPath.h>
#include <maya/MDagPathArray.h>
#include <maya/MEulerRotation.h>
#include <maya/MFileObject.h>
#include <maya/MFloatArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MFnBase.h>
#include <maya/MFnComponent.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnData.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnIkJoint.h>
#include <maya/MFnMatrixData.h>
#include <maya/MFnMesh.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnPhongShader.h>
#include <maya/MFnSet.h>
#include <maya/MFnSingleIndexedComponent.h>
#include <maya/MFnSkinCluster.h>
#include <maya/MFnTransform.h>
#include <maya/MFStream.h>
#include <maya/MGlobal.h>
#include <maya/MIntArray.h>
#include <maya/MIOStream.h> 
#include <maya/MItDag.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MItDependencyNodes.h>
#include <maya/MItGeometry.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MItSelectionList.h>
#include <maya/MMatrix.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MPointArray.h>
#include <maya/MPxFileTranslator.h>
#include <maya/MQuaternion.h>
#include <maya/MSelectionList.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MStringArray.h>
#include <maya/MVector.h>
#include <maya/MFnAnimCurve.h>
#include <maya/MAnimUtil.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MItMeshVertex.h>
#include <maya/MPxNode.h>
#include <maya/MFnMessageAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnMessageAttribute.h>

#include <gen/enums.h>

using namespace Niflib;

class BSSegment : public MPxNode
{
public:

	BSSegment();

	virtual ~BSSegment();

	virtual MStatus compute(const MPlug& plug, MDataBlock& dataBlock);

	static void* creator();

	static MStatus initialize();

public:

	static MObject targetShape;

	static MObject subSegments;

	static MTypeId id;
};

#endif
