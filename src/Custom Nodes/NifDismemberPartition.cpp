#include "include/Custom Nodes/NifDismemberPartition.h"


NifDismemberPartition::NifDismemberPartition() {

}

NifDismemberPartition::~NifDismemberPartition() {

}

MStatus NifDismemberPartition::compute( const MPlug& plug, MDataBlock& dataBlock ) {
	return MStatus::kSuccess;
}

void* NifDismemberPartition::creator() {
	return new NifDismemberPartition();
}

MStatus NifDismemberPartition::initialize() {
	MFnTypedAttribute body_parts_flags_attribute;
	MFnTypedAttribute parts_flags_attribute;
	MFnMessageAttribute target_faces_attribute;
	MFnMessageAttribute target_shape_attribute;

	MStatus status;

	status = target_faces_attribute.setStorable(true);
	status = target_faces_attribute.setConnectable(true);
	status = target_faces_attribute.setWritable(true);
	status = target_shape_attribute.setWritable(true);
	status = target_shape_attribute.setConnectable(true);
	status = target_shape_attribute.setStorable(true);

	status = body_parts_flags_attribute.setStorable(true);
	status = parts_flags_attribute.setStorable(true);

	targetFaces = target_faces_attribute.create("targetFaces", "tF", &status);
	targetShape = target_shape_attribute.create("targetShape", "tS", &status);
	partsFlags = parts_flags_attribute.create("partsFlags", "pF", MFnData::kStringArray, &status);
	bodyPartsFlags = body_parts_flags_attribute.create("bodyPartsFlags", "bPF", MFnData::kStringArray, &status);

	status = MPxNode::addAttribute(targetFaces);
	status = MPxNode::addAttribute(targetShape);
	status = MPxNode::addAttribute(partsFlags);
	status = MPxNode::addAttribute(bodyPartsFlags);

	return MStatus::kSuccess;
}

