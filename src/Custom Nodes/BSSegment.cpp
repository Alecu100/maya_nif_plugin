#include "include/Custom Nodes/BSSegment.h"

BSSegment::BSSegment()
{
}

BSSegment::~BSSegment()
{
}

MStatus BSSegment::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	return MStatus::kSuccess;
}

void* BSSegment::creator()
{
	return new BSSegment();
}

MStatus BSSegment::initialize()
{
	MFnMessageAttribute target_shape_attribute;
	MFnMessageAttribute subsegments_attribute;

	MStatus status;

	targetShape = target_shape_attribute.create("targetShape", "tS", &status);
	subSegments = subsegments_attribute.create("subSegments", "sS", &status);


	target_shape_attribute.setWritable(true);
	target_shape_attribute.setConnectable(true);
	target_shape_attribute.setStorable(true);

	status = subsegments_attribute.setArray(true);
	status = subsegments_attribute.setReadable(true);
	status = subsegments_attribute.setConnectable(true);
	status = subsegments_attribute.setStorable(true);
	status = subsegments_attribute.setUsesArrayDataBuilder(true);

	status = MPxNode::addAttribute(targetShape);
	status = MPxNode::addAttribute(subSegments);

	return MStatus::kSuccess;
}


MObject BSSegment::targetShape;

MObject BSSegment::subSegments;

MTypeId BSSegment::id(0x7ff23);
