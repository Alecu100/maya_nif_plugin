#include "include/Exporters/NifExportingFixtureSkyrim.h"

NifExportingFixtureSkyrim::NifExportingFixtureSkyrim() {

}

NifExportingFixtureSkyrim::NifExportingFixtureSkyrim(NifTranslatorDataRef translatorData, NifTranslatorOptionsRef translatorOptions, NifTranslatorUtilsRef translatorUtils) {
	this->translatorOptions = translatorOptions;
	this->translatorData = translatorData;
	this->translatorUtils = translatorUtils;
	this->nodeExporter = new NifNodeExporter(translatorOptions, translatorData, translatorUtils);
	this->meshExporter = new NifMeshExporterSkyrim(this->nodeExporter, translatorOptions, translatorData, translatorUtils);
	this->materialExporter = new NifMaterialExporterSkyrim(translatorOptions, translatorData, translatorUtils);
	this->animationExporter = new NifAnimationExporter(translatorOptions, translatorData, translatorUtils);
}


string NifExportingFixtureSkyrim::asString(bool verbose /*= false */) const {
	stringstream out;

	out << NifDefaultExportingFixture::asString(verbose) << endl;
	out << this->nodeExporter->asString(verbose) << endl;
	out << this->meshExporter->asString(verbose) << endl;
	out << this->materialExporter->asString(verbose) << endl;

	return out.str();
}

const Type& NifExportingFixtureSkyrim::getType() const {
	return TYPE;
}


const Type NifExportingFixtureSkyrim::TYPE("NifExportingFixtureSkyrim", &NifDefaultExportingFixture::TYPE);