MStringArray NifDismemberPartition::bodyPartTypeToStringArray( BSDismemberBodyPartType body_parts ) {
	MStringArray ret;

	if(body_parts == 0) {
		ret.append("BP_TORSO");
		return ret;
	}

	bool is_done = false;
	while(is_done == false) {
		is_done = true;

		if((BP_SECTIONCAP_HEAD & body_parts) == BP_SECTIONCAP_HEAD) {
			/*!< Section Cap | Head */
			ret.append("BP_SECTIONCAP_HEAD");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_HEAD);
			is_done = false;
		} else if((BP_SECTIONCAP_HEAD2 & body_parts) == BP_SECTIONCAP_HEAD2) {
			ret.append("BP_SECTIONCAP_HEAD2");
			/*!< Section Cap | Head 2 */
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_HEAD2);
			is_done = false;
		} else if((BP_SECTIONCAP_LEFTARM & body_parts) == BP_SECTIONCAP_LEFTARM) {
			/*!< Section Cap | Left Arm */
			ret.append("BP_SECTIONCAP_LEFTARM");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_LEFTARM);
			is_done = false;
		} else if((BP_SECTIONCAP_LEFTARM2 & body_parts) == BP_SECTIONCAP_LEFTARM2) {
			/*!< Section Cap | Left Arm 2 */
			ret.append("BP_SECTIONCAP_LEFTARM2");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_LEFTARM2);
			is_done = false;
		} else if((BP_SECTIONCAP_RIGHTARM & body_parts) == BP_SECTIONCAP_RIGHTARM) {
			/*!< Section Cap | Right Arm */
			ret.append("BP_SECTIONCAP_RIGHTARM");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_RIGHTARM);
			is_done = false;
		} else if((BP_SECTIONCAP_RIGHTARM2 & body_parts) == BP_SECTIONCAP_RIGHTARM2) {
			/*!< Section Cap | Right Arm 2 */
			ret.append("BP_SECTIONCAP_RIGHTARM2");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_RIGHTARM2);
			is_done = false;
		} else if((BP_SECTIONCAP_LEFTLEG & body_parts) == BP_SECTIONCAP_LEFTLEG) {
			/*!< Section Cap | Left Leg */ 
			ret.append("BP_SECTIONCAP_LEFTLEG");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_LEFTLEG);
			is_done = false;
		} else if((BP_SECTIONCAP_LEFTLEG2 & body_parts) == BP_SECTIONCAP_LEFTLEG2) {
			/*!< Section Cap | Left Leg 2 */
			ret.append("BP_SECTIONCAP_LEFTLEG2");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_LEFTLEG2);
			is_done = false;
		} else if((BP_SECTIONCAP_LEFTLEG3 & body_parts) == BP_SECTIONCAP_LEFTLEG3) {
			/*!< Section Cap | Left Leg 3 */
			ret.append("BP_SECTIONCAP_LEFTLEG3");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_LEFTLEG3);
			is_done = false;
		} else if((BP_SECTIONCAP_RIGHTLEG & body_parts) == BP_SECTIONCAP_RIGHTLEG) {
			/*!< Section Cap | Right Leg */
			ret.append("BP_SECTIONCAP_RIGHTLEG");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_RIGHTLEG);
			is_done = false;
		} else if((BP_SECTIONCAP_RIGHTLEG2 & body_parts) == BP_SECTIONCAP_RIGHTLEG2) {
			/*!< Section Cap | Right Leg 2 */
			ret.append("BP_SECTIONCAP_RIGHTLEG2");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_RIGHTLEG2);
			is_done = false;
		} else if((BP_SECTIONCAP_RIGHTLEG3 & body_parts) == BP_SECTIONCAP_RIGHTLEG3) {
			/*!< Section Cap | Right Leg 3 */
			ret.append("BP_SECTIONCAP_RIGHTLEG3");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_RIGHTLEG3);
			is_done = false;
		} else if((BP_SECTIONCAP_BRAIN & body_parts) == BP_SECTIONCAP_BRAIN) {
			/*!< Section Cap | Brain */
			ret.append("BP_SECTIONCAP_BRAIN");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_BRAIN);
			is_done = false;
		} else if((BP_TORSOCAP_HEAD & body_parts) == BP_TORSOCAP_HEAD) {
			/*!< Torso Cap | Head */
			ret.append("BP_TORSOCAP_HEAD");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_HEAD);
			is_done = false;
		} else if((BP_TORSOCAP_HEAD2 & body_parts) == BP_TORSOCAP_HEAD2) {
			/*!< Torso Cap | Head 2 */
			ret.append("BP_TORSOCAP_HEAD2");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_HEAD2);
			is_done = false;
		} else if((BP_TORSOCAP_LEFTARM & body_parts) == BP_TORSOCAP_LEFTARM) {
			/*!< Torso Cap | Left Arm */
			ret.append("BP_TORSOCAP_LEFTARM");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_LEFTARM);
			is_done = false;
		} else if((BP_TORSOCAP_LEFTARM2 & body_parts) == BP_TORSOCAP_LEFTARM2) {
			/*!< Torso Cap | Left Arm 2 */
			ret.append("BP_TORSOCAP_LEFTARM2");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_LEFTARM2);
			is_done = false;
		} else if((BP_TORSOCAP_RIGHTARM & body_parts) == BP_TORSOCAP_RIGHTARM) {
			/*!< Torso Cap | Right Arm */
			ret.append("BP_TORSOCAP_RIGHTARM");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_RIGHTARM);
			is_done = false;
		} else if((BP_TORSOCAP_RIGHTARM2 & body_parts) == BP_TORSOCAP_RIGHTARM2) {
			/*!< Torso Cap | Right Arm 2 */
			ret.append("BP_TORSOCAP_RIGHTARM2");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_RIGHTARM2);
			is_done = false;
		} else if((BP_TORSOCAP_LEFTLEG & body_parts) == BP_TORSOCAP_LEFTLEG) {
			/*!< Torso Cap | Left Leg */
			ret.append("BP_TORSOCAP_LEFTLEG");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_LEFTLEG);
			is_done = false;
		} else if((BP_TORSOCAP_LEFTLEG2 & body_parts) == BP_TORSOCAP_LEFTLEG2) {
			/*!< Torso Cap | Left Leg 2 */
			ret.append("BP_TORSOCAP_LEFTLEG2");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_LEFTLEG2);
			is_done = false;
		} else if((BP_TORSOCAP_LEFTLEG3 & body_parts) == BP_TORSOCAP_LEFTLEG3) {
			/*!< Torso Cap | Left Leg 3 */
			ret.append("BP_TORSOCAP_LEFTLEG3");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_LEFTLEG3);
			is_done = false;
		} else if((BP_TORSOCAP_RIGHTLEG & body_parts) == BP_TORSOCAP_RIGHTLEG) {
			/*!< Torso Cap | Right Leg */
			ret.append("BP_TORSOCAP_RIGHTLEG");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_RIGHTLEG);
			is_done = false;
		} else if((BP_TORSOCAP_RIGHTLEG2 & body_parts) == BP_TORSOCAP_RIGHTLEG2) {
			/*!< Torso Cap | Right Leg 2 */
			ret.append("BP_TORSOCAP_RIGHTLEG2");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_RIGHTLEG2);
			is_done = false;
		} else if((BP_TORSOCAP_RIGHTLEG3 & body_parts) == BP_TORSOCAP_RIGHTLEG3) {
			/*!< Torso Cap | Right Leg 3 */
			ret.append("BP_TORSOCAP_RIGHTLEG3");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_RIGHTLEG3);
			is_done = false;
		} else if((BP_TORSOCAP_BRAIN & body_parts) == BP_TORSOCAP_BRAIN) {
			/*!< Torso Cap | Brain */
			ret.append("BP_TORSOCAP_BRAIN");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_BRAIN);
			is_done = false;
		} else if((BP_TORSOSECTION_HEAD & body_parts) == BP_TORSOSECTION_HEAD) {
			/*!< Torso Section | Head */
			ret.append("BP_TORSOSECTION_HEAD");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_HEAD);
			is_done = false;
		} else if((BP_TORSOSECTION_HEAD2 & body_parts) == BP_TORSOSECTION_HEAD2) {
			/*!< Torso Section | Head 2 */
			ret.append("BP_TORSOSECTION_HEAD2");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_HEAD2);
			is_done = false;
		} else if((BP_TORSOSECTION_LEFTARM & body_parts) == BP_TORSOSECTION_LEFTARM) {
			/*!< Torso Section | Left Arm */
			ret.append("BP_TORSOSECTION_LEFTARM");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_LEFTARM);
			is_done = false;
		} else if((BP_TORSOSECTION_LEFTARM2 & body_parts) == BP_TORSOSECTION_LEFTARM2) {
			/*!< Torso Section | Left Arm 2 */
			ret.append("BP_TORSOSECTION_LEFTARM2");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_LEFTARM2);
			is_done = false;
		} else if((BP_TORSOSECTION_RIGHTARM & body_parts) == BP_TORSOSECTION_RIGHTARM) {
			/*!< Torso Section | Right Arm */
			ret.append("BP_TORSOSECTION_RIGHTARM");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_RIGHTARM);
			is_done = false;
		} else if((BP_TORSOSECTION_RIGHTARM2 & body_parts) == BP_TORSOSECTION_RIGHTARM2) {
			/*!< Torso Section | Right Arm 2 */
			ret.append("BP_TORSOSECTION_RIGHTARM2");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_RIGHTARM2);
			is_done = false;
		} else if((BP_TORSOSECTION_LEFTLEG & body_parts) == BP_TORSOSECTION_LEFTLEG) {
			/*!< Torso Section | Left Leg */
			ret.append("BP_TORSOSECTION_LEFTLEG");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_LEFTLEG);
		} else if((BP_TORSOSECTION_LEFTLEG2 & body_parts) == BP_TORSOSECTION_LEFTLEG2) {
			/*!< Torso Section | Left Leg 2 */
			ret.append("BP_TORSOSECTION_LEFTLEG2");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_LEFTLEG2);
			is_done = false;
		} else if((BP_TORSOSECTION_LEFTLEG3 & body_parts) == BP_TORSOSECTION_LEFTLEG3) {
			/*!< Torso Section | Left Leg 3 */
			ret.append("BP_TORSOSECTION_LEFTLEG3");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_LEFTLEG2);
			is_done = false;
		} else if((BP_TORSOSECTION_RIGHTLEG & body_parts) == BP_TORSOSECTION_RIGHTLEG) {
			/*!< Torso Section | Right Leg */
			ret.append("BP_TORSOSECTION_RIGHTLEG");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_RIGHTLEG);
			is_done = false;
		} else if((BP_TORSOSECTION_RIGHTLEG2 & body_parts) == BP_TORSOSECTION_RIGHTLEG2) {
			/*!< Torso Section | Right Leg 2 */
			ret.append("BP_TORSOSECTION_RIGHTLEG2");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_RIGHTLEG2);
			is_done = false;
		} else if((BP_TORSOSECTION_RIGHTLEG3 & body_parts) == BP_TORSOSECTION_RIGHTLEG3) {
			/*!< Torso Section | Right Leg 3 */
			ret.append("BP_TORSOSECTION_RIGHTLEG3");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_RIGHTLEG3);
			is_done = false;
		} else if((BP_TORSOSECTION_BRAIN & body_parts) == BP_TORSOSECTION_BRAIN) {
			/*!< Torso Section | Brain */
			ret.append("BP_TORSOSECTION_BRAIN");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_BRAIN);
			is_done = false;
		} else if((BP_HEAD & body_parts) == BP_HEAD) {
			/*!< Head */
			ret.append("BP_HEAD");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_HEAD);
			is_done = false;
		} else if((BP_HEAD2 & body_parts) == BP_HEAD2) {
			/*!< Head 2 */
			ret.append("BP_HEAD2");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_HEAD2);
			is_done = false;
		} else if((BP_LEFTARM & body_parts) == BP_LEFTARM) {
			/*!< Left Arm */
			ret.append("BP_LEFTARM");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_LEFTARM);
			is_done = false;
		} else if((BP_LEFTARM2 & body_parts) == BP_LEFTARM2) {
			/*!< Left Arm 2 */
			ret.append("BP_LEFTARM2");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_LEFTARM2);
			is_done = false;
		} else if((BP_RIGHTARM & body_parts) == BP_RIGHTARM) {
			/*!< Right Arm */
			ret.append("BP_RIGHTARM");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_RIGHTARM);
			is_done = false;
		} else if((BP_RIGHTARM2 & body_parts) == BP_RIGHTARM2) {
			/*!< Right Arm 2 */ 
			ret.append("BP_RIGHTARM2");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_RIGHTARM2);
			is_done = false;
		} else if((BP_LEFTLEG & body_parts) == BP_LEFTLEG) {
			/*!< Left Leg */
			ret.append("BP_LEFTLEG");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_LEFTLEG);
			is_done = false;
		} else if((BP_LEFTLEG2 & body_parts) == BP_LEFTLEG2) {
			/*!< Left Leg 2 */
			ret.append("BP_LEFTLEG2");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_LEFTLEG2);
			is_done = false;
		} else if((BP_LEFTLEG3 & body_parts) == BP_LEFTLEG3) {
			/*!< Left Leg 3 */
			ret.append("BP_LEFTLEG3");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_LEFTLEG3);
			is_done = false;
		} else if((BP_RIGHTLEG & body_parts) == BP_RIGHTLEG) {
			/*!< Right Leg */
			ret.append("BP_RIGHTLEG");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_RIGHTLEG);
			is_done = false;
		} else if((BP_RIGHTLEG2 & body_parts) == BP_RIGHTLEG2) {
			/*!< Right Leg 2 */
			ret.append("BP_RIGHTLEG2");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_RIGHTLEG2);
			is_done = false;
		} else if((BP_RIGHTLEG3 & body_parts) == BP_RIGHTLEG3) {
			/*!< Right Leg 3 */
			ret.append("BP_RIGHTLEG3");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_RIGHTLEG3);
			is_done = false;
		} else if((BP_BRAIN & body_parts) == BP_BRAIN) {
			/*!< Brain */
			ret.append("BP_BRAIN");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~BP_BRAIN);
			is_done = false;
		} else if((32 & body_parts) == 32) {
			ret.append("UNKNOWN_IMPORTANT_FLAG_32");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~32);
			is_done = false;
		}
		else if ((SBP_30_HEAD & body_parts) == SBP_30_HEAD) {
			ret.append("SBP_30_HEAD");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~SBP_30_HEAD);
			is_done = false;
		}
		else if ((SBP_31_HAIR & body_parts) == SBP_31_HAIR) {
			ret.append("SBP_31_HAIR");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~SBP_31_HAIR);
			is_done = false;
		}
		else if ((SBP_32_BODY & body_parts) == SBP_32_BODY) {
			ret.append("SBP_32_BODY");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~SBP_32_BODY);
			is_done = false;
		}
		else if ((SBP_33_HANDS & body_parts) == SBP_33_HANDS) {
			ret.append("SBP_33_HANDS");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~SBP_33_HANDS);
			is_done = false;
		}
		else if ((SBP_34_FOREARMS & body_parts) == SBP_34_FOREARMS) {
			ret.append("SBP_34_FOREARMS");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~SBP_34_FOREARMS);
			is_done = false;
		}
		else if ((SBP_35_AMULET & body_parts) == SBP_35_AMULET) {
			ret.append("SBP_35_AMULET");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~SBP_35_AMULET);
			is_done = false;
		}
		else if ((SBP_36_RING & body_parts) == SBP_36_RING) {
			ret.append("SBP_36_RING");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~SBP_36_RING);
			is_done = false;
		}
		else if ((SBP_37_FEET & body_parts) == SBP_37_FEET) {
			ret.append("SBP_37_FEET");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~SBP_37_FEET);
			is_done = false;
		}
		else if ((SBP_38_CALVES & body_parts) == SBP_38_CALVES) {
			ret.append("SBP_38_CALVES");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~SBP_38_CALVES);
			is_done = false;
		}
		else if ((SBP_39_SHIELD & body_parts) == SBP_39_SHIELD) {
			ret.append("SBP_39_SHIELD");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~SBP_39_SHIELD);
			is_done = false;
		}
		else if ((SBP_40_TAIL & body_parts) == SBP_40_TAIL) {
			ret.append("SBP_40_TAIL");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~SBP_40_TAIL);
			is_done = false;
		}
		else if ((SBP_41_LONGHAIR & body_parts) == SBP_41_LONGHAIR) {
			ret.append("SBP_41_LONGHAIR");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~SBP_41_LONGHAIR);
			is_done = false;
		}
		else if ((SBP_42_CIRCLET & body_parts) == SBP_42_CIRCLET) {
			ret.append("SBP_42_CIRCLET");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~SBP_42_CIRCLET);
			is_done = false;
		}
		else if ((SBP_43_EARS & body_parts) == SBP_43_EARS) {
			ret.append("SBP_43_EARS");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~SBP_43_EARS);
			is_done = false;
		}
		else if ((SBP_44_DRAGON_BLOODHEAD_OR_MOD_MOUTH & body_parts) == SBP_44_DRAGON_BLOODHEAD_OR_MOD_MOUTH) {
			ret.append("SBP_44_DRAGON_BLOODHEAD_OR_MOD_MOUTH");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~SBP_44_DRAGON_BLOODHEAD_OR_MOD_MOUTH);
			is_done = false;
		}
		else if ((SBP_45_DRAGON_BLOODWINGL_OR_MOD_NECK & body_parts) == SBP_45_DRAGON_BLOODWINGL_OR_MOD_NECK) {
			ret.append("SBP_45_DRAGON_BLOODWINGL_OR_MOD_NECK");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~SBP_45_DRAGON_BLOODWINGL_OR_MOD_NECK);
			is_done = false;
		}
		else if ((SBP_46_DRAGON_BLOODWINGR_OR_MOD_CHEST_PRIMARY & body_parts) == SBP_46_DRAGON_BLOODWINGR_OR_MOD_CHEST_PRIMARY) {
			ret.append("SBP_46_DRAGON_BLOODWINGR_OR_MOD_CHEST_PRIMARY");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~SBP_46_DRAGON_BLOODWINGR_OR_MOD_CHEST_PRIMARY);
			is_done = false;
		}
		else if ((SBP_47_DRAGON_BLOODTAIL_OR_MOD_BACK & body_parts) == SBP_47_DRAGON_BLOODTAIL_OR_MOD_BACK) {
			ret.append("SBP_47_DRAGON_BLOODTAIL_OR_MOD_BACK");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~SBP_47_DRAGON_BLOODTAIL_OR_MOD_BACK);
			is_done = false;
		}
		else if ((SBP_48_MOD_MISC1 & body_parts) == SBP_48_MOD_MISC1) {
			ret.append("SBP_48_MOD_MISC1");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~SBP_48_MOD_MISC1);
			is_done = false;
		}
		else if ((SBP_49_MOD_PELVIS_PRIMARY & body_parts) == SBP_49_MOD_PELVIS_PRIMARY) {
			ret.append("SBP_49_MOD_PELVIS_PRIMARY");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~SBP_49_MOD_PELVIS_PRIMARY);
			is_done = false;
		}
		else if ((SBP_50_DECAPITATEDHEAD & body_parts) == SBP_50_DECAPITATEDHEAD) {
			ret.append("SBP_50_DECAPITATEDHEAD");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~SBP_50_DECAPITATEDHEAD);
			is_done = false;
		}
		else if ((SBP_51_DECAPITATE & body_parts) == SBP_51_DECAPITATE) {
			ret.append("SBP_51_DECAPITATE");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~SBP_51_DECAPITATE);
			is_done = false;
		}
		else if ((SBP_52_MOD_PELVIS_SECONDARY & body_parts) == SBP_52_MOD_PELVIS_SECONDARY) {
			ret.append("SBP_52_MOD_PELVIS_SECONDARY");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~SBP_52_MOD_PELVIS_SECONDARY);
			is_done = false;
		}
		else if ((SBP_53_MOD_LEG_RIGHT & body_parts) == SBP_53_MOD_LEG_RIGHT) {
			ret.append("SBP_53_MOD_LEG_RIGHT");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~SBP_53_MOD_LEG_RIGHT);
			is_done = false;
		}
		else if ((32 & body_parts) == SBP_54_MOD_LEG_LEFT) {
			ret.append("SBP_54_MOD_LEG_LEFT");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~SBP_54_MOD_LEG_LEFT);
			is_done = false;
		}
		else if ((SBP_55_MOD_FACE_JEWELRY & body_parts) == SBP_55_MOD_FACE_JEWELRY) {
			ret.append("SBP_55_MOD_FACE_JEWELRY");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~SBP_55_MOD_FACE_JEWELRY);
			is_done = false;
		}
		else if ((SBP_57_MOD_SHOULDER & body_parts) == SBP_57_MOD_SHOULDER) {
			ret.append("SBP_57_MOD_SHOULDER");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~SBP_57_MOD_SHOULDER);
			is_done = false;
		}
		else if ((SBP_58_MOD_ARM_LEFT & body_parts) == SBP_58_MOD_ARM_LEFT) {
			ret.append("SBP_58_MOD_ARM_LEFT");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~SBP_58_MOD_ARM_LEFT);
			is_done = false;
		}
		else if ((SBP_59_MOD_ARM_RIGHT & body_parts) == SBP_59_MOD_ARM_RIGHT) {
			ret.append("SBP_59_MOD_ARM_RIGHT");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~SBP_59_MOD_ARM_RIGHT);
			is_done = false;
		}
		else if ((SBP_60_MOD_MISC2 & body_parts) == SBP_60_MOD_MISC2) {
			ret.append("SBP_60_MOD_MISC2");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~SBP_60_MOD_MISC2);
			is_done = false;
		}
		else if ((SBP_61_FX01 & body_parts) == SBP_61_FX01) {
			ret.append("SBP_61_FX01");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~SBP_61_FX01);
			is_done = false;
		}
		else if ((SBP_130_HEAD & body_parts) == SBP_130_HEAD) {
			ret.append("SBP_130_HEAD");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~SBP_130_HEAD);
			is_done = false;
		}
		else if ((SBP_131_HAIR & body_parts) == SBP_131_HAIR) {
			ret.append("SBP_131_HAIR");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~SBP_131_HAIR);
			is_done = false;
		}
		else if ((SBP_141_LONGHAIR & body_parts) == SBP_141_LONGHAIR) {
			ret.append("SBP_141_LONGHAIR");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~SBP_141_LONGHAIR);
			is_done = false;
		}
		else if ((SBP_142_CIRCLET & body_parts) == SBP_142_CIRCLET) {
			ret.append("SBP_142_CIRCLET");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~SBP_142_CIRCLET);
			is_done = false;
		}
		else if ((SBP_143_EARS & body_parts) == SBP_143_EARS) {
			ret.append("SBP_143_EARS");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~SBP_143_EARS);
			is_done = false;
		}
		else if ((SBP_150_DECAPITATEDHEAD & body_parts) == SBP_150_DECAPITATEDHEAD) {
			ret.append("SBP_150_DECAPITATEDHEAD");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~SBP_150_DECAPITATEDHEAD);
			is_done = false;
		}
		else if ((SBP_230_HEAD & body_parts) == SBP_230_HEAD) {
			ret.append("SBP_230_HEAD");
			body_parts = (BSDismemberBodyPartType)(body_parts & ~SBP_230_HEAD);
			is_done = false;
		}

		
	}

	return ret;
}

