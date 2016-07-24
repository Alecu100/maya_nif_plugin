#include "include/Custom Nodes/BSSubSegment.h"

BSSubSegment::BSSubSegment()
{
}

BSSubSegment::~BSSubSegment()
{
}

MStatus BSSubSegment::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	return MStatus::kSuccess;
}

void* BSSubSegment::creator()
{
	return new BSSubSegment();
}

MStatus BSSubSegment::initialize()
{
	MFnMessageAttribute target_shape_attribute;
	MFnMessageAttribute target_faces_attribute;

	MStatus status;

	target_shape_attribute.setWritable(true);
	target_shape_attribute.setConnectable(true);
	target_shape_attribute.setStorable(true);
	target_faces_attribute.setStorable(true);
	target_faces_attribute.setConnectable(true);
	target_faces_attribute.setWritable(true);

	targetShape = target_shape_attribute.create("targetShape", "tS", &status);
	targetFaces = target_faces_attribute.create("targetFaces", "tF", &status);

	status = MPxNode::addAttribute(targetShape);
	status = MPxNode::addAttribute(targetFaces);

	return MStatus::kSuccess;
}


MObject BSSubSegment::targetShape;

MObject BSSubSegment::targetFaces;

MTypeId BSSubSegment::id(0x7ff19);


