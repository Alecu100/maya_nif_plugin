#include "include/Exporters/NifExportingFixtureFallout4.h"

NifExportingFixtureFallout4::NifExportingFixtureFallout4() {

}

NifExportingFixtureFallout4::NifExportingFixtureFallout4(NifTranslatorDataRef translatorData, NifTranslatorOptionsRef translatorOptions, NifTranslatorUtilsRef translatorUtils) {
	this->translatorOptions = translatorOptions;
	this->translatorData = translatorData;
	this->translatorUtils = translatorUtils;
	this->nodeExporter = new NifNodeExporter(translatorOptions, translatorData, translatorUtils);
	this->meshExporter = new NifMeshExporterFallout4(this->nodeExporter, translatorOptions, translatorData, translatorUtils);
	this->materialExporter = new NifMaterialExporterSkyrim(translatorOptions, translatorData, translatorUtils);
	this->animationExporter = new NifAnimationExporter(translatorOptions, translatorData, translatorUtils);
}


string NifExportingFixtureFallout4::asString(bool verbose /*= false */) const {
	stringstream out;

	out << NifDefaultExportingFixture::asString(verbose) << endl;
	out << this->nodeExporter->asString(verbose) << endl;
	out << this->meshExporter->asString(verbose) << endl;
	out << this->materialExporter->asString(verbose) << endl;

	return out.str();
}

const Type& NifExportingFixtureFallout4::getType() const {
	return TYPE;
}


const Type NifExportingFixtureFallout4::TYPE("NifExportingFixtureFallout4", &NifDefaultExportingFixture::TYPE);