BSDismemberBodyPartType NifDismemberPartition::stringArrayToBodyPartType( const MStringArray& string_array ) {
	BSDismemberBodyPartType ret = (BSDismemberBodyPartType)0;
	MString current_flag;

	for(int i = 0; i < string_array.length(); i++) {
		current_flag = string_array[i];

		if (current_flag == "BP_TORSO") {
			/*!< Torso */
			ret = (BSDismemberBodyPartType)(ret | BP_TORSO);
		} else if(current_flag == "BP_HEAD") {
			/*!< Head */
			ret = (BSDismemberBodyPartType)(ret | BP_HEAD); 
		} else if(current_flag == "BP_HEAD2"){
			/*!< Head 2 */
			ret = (BSDismemberBodyPartType)(ret | BP_HEAD2);
		} else if(current_flag == "BP_LEFTARM") {
			/*!< Left Arm */
			ret = (BSDismemberBodyPartType)(ret | BP_LEFTARM);
		} else if(current_flag == "BP_LEFTARM2") {
			/*!< Left Arm 2 */
			ret = (BSDismemberBodyPartType)(ret | BP_LEFTARM2);
		} else if(current_flag == "BP_RIGHTARM") { 
			/*!< Right Arm */
			ret = (BSDismemberBodyPartType)(ret | BP_RIGHTARM);
		} else if(current_flag == "BP_RIGHTARM2") {
			/*!< Right Arm 2 */
			ret = (BSDismemberBodyPartType)(ret | BP_RIGHTARM2);
		} else if(current_flag == "BP_LEFTLEG") {
			/*!< Left Leg */
			ret = (BSDismemberBodyPartType)(ret | BP_LEFTLEG); 
		} else if(current_flag == "BP_LEFTLEG2") {
			/*!< Left Leg 2 */
			ret = (BSDismemberBodyPartType)(ret | BP_LEFTLEG2);
		} else if(current_flag == "BP_LEFTLEG3") {
			/*!< Left Leg 3 */
			ret = (BSDismemberBodyPartType)(ret | BP_LEFTLEG3);
		} else if(current_flag == "BP_RIGHTLEG") {
			/*!< Right Leg */
			ret = (BSDismemberBodyPartType)(ret | BP_RIGHTLEG);
		} else if(current_flag == "BP_RIGHTLEG2") {
			/*!< Right Leg 2 */
			ret = (BSDismemberBodyPartType)(ret | BP_RIGHTLEG2);
		} else if(current_flag == "BP_RIGHTLEG3") {
			/*!< Right Leg 3 */
			ret = (BSDismemberBodyPartType)(ret | BP_RIGHTLEG3);
		} else if(current_flag == "BP_BRAIN") {
			/*!< Brain */
			ret = (BSDismemberBodyPartType)(ret | BP_BRAIN);
		} else if(current_flag == "BP_SECTIONCAP_HEAD") {
			/*!< Section Cap | Head */
			ret = (BSDismemberBodyPartType)(ret | BP_SECTIONCAP_HEAD);
		} else if(current_flag == "BP_SECTIONCAP_HEAD2") {
			/*!< Section Cap | Head 2 */
			ret = (BSDismemberBodyPartType)(ret | BP_SECTIONCAP_HEAD2);
		} else if(current_flag == "BP_SECTIONCAP_LEFTARM") {
			/*!< Section Cap | Left Arm */
			ret = (BSDismemberBodyPartType)(ret | BP_SECTIONCAP_LEFTARM);
		} else if(current_flag == "BP_SECTIONCAP_LEFTARM2") {
			/*!< Section Cap | Left Arm 2 */
			ret = (BSDismemberBodyPartType)(ret | BP_SECTIONCAP_LEFTARM2);
		} else if(current_flag == "BP_SECTIONCAP_RIGHTARM") {
			/*!< Section Cap | Right Arm */
			ret = (BSDismemberBodyPartType)(ret | BP_SECTIONCAP_RIGHTARM);
		} else if(current_flag == "BP_SECTIONCAP_RIGHTARM2") {
			/*!< Section Cap | Right Arm 2 */
			ret = (BSDismemberBodyPartType)(ret | BP_SECTIONCAP_RIGHTARM2);
		} else if(current_flag == "BP_SECTIONCAP_LEFTLEG") {
			/*!< Section Cap | Left Leg */
			ret = (BSDismemberBodyPartType)(ret | BP_SECTIONCAP_LEFTLEG);
		} else if(current_flag == "BP_SECTIONCAP_LEFTLEG2") {
			/*!< Section Cap | Left Leg 2 */
			ret = (BSDismemberBodyPartType)(ret | BP_SECTIONCAP_LEFTLEG2);
		} else if(current_flag == "BP_SECTIONCAP_LEFTLEG3") {
			/*!< Section Cap | Left Leg 3 */
			ret = (BSDismemberBodyPartType)(ret | BP_SECTIONCAP_LEFTLEG3);
		} else if(current_flag == "BP_SECTIONCAP_RIGHTLEG") {
			/*!< Section Cap | Right Leg */
			ret = (BSDismemberBodyPartType)(ret | BP_SECTIONCAP_RIGHTLEG);
		} else if(current_flag == "BP_SECTIONCAP_RIGHTLEG2") {
			/*!< Section Cap | Right Leg 2 */
			ret = (BSDismemberBodyPartType)(ret | BP_SECTIONCAP_RIGHTLEG2);
		} else if(current_flag == "BP_SECTIONCAP_RIGHTLEG3") {
			/*!< Section Cap | Right Leg 3 */
			ret = (BSDismemberBodyPartType)(ret | BP_SECTIONCAP_RIGHTLEG3);
		} else if(current_flag == "BP_SECTIONCAP_BRAIN") {
			/*!< Section Cap | Brain */
			ret = (BSDismemberBodyPartType)(ret | BP_SECTIONCAP_BRAIN);
		} else if(current_flag == "BP_TORSOCAP_HEAD") {
			/*!< Torso Cap | Head */
			ret = (BSDismemberBodyPartType)(ret | BP_TORSOCAP_HEAD);
		} else if(current_flag == "BP_TORSOCAP_HEAD2") {
			/*!< Torso Cap | Head 2 */
			ret = (BSDismemberBodyPartType)(ret | BP_TORSOCAP_HEAD2);
		} else if(current_flag == "BP_TORSOCAP_LEFTARM") {
			/*!< Torso Cap | Left Arm */
			ret = (BSDismemberBodyPartType)(ret | BP_TORSOCAP_LEFTARM);
		} else if(current_flag == "BP_TORSOCAP_LEFTARM2") {
			/*!< Torso Cap | Left Arm 2 */
			ret = (BSDismemberBodyPartType)(ret | BP_TORSOCAP_LEFTARM2);
		} else if(current_flag == "BP_TORSOCAP_RIGHTARM") {
			/*!< Torso Cap | Right Arm */
			ret = (BSDismemberBodyPartType)(ret | BP_TORSOCAP_RIGHTARM);
		} else if(current_flag == "BP_TORSOCAP_RIGHTARM2") {
			/*!< Torso Cap | Right Arm 2 */
			ret = (BSDismemberBodyPartType)(ret | BP_TORSOCAP_RIGHTARM2);
		} else if(current_flag == "BP_TORSOCAP_LEFTLEG") {
			/*!< Torso Cap | Left Leg */
			ret = (BSDismemberBodyPartType)(ret | BP_TORSOCAP_LEFTLEG);
		} else if(current_flag == "BP_TORSOCAP_LEFTLEG2") {
			/*!< Torso Cap | Left Leg 2 */
			ret = (BSDismemberBodyPartType)(ret | BP_TORSOCAP_LEFTLEG2);
		} else if(current_flag == "BP_TORSOCAP_LEFTLEG3") {
			/*!< Torso Cap | Left Leg 3 */
			ret = (BSDismemberBodyPartType)(ret | BP_TORSOCAP_LEFTLEG3);
		} else if(current_flag == "BP_TORSOCAP_RIGHTLEG") {
			/*!< Torso Cap | Right Leg */
			ret = (BSDismemberBodyPartType)(ret | BP_TORSOCAP_RIGHTLEG);
		} else if(current_flag == "BP_TORSOCAP_RIGHTLEG2") {
			/*!< Torso Cap | Right Leg 2 */
			ret = (BSDismemberBodyPartType)(ret | BP_TORSOCAP_RIGHTLEG2);
		} else if(current_flag == "BP_TORSOCAP_RIGHTLEG3") {
			/*!< Torso Cap | Right Leg 3 */
			ret = (BSDismemberBodyPartType)(ret | BP_TORSOCAP_RIGHTLEG3);
		} else if(current_flag == "BP_TORSOCAP_BRAIN") {
			/*!< Torso Cap | Brain */
			ret = (BSDismemberBodyPartType)(ret | BP_TORSOCAP_BRAIN);
		} else if(current_flag == "BP_TORSOSECTION_HEAD") {
			/*!< Torso Section | Head */
			ret = (BSDismemberBodyPartType)(ret | BP_TORSOSECTION_HEAD);
		} else if(current_flag == "BP_TORSOSECTION_HEAD2") {
			/*!< Torso Section | Head 2 */
			ret = (BSDismemberBodyPartType)(ret | BP_TORSOSECTION_HEAD2);
		} else if(current_flag == "BP_TORSOSECTION_LEFTARM") {
			/*!< Torso Section | Left Arm */
			ret = (BSDismemberBodyPartType)(ret | BP_TORSOSECTION_LEFTARM);
		} else if(current_flag == "BP_TORSOSECTION_LEFTARM2") {
			/*!< Torso Section | Left Arm 2 */
			ret = (BSDismemberBodyPartType)(ret | BP_TORSOSECTION_LEFTARM2);
		} else if(current_flag == "BP_TORSOSECTION_RIGHTARM") {
			/*!< Torso Section | Right Arm */
			ret = (BSDismemberBodyPartType)(ret | BP_TORSOSECTION_RIGHTARM);
		} else if(current_flag == "BP_TORSOSECTION_RIGHTARM2") {
			/*!< Torso Section | Right Arm 2 */
			ret = (BSDismemberBodyPartType)(ret | BP_TORSOSECTION_RIGHTARM2);
		} else if(current_flag == "BP_TORSOSECTION_LEFTLEG") {
			/*!< Torso Section | Left Leg */
			ret = (BSDismemberBodyPartType)(ret | BP_TORSOSECTION_LEFTLEG);
		} else if(current_flag == "BP_TORSOSECTION_LEFTLEG2") {
			/*!< Torso Section | Left Leg 2 */
			ret = (BSDismemberBodyPartType)(ret | BP_TORSOSECTION_LEFTLEG2);
		} else if(current_flag == "BP_TORSOSECTION_LEFTLEG3") {
			/*!< Torso Section | Left Leg 3 */
			ret = (BSDismemberBodyPartType)(ret | BP_TORSOSECTION_LEFTLEG3);
		} else if(current_flag == "BP_TORSOSECTION_RIGHTLEG") {
			/*!< Torso Section | Right Leg */
			ret = (BSDismemberBodyPartType)(ret | BP_TORSOSECTION_RIGHTLEG);
		} else if(current_flag == "BP_TORSOSECTION_RIGHTLEG2") {
			/*!< Torso Section | Right Leg 2 */
			ret = (BSDismemberBodyPartType)(ret | BP_TORSOSECTION_RIGHTLEG2);
		} else if(current_flag == "BP_TORSOSECTION_RIGHTLEG3") {
			/*!< Torso Section | Right Leg 3 */
			ret = (BSDismemberBodyPartType)(ret | BP_TORSOSECTION_RIGHTLEG3);
		} else if(current_flag == "BP_TORSOSECTION_BRAIN") {
			/*!< Torso Section | Brain */
			ret = (BSDismemberBodyPartType)(ret | BP_TORSOSECTION_BRAIN);
		} else if(current_flag == "UNKNOWN_IMPORTANT_FLAG_32") {
			ret = (BSDismemberBodyPartType)(ret | 32);
		}
		else if (current_flag == "SBP_30_HEAD") {
			/*!< Torso Section | Brain */
			ret = (BSDismemberBodyPartType)(ret | SBP_30_HEAD);
		}
		else if (current_flag == "SBP_31_HAIR") {
			/*!< Torso Section | Brain */
			ret = (BSDismemberBodyPartType)(ret | SBP_31_HAIR);
		}
		else if (current_flag == "SBP_32_BODY") {
			/*!< Torso Section | Brain */
			ret = (BSDismemberBodyPartType)(ret | SBP_32_BODY);
		}
		else if (current_flag == "SBP_33_HANDS") {
			/*!< Torso Section | Brain */
			ret = (BSDismemberBodyPartType)(ret | SBP_33_HANDS);
		}
		else if (current_flag == "SBP_34_FOREARMS") {
			/*!< Torso Section | Brain */
			ret = (BSDismemberBodyPartType)(ret | SBP_34_FOREARMS);
		}
		else if (current_flag == "SBP_35_AMULET") {
			/*!< Torso Section | Brain */
			ret = (BSDismemberBodyPartType)(ret | SBP_35_AMULET);
		}
		else if (current_flag == "SBP_36_RING") {
			/*!< Torso Section | Brain */
			ret = (BSDismemberBodyPartType)(ret | SBP_36_RING);
		}
		else if (current_flag == "SBP_37_FEET") {
			/*!< Torso Section | Brain */
			ret = (BSDismemberBodyPartType)(ret | SBP_37_FEET);
		}
		else if (current_flag == "SBP_38_CALVES") {
			/*!< Torso Section | Brain */
			ret = (BSDismemberBodyPartType)(ret | SBP_38_CALVES);
		}
		else if (current_flag == "SBP_39_SHIELD") {
			/*!< Torso Section | Brain */
			ret = (BSDismemberBodyPartType)(ret | SBP_39_SHIELD);
		}
		else if (current_flag == "SBP_40_TAIL") {
			/*!< Torso Section | Brain */
			ret = (BSDismemberBodyPartType)(ret | SBP_40_TAIL);
		}
		else if (current_flag == "SBP_41_LONGHAIR") {
			/*!< Torso Section | Brain */
			ret = (BSDismemberBodyPartType)(ret | SBP_41_LONGHAIR);
		}
		else if (current_flag == "SBP_42_CIRCLET") {
			/*!< Torso Section | Brain */
			ret = (BSDismemberBodyPartType)(ret | SBP_42_CIRCLET);
		}
		else if (current_flag == "SBP_43_EARS") {
			/*!< Torso Section | Brain */
			ret = (BSDismemberBodyPartType)(ret | SBP_43_EARS);
		}
		else if (current_flag == "SBP_44_DRAGON_BLOODHEAD_OR_MOD_MOUTH") {
			/*!< Torso Section | Brain */
			ret = (BSDismemberBodyPartType)(ret | SBP_44_DRAGON_BLOODHEAD_OR_MOD_MOUTH);
		}
		else if (current_flag == "SBP_45_DRAGON_BLOODWINGL_OR_MOD_NECK") {
			/*!< Torso Section | Brain */
			ret = (BSDismemberBodyPartType)(ret | SBP_45_DRAGON_BLOODWINGL_OR_MOD_NECK);
		}
		else if (current_flag == "SBP_46_DRAGON_BLOODWINGR_OR_MOD_CHEST_PRIMARY") {
			/*!< Torso Section | Brain */
			ret = (BSDismemberBodyPartType)(ret | SBP_46_DRAGON_BLOODWINGR_OR_MOD_CHEST_PRIMARY);
		}
		else if (current_flag == "SBP_47_DRAGON_BLOODTAIL_OR_MOD_BACK") {
			/*!< Torso Section | Brain */
			ret = (BSDismemberBodyPartType)(ret | SBP_47_DRAGON_BLOODTAIL_OR_MOD_BACK);
		}
		else if (current_flag == "SBP_48_MOD_MISC1") {
			/*!< Torso Section | Brain */
			ret = (BSDismemberBodyPartType)(ret | SBP_48_MOD_MISC1);
		}
		else if (current_flag == "SBP_49_MOD_PELVIS_PRIMARY") {
			/*!< Torso Section | Brain */
			ret = (BSDismemberBodyPartType)(ret | SBP_49_MOD_PELVIS_PRIMARY);
		}
		else if (current_flag == "SBP_50_DECAPITATEDHEAD") {
			/*!< Torso Section | Brain */
			ret = (BSDismemberBodyPartType)(ret | SBP_50_DECAPITATEDHEAD);
		}
		else if (current_flag == "SBP_51_DECAPITATE") {
			/*!< Torso Section | Brain */
			ret = (BSDismemberBodyPartType)(ret | SBP_51_DECAPITATE);
		}
		else if (current_flag == "SBP_52_MOD_PELVIS_SECONDARY") {
			/*!< Torso Section | Brain */
			ret = (BSDismemberBodyPartType)(ret | SBP_52_MOD_PELVIS_SECONDARY);
		}
		else if (current_flag == "SBP_53_MOD_LEG_RIGHT") {
			/*!< Torso Section | Brain */
			ret = (BSDismemberBodyPartType)(ret | SBP_53_MOD_LEG_RIGHT);
		}
		else if (current_flag == "SBP_54_MOD_LEG_LEFT") {
			/*!< Torso Section | Brain */
			ret = (BSDismemberBodyPartType)(ret | SBP_54_MOD_LEG_LEFT);
		}
		else if (current_flag == "SBP_55_MOD_FACE_JEWELRY") {
			/*!< Torso Section | Brain */
			ret = (BSDismemberBodyPartType)(ret | SBP_55_MOD_FACE_JEWELRY);
		}
		else if (current_flag == "SBP_56_MOD_CHEST_SECONDARY") {
			/*!< Torso Section | Brain */
			ret = (BSDismemberBodyPartType)(ret | SBP_56_MOD_CHEST_SECONDARY);
		}
		else if (current_flag == "SBP_57_MOD_SHOULDER") {
			/*!< Torso Section | Brain */
			ret = (BSDismemberBodyPartType)(ret | SBP_57_MOD_SHOULDER);
		}
		else if (current_flag == "SBP_58_MOD_ARM_LEFT") {
			/*!< Torso Section | Brain */
			ret = (BSDismemberBodyPartType)(ret | SBP_58_MOD_ARM_LEFT);
		}
		else if (current_flag == "SBP_59_MOD_ARM_RIGHT") {
			/*!< Torso Section | Brain */
			ret = (BSDismemberBodyPartType)(ret | SBP_59_MOD_ARM_RIGHT);
		}
		else if (current_flag == "SBP_60_MOD_MISC2") {
			/*!< Torso Section | Brain */
			ret = (BSDismemberBodyPartType)(ret | SBP_60_MOD_MISC2);
		}
		else if (current_flag == "SBP_61_FX01") {
			/*!< Torso Section | Brain */
			ret = (BSDismemberBodyPartType)(ret | SBP_61_FX01);
		}
		else if (current_flag == "SBP_130_HEAD") {
			/*!< Torso Section | Brain */
			ret = (BSDismemberBodyPartType)(ret | SBP_130_HEAD);
		}
		else if (current_flag == "SBP_131_HAIR") {
			/*!< Torso Section | Brain */
			ret = (BSDismemberBodyPartType)(ret | SBP_131_HAIR);
		}
		else if (current_flag == "SBP_141_LONGHAIR") {
			/*!< Torso Section | Brain */
			ret = (BSDismemberBodyPartType)(ret | SBP_141_LONGHAIR);
		}
		else if (current_flag == "SBP_142_CIRCLET") {
			/*!< Torso Section | Brain */
			ret = (BSDismemberBodyPartType)(ret | SBP_142_CIRCLET);
		}
		else if (current_flag == "SBP_143_EARS") {
			/*!< Torso Section | Brain */
			ret = (BSDismemberBodyPartType)(ret | SBP_143_EARS);
		}
		else if (current_flag == "SBP_150_DECAPITATEDHEAD") {
			/*!< Torso Section | Brain */
			ret = (BSDismemberBodyPartType)(ret | SBP_150_DECAPITATEDHEAD);
		}
		else if (current_flag == "SBP_230_HEAD") {
			/*!< Torso Section | Brain */
			ret = (BSDismemberBodyPartType)(ret | SBP_230_HEAD);
		}
	}

	return ret;
}

