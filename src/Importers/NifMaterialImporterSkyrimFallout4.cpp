#include "include/Importers/NifMaterialImporterSkyrimFallout4.h"
#include <obj/BSShape.h>
#include <obj/BSTriShape.h>
#include <obj/BSSubIndexTriShape.h>
#include <obj/BSMeshLODTriShape.h>


NifMaterialImporterSkyrimFallout4::NifMaterialImporterSkyrimFallout4() {

}

NifMaterialImporterSkyrimFallout4::NifMaterialImporterSkyrimFallout4( NifTranslatorOptionsRef translatorOptions, NifTranslatorDataRef translatorData, NifTranslatorUtilsRef translatorUtils ) {
	this->translatorOptions = translatorOptions;
	this->translatorData = translatorData;
	this->translatorUtils = translatorUtils;
}

void NifMaterialImporterSkyrimFallout4::ImportMaterialsAndTextures( NiAVObjectRef & root ) {
	this->GatherMaterialsAndTextures(root);
}

void NifMaterialImporterSkyrimFallout4::GatherMaterialsAndTextures( NiAVObjectRef & root ) {
	if(root->GetType().IsDerivedType(NiGeometry::TYPE) || root->GetType().IsDerivedType(BSShape::TYPE)) {

		NiAlphaPropertyRef alpha_property = NULL;
		BSLightingShaderPropertyRef shader_property = NULL;

		NiGeometryRef ni_geometry = DynamicCast<NiGeometry>(root);

		if (ni_geometry != NULL)
		{
			Niflib::array<2, Ref<NiProperty>> properties = ni_geometry->GetBSProperties();

			for (int i = 0; i < 2; i++) {
				NiPropertyRef current_property = properties[i];

				if (current_property != NULL && current_property->GetType().IsSameType(BSLightingShaderProperty::TYPE)) {
					shader_property = DynamicCast<BSLightingShaderProperty>(current_property);
				}
				if (current_property != NULL && current_property->GetType().IsSameType(NiAlphaProperty::TYPE)) {
					alpha_property = DynamicCast<NiAlphaProperty>(current_property);
				}
			}
		}

		BSTriShapeRef bs_tri_shape = DynamicCast<BSTriShape>(root);

		if (bs_tri_shape != NULL)
		{
			for (int i = 0; i < 2; i++) {
				NiPropertyRef current_property = bs_tri_shape->GetBSProperty(i);

				if (current_property != NULL && current_property->GetType().IsSameType(BSLightingShaderProperty::TYPE)) {
					shader_property = DynamicCast<BSLightingShaderProperty>(current_property);
				}
				if (current_property != NULL && current_property->GetType().IsSameType(NiAlphaProperty::TYPE)) {
					alpha_property = DynamicCast<NiAlphaProperty>(current_property);
				}
			}
		}

		BSSubIndexTriShapeRef bs_sub_index_tri_shape = DynamicCast<BSSubIndexTriShape>(root);

		if (bs_sub_index_tri_shape != NULL)
		{
			for (int i = 0; i < 2; i++) {
				NiPropertyRef current_property = bs_sub_index_tri_shape->GetBSProperty(i);

				if (current_property != NULL && current_property->GetType().IsSameType(BSLightingShaderProperty::TYPE)) {
					shader_property = DynamicCast<BSLightingShaderProperty>(current_property);
				}
				if (current_property != NULL && current_property->GetType().IsSameType(NiAlphaProperty::TYPE)) {
					alpha_property = DynamicCast<NiAlphaProperty>(current_property);
				}
			}
		}

		BSMeshLODTriShapeRef bs_mesh_lod_tri_shape = DynamicCast<BSMeshLODTriShape>(root);

		if (bs_mesh_lod_tri_shape != NULL)
		{
			for (int i = 0; i < 2; i++) {
				NiPropertyRef current_property = bs_mesh_lod_tri_shape->GetBSProperty(i);

				if (current_property != NULL && current_property->GetType().IsSameType(BSLightingShaderProperty::TYPE)) {
					shader_property = DynamicCast<BSLightingShaderProperty>(current_property);
				}
				if (current_property != NULL && current_property->GetType().IsSameType(NiAlphaProperty::TYPE)) {
					alpha_property = DynamicCast<NiAlphaProperty>(current_property);
				}
			}
		}

		int valid_properties = 0;

		if(alpha_property != NULL) {
			valid_properties++;
		}
		if(shader_property != NULL) {
			valid_properties++;
		}

		if(valid_properties == 0) {
			return;
		}

		MObject found_material;

		for(int i = 0; i < this->property_groups.size(); i++) {
			vector<NiPropertyRef> property_group = this->property_groups[i];
			int similarities = 0;

			for(int i = 0; i < property_group.size(); i++) {
				if(alpha_property != NULL && property_group[i]->GetType().IsSameType(NiAlphaProperty::TYPE)) {
					NiAlphaPropertyRef current_alpha_property = DynamicCast<NiAlphaProperty>(property_group[i]);
					if(current_alpha_property == alpha_property) {
						similarities++;
					}
				}
				if(shader_property != NULL && property_group[i]->GetType().IsSameType(BSLightingShaderProperty::TYPE)) {
					BSLightingShaderPropertyRef current_shader_property = DynamicCast<BSLightingShaderProperty>(property_group[i]);
					if(current_shader_property == shader_property) {
						similarities++;
					}
				}
			}

			if(valid_properties != 0 && similarities == valid_properties) {
				found_material = this->imported_materials[i];
				break;
			}
		}

		if(found_material.isNull()) {
			MFnPhongShader new_shader;
			found_material = new_shader.create();

			if(shader_property != NULL) {
				MColor reflective_color(shader_property->GetSpecularColor().r, shader_property->GetSpecularColor().g, shader_property->GetSpecularColor().b);
				MColor incadescence_color(shader_property->GetEmissiveColor().r, shader_property->GetEmissiveColor().g, shader_property->GetEmissiveColor().b);
				float glow_intensity = shader_property->GetEmissiveMultiple() / 1000;
				float reflective_strength = shader_property->GetSpecularStrength() / 1000;
				float cosine_power = shader_property->GetGlossiness();
				float transparency = 1.0f - shader_property->GetAlpha();

				new_shader.setReflectedColor(reflective_color);
				new_shader.findPlug("reflectivity").setFloat(reflective_strength);
				new_shader.setIncandescence(incadescence_color);
				new_shader.setGlowIntensity(glow_intensity);
				new_shader.setCosPower(cosine_power);
				new_shader.setTransparency(transparency);

				BSShaderTextureSetRef texture_set = shader_property->GetTextureSet();
				if(texture_set != NULL) {
					MDGModifier dg_modifier;
					string color_texture = texture_set->GetTexture(0);
					string normal_map = texture_set->GetTexture(1);

					if(color_texture.length() > 0) {
						MString color_texture_file = this->GetTextureFilePath(color_texture);
						MFnDependencyNode color_texture_node;
						color_texture_node.create(MString("file"), MString(color_texture.c_str()));
						color_texture_node.findPlug("ftn").setValue(color_texture_file);
						MItDependencyNodes node_it(MFn::kTextureList);
						MFnDependencyNode texture_list(node_it.item());
						MPlug textures = texture_list.findPlug(MString("textures"));

						MPlug current_texture;
						int next = 0;
						while(true) {
							current_texture = textures.elementByLogicalIndex(next);

							if(current_texture.isConnected() == false) {
								break;
							}
							next++;
						}
						MPlug texture_message = color_texture_node.findPlug(MString("message"));
						dg_modifier.connect(texture_message, current_texture);

						MPlug color_texture_outcolor = color_texture_node.findPlug("outColor");
						MPlug new_shader_color = new_shader.findPlug("color");

						dg_modifier.connect(color_texture_outcolor, new_shader_color);

						if(alpha_property != NULL) {
							MPlug color_texture_outalpha = color_texture_node.findPlug("outTransparency");
							MPlug new_shader_transparency = new_shader.findPlug("transparency");
							dg_modifier.connect(color_texture_outalpha, new_shader_transparency);
						}

						MFnDependencyNode color_texture_placement;
						color_texture_placement.create("place2dTexture", "place2dTexture");

						dg_modifier.connect( color_texture_placement.findPlug("coverage"), color_texture_node.findPlug("coverage") );
						dg_modifier.connect( color_texture_placement.findPlug("mirrorU"), color_texture_node.findPlug("mirrorU") );
						dg_modifier.connect( color_texture_placement.findPlug("mirrorV"), color_texture_node.findPlug("mirrorV") );
						dg_modifier.connect( color_texture_placement.findPlug("noiseUV"), color_texture_node.findPlug("noiseUV") );
						dg_modifier.connect( color_texture_placement.findPlug("offset"), color_texture_node.findPlug("offset") );
						dg_modifier.connect( color_texture_placement.findPlug("outUV"), color_texture_node.findPlug("uvCoord") );
						dg_modifier.connect( color_texture_placement.findPlug("outUvFilterSize"), color_texture_node.findPlug("uvFilterSize") );
						dg_modifier.connect( color_texture_placement.findPlug("repeatUV"), color_texture_node.findPlug("repeatUV") );
						dg_modifier.connect( color_texture_placement.findPlug("rotateFrame"), color_texture_node.findPlug("rotateFrame") );
						dg_modifier.connect( color_texture_placement.findPlug("rotateUV"), color_texture_node.findPlug("rotateUV") );
						dg_modifier.connect( color_texture_placement.findPlug("stagger"), color_texture_node.findPlug("stagger") );
						dg_modifier.connect( color_texture_placement.findPlug("translateFrame"), color_texture_node.findPlug("translateFrame") );
						dg_modifier.connect( color_texture_placement.findPlug("vertexCameraOne"), color_texture_node.findPlug("vertexCameraOne") );
						dg_modifier.connect( color_texture_placement.findPlug("vertexUvOne"), color_texture_node.findPlug("vertexUvOne") );
						dg_modifier.connect( color_texture_placement.findPlug("vertexUvTwo"), color_texture_node.findPlug("vertexUvTwo") );
						dg_modifier.connect( color_texture_placement.findPlug("vertexUvThree"), color_texture_node.findPlug("vertexUvThree") );
						dg_modifier.connect( color_texture_placement.findPlug("wrapU"), color_texture_node.findPlug("wrapU") );
						dg_modifier.connect( color_texture_placement.findPlug("wrapV"), color_texture_node.findPlug("wrapV") );

					}

					if(normal_map.length() > 0) {
						MString normal_map_file = this->GetTextureFilePath(normal_map);
						MFnDependencyNode normal_map_node;
						normal_map_node.create(MString("file"), MString(normal_map.c_str()));
						normal_map_node.findPlug("ftn").setValue(normal_map_file);
						MItDependencyNodes node_it(MFn::kTextureList);
						MFnDependencyNode texture_list(node_it.item());
						MPlug textures = texture_list.findPlug(MString("textures"));

						MPlug current_texture;
						int next = 0;
						while(true) {
							current_texture = textures.elementByLogicalIndex(next);

							if(current_texture.isConnected() == false) {
								break;
							}
							next++;
						}

						MPlug texture_message = normal_map_node.findPlug("message");
						dg_modifier.connect(texture_message, current_texture);

						MFnDependencyNode bump2d_node;
						bump2d_node.create(MString("bump2d"),MString("bump2d"));
						MPlug bump_interp = bump2d_node.findPlug("bumpInterp");
						bump_interp.setInt(1);
						MString str = bump_interp.info();

						MPlug normal_map_outAlpha = normal_map_node.findPlug("outAlpha");
						MPlug bump2d_bumpvalue = bump2d_node.findPlug("bumpValue");
						dg_modifier.connect(normal_map_outAlpha, bump2d_bumpvalue);

						MPlug bump2d_outnormal = bump2d_node.findPlug("outNormal");
						MPlug new_shader_normalcamera = new_shader.findPlug("normalCamera");
						dg_modifier.connect(bump2d_outnormal, new_shader_normalcamera);

						MFnDependencyNode normal_map_placement;
						normal_map_placement.create("place2dTexture", "place2dTexture");

						dg_modifier.connect( normal_map_placement.findPlug("coverage"), normal_map_node.findPlug("coverage") );
						dg_modifier.connect( normal_map_placement.findPlug("mirrorU"), normal_map_node.findPlug("mirrorU") );
						dg_modifier.connect( normal_map_placement.findPlug("mirrorV"), normal_map_node.findPlug("mirrorV") );
						dg_modifier.connect( normal_map_placement.findPlug("noiseUV"), normal_map_node.findPlug("noiseUV") );
						dg_modifier.connect( normal_map_placement.findPlug("offset"), normal_map_node.findPlug("offset") );
						dg_modifier.connect( normal_map_placement.findPlug("outUV"), normal_map_node.findPlug("uvCoord") );
						dg_modifier.connect( normal_map_placement.findPlug("outUvFilterSize"), normal_map_node.findPlug("uvFilterSize") );
						dg_modifier.connect( normal_map_placement.findPlug("repeatUV"), normal_map_node.findPlug("repeatUV") );
						dg_modifier.connect( normal_map_placement.findPlug("rotateFrame"), normal_map_node.findPlug("rotateFrame") );
						dg_modifier.connect( normal_map_placement.findPlug("rotateUV"), normal_map_node.findPlug("rotateUV") );
						dg_modifier.connect( normal_map_placement.findPlug("stagger"), normal_map_node.findPlug("stagger") );
						dg_modifier.connect( normal_map_placement.findPlug("translateFrame"), normal_map_node.findPlug("translateFrame") );
						dg_modifier.connect( normal_map_placement.findPlug("vertexCameraOne"), normal_map_node.findPlug("vertexCameraOne") );
						dg_modifier.connect( normal_map_placement.findPlug("vertexUvOne"), normal_map_node.findPlug("vertexUvOne") );
						dg_modifier.connect( normal_map_placement.findPlug("vertexUvTwo"), normal_map_node.findPlug("vertexUvTwo") );
						dg_modifier.connect( normal_map_placement.findPlug("vertexUvThree"), normal_map_node.findPlug("vertexUvThree") );
						dg_modifier.connect( normal_map_placement.findPlug("wrapU"), normal_map_node.findPlug("wrapU") );
						dg_modifier.connect( normal_map_placement.findPlug("wrapV"), normal_map_node.findPlug("wrapV") );
					}

					dg_modifier.doIt();
				}

				MString mel_command;
			
				MDGModifier dg_modifier;
				MFnDependencyNode bs_lightning_node;
				bs_lightning_node.create("bsLightningShader");

				MPlug input_message_shader = bs_lightning_node.findPlug("targetShader");
				MPlug new_shader_out = new_shader.findPlug("message");
				dg_modifier.connect(new_shader_out, input_message_shader);
				dg_modifier.doIt();

				MStringArray shader_flags1_strings = BSLightningShader::shaderFlags1ToStringArray(shader_property->GetShaderFlags1());
				MStringArray shader_flags2_strings = BSLightningShader::shaderFlags2ToStringArray(shader_property->GetShaderFlags2());

				mel_command = "setAttr ";
				mel_command += (bs_lightning_node.name() + ".shaderFlags1");
				mel_command += " -type \"stringArray\" ";
				mel_command += shader_flags1_strings.length();
				for (int z = 0; z < shader_flags1_strings.length(); z++) {
					mel_command += (" \"" + shader_flags1_strings[z] + "\"");
				}
				MGlobal::executeCommand(mel_command);

				mel_command = "setAttr ";
				mel_command += (bs_lightning_node.name() + ".shaderFlags2");
				mel_command += " -type \"stringArray\" ";
				mel_command += shader_flags2_strings.length();
				for (int z = 0; z < shader_flags2_strings.length(); z++) {
					mel_command += (" \"" + shader_flags2_strings[z] + "\"");
				}
				MGlobal::executeCommand(mel_command);

				MPlug texture_slot_plug;

				for (int y = 2; y <= 8; y++) {
					mel_command = "textureSlot";
					mel_command += y;

					texture_slot_plug = bs_lightning_node.findPlug(mel_command);

					texture_slot_plug.setString(texture_set->GetTexture(y).c_str());
				}

				MPlug shader_type_plug = bs_lightning_node.findPlug("shaderType");
				shader_type_plug.setString(BSLightningShader::skyrimShaderTypeToString(shader_property->GetSkyrimShaderType()));

				MPlug others_plug = bs_lightning_node.findPlug("lightingEffect1");
				others_plug.setFloat(shader_property->GetLightingEffect1());

				others_plug = bs_lightning_node.findPlug("lightingEffect2");
				others_plug.setFloat(shader_property->GetLightingEffect2());

				others_plug = bs_lightning_node.findPlug("environmentMapScale");
				others_plug.setFloat(shader_property->GetEnvironmentMapScale());

				others_plug = bs_lightning_node.findPlug("skinTintColor");
				others_plug.child(0).setValue(shader_property->GetSkinTintColor().r);
				others_plug.child(1).setValue(shader_property->GetSkinTintColor().g);
				others_plug.child(2).setValue(shader_property->GetSkinTintColor().b);

				others_plug = bs_lightning_node.findPlug("hairTintColor");
				others_plug.child(0).setValue(shader_property->GetHairTintColor().r);
				others_plug.child(1).setValue(shader_property->GetHairTintColor().g);
				others_plug.child(2).setValue(shader_property->GetHairTintColor().b);

			}

			vector<NiPropertyRef> property_group;

			if(shader_property != NULL) {
				property_group.push_back(DynamicCast<NiProperty>(shader_property));
			}
			if(alpha_property != NULL) {
				property_group.push_back(DynamicCast<NiProperty>(alpha_property));
			}

			this->imported_materials.push_back(found_material);
			this->property_groups.push_back(property_group);
			this->translatorData->importedMaterials.push_back(pair<vector<NiPropertyRef>, MObject>(property_group, found_material));
		}
		return;
	}

	if(root->GetType().IsDerivedType(NiNode::TYPE)) {
		NiNodeRef node = DynamicCast<NiNode>(root);
		vector<NiAVObjectRef> children = node->GetChildren();

		for(int i = 0; i < children.size(); i++) {
			this->GatherMaterialsAndTextures(children[i]);
		}
	}
}

string NifMaterialImporterSkyrimFallout4::asString( bool verbose /*= false */ ) const {
	stringstream out;

	out<<NifMaterialImporter::asString(verbose)<<endl;
	out<<"NifMaterialExporterSkyrim"<<endl;

	return out.str();
}

const Type& NifMaterialImporterSkyrimFallout4::GetType() const {
	return TYPE;
}

const Type NifMaterialImporterSkyrimFallout4::TYPE("NifMaterialImporterSkyrimFallout4",&NifMaterialImporter::TYPE);


