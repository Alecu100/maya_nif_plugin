#include "include/Importers/NifImportingFixtureSkyrim.h"


NifImportingFixtureSkyrim::NifImportingFixtureSkyrim() {

}

NifImportingFixtureSkyrim::NifImportingFixtureSkyrim( NifTranslatorOptionsRef translatorOptions, NifTranslatorDataRef translatorData, NifTranslatorUtilsRef translatorUtils ) {
	this->translatorOptions = translatorOptions;
	this->translatorData = translatorData;
	this->translatorUtils = translatorUtils;
	this->nodeImporter = new NifNodeImporter(translatorOptions, translatorData, translatorUtils);
	this->meshImporter = new NifMeshImporterSkyrim(translatorOptions, translatorData, translatorUtils);
	this->materialImporter = new NifMaterialImporterSkyrimFallout4(translatorOptions, translatorData, translatorUtils);
	this->animationImporter = new NifAnimationImporter(translatorOptions, translatorData, translatorUtils);
}

string NifImportingFixtureSkyrim::asString( bool verbose /*= false */ ) const {
	stringstream out;

	out<<NifDefaultImportingFixture::asString(verbose)<<endl;
	out<<"NifSkyrimImportingFixture";

	return out.str();
}

const Type& NifImportingFixtureSkyrim::GetType() const {
	return TYPE;
}

const Type NifImportingFixtureSkyrim::TYPE("NifSkyrimImportingFixture", &NifDefaultImportingFixture::TYPE);