MStringArray NifDismemberPartition::partToStringArray( BSPartFlag parts ) {
	MStringArray ret;

	if((PF_EDITOR_VISIBLE & parts) == PF_EDITOR_VISIBLE) {
		/*!< Visible in Editor */
		ret.append("PF_EDITOR_VISIBLE");
	}
	if((PF_START_NET_BONESET & parts) == PF_START_NET_BONESET) {
		/*!< Start a new shared boneset.  It is expected this BoneSet and the following sets in the Skin Partition will have the same bones. */
		ret.append("PF_START_NET_BONESET");
	}

	return ret;
}

BSPartFlag NifDismemberPartition::stringArrayToPart( const MStringArray& string_array ) {
	BSPartFlag ret = (BSPartFlag)0;
	MString current_flag;

	for(int i = 0; i < string_array.length(); i++) {
		current_flag = string_array[i];

		if(current_flag == "PF_EDITOR_VISIBLE") {
			/*!< Visible in Editor */
			ret = (BSPartFlag)(ret | PF_EDITOR_VISIBLE);
		} else if(current_flag == "PF_START_NET_BONESET") {
			/*!< Start a new shared boneset.  It is expected this BoneSet and the following sets in the Skin Partition will have the same bones. */
			ret = (BSPartFlag)(ret | PF_START_NET_BONESET);
		}
	}

	return ret;
}

MObject NifDismemberPartition::targetFaces;

MObject NifDismemberPartition::targetShape;

MObject NifDismemberPartition::partsFlags;

MObject NifDismemberPartition::bodyPartsFlags;

MTypeId NifDismemberPartition::id(0x7ff11);
