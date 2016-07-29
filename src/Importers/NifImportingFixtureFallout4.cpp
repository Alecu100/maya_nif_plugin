#include "include/Importers/NifImportingFixtureFallout4.h"


NifImportingFixtureFallout4::NifImportingFixtureFallout4() {

}

NifImportingFixtureFallout4::NifImportingFixtureFallout4(NifTranslatorOptionsRef translatorOptions, NifTranslatorDataRef translatorData, NifTranslatorUtilsRef translatorUtils) {
	this->translatorOptions = translatorOptions;
	this->translatorData = translatorData;
	this->translatorUtils = translatorUtils;
	this->nodeImporter = new NifNodeImporterFallout4(translatorOptions, translatorData, translatorUtils);
	this->meshImporter = new NifMeshImporterFallout4(translatorOptions, translatorData, translatorUtils);
	this->materialImporter = new NifMaterialImporterSkyrimFallout4(translatorOptions, translatorData, translatorUtils);
	this->animationImporter = new NifAnimationImporter(translatorOptions, translatorData, translatorUtils);
}

string NifImportingFixtureFallout4::asString(bool verbose /*= false */) const {
	stringstream out;

	out << NifDefaultImportingFixture::asString(verbose) << endl;
	out << "NifImportingFixtureFallout4";

	return out.str();
}

const Type& NifImportingFixtureFallout4::GetType() const {
	return TYPE;
}

const Type NifImportingFixtureFallout4::TYPE("NifImportingFixtureFallout4", &NifDefaultImportingFixture::TYPE);
