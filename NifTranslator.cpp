#include "NifTranslator.h"


//#define _DEBUG
ofstream out;

//--Function Definitions--//

//--NifTranslator::identifyFile--//

// This routine must use passed data to determine if the file is of a type supported by this translator

// Code adapted from animImportExport example that comes with Maya 6.5

MPxFileTranslator::MFileKind NifTranslator::identifyFile(const MFileObject& fileName, const char* buffer, short size) const
{
	MString fName = fileName.name();
	if (fName.toUpperCase() != "NIF" && fName.toUpperCase() != "KF")
		return kNotMyFileType;

	return kIsMyFileType;
}

//--Plug-in Load/Unload--//

//--initializePlugin--//

// Code adapted from lepTranslator example that comes with Maya 6.5
MStatus initializePlugin(MObject obj)
{
#ifdef _DEBUG
	out.open( "C:\\Maya NIF Plug-in Log.txt", ofstream::binary );
#endif

	//out << "Initializing Plugin..." << endl;
	MStatus status;
	MFnPlugin plugin(obj, "NifTools", PLUGIN_VERSION);

	// Register the translator with the system
	status = plugin.registerFileTranslator(TRANSLATOR_NAME, //File Translator Name
	                                       "nifTranslator.rgb", //Icon
	                                       NifTranslator::creator, //Factory Function
	                                       "nifTranslatorOpts", //MEL Script for options dialog
	                                       NULL, //Default Options
	                                       false); //Requires MEL support

	if (!status)
	{
		status.perror("registerFileTranslator");
		return status;
	}

	status = plugin.registerNode("nifDismemberPartition", NifDismemberPartition::id, NifDismemberPartition::creator, NifDismemberPartition::initialize, MPxNode::kDependNode);
	if (!status)
	{
		status.perror("registerNifDismemberPartition");
		return status;
	}


	status = plugin.registerNode("bsLightningShader", BSLightningShader::id, BSLightningShader::creator, BSLightningShader::initialize, MPxNode::kDependNode);
	if (!status)
	{
		status.perror("registerBSLightningShader");
		return status;
	}

	status = plugin.registerNode("bsSegment", BSSegment::id, BSSegment::creator, BSSegment::initialize, MPxNode::kDependNode);
	if (!status)
	{
		status.perror("registerBSSegment");
		return status;
	}

	status = plugin.registerNode("bsSubSegment", BSSubSegment::id, BSSubSegment::creator, BSSubSegment::initialize, MPxNode::kDependNode);
	if (!status)
	{
		status.perror("registerBSSubSegment");
		return status;
	}

	//Execute the command to create the NifTools Menu
	MGlobal::executeCommand("nifTranslatorMenuCreate");


	//out << "Done Initializing." << endl;
	return status;
}

//--uninitializePlugin--//

// Code adapted from lepTranslator example that comes with Maya 6.5
MStatus uninitializePlugin(MObject obj)
{
	MStatus status;
	MFnPlugin plugin(obj);

	status = plugin.deregisterFileTranslator(TRANSLATOR_NAME);
	if (!status)
	{
		status.perror("deregisterFileTranslator");
		return status;
	}

	status = plugin.deregisterNode(NifDismemberPartition::id);
	if (!status)
	{
		status.perror("deregisterFileTranslator");
		return status;
	}

	//Execute the command to delete the NifTools Menu
	MGlobal::executeCommand("nifTranslatorMenuRemove");
	if (!status)
	{
		status.perror("deregisterNifDismemberPartition");
		return status;
	}

	return status;
}

//--NifTranslator::reader--//

//This routine is called by Maya when it is necessary to load a file of a type supported by this translator.
//Responsible for reading the contents of the given file, and creating Maya objects via API or MEL calls to reflect the data in the file.
MStatus NifTranslator::reader(const MFileObject& file, const MString& optionsString, MPxFileTranslator::FileAccessMode mode)
{
	NifTranslatorDataRef translator_data(new NifTranslatorData());
	NifTranslatorOptionsRef translator_options(new NifTranslatorOptions());
	NifTranslatorUtilsRef translator_utils(new NifTranslatorUtils(translator_data, translator_options));
	NifImportingFixtureRef importer;

	ImportType import_type = ImportType::Default;
	Header file_header = ReadHeader(file.fullName().asChar());

	vector<string> block_types = file_header.getBlockTypes();
	vector<unsigned short> block_types_index = file_header.getBlockTypeIndex();

	translator_options->ParseOptionsString(optionsString);

	if (block_types[block_types_index[0]] == NiControllerSequence::TYPE.GetTypeName())
	{
		import_type = ImportType::AnimationKF;
	}
	else if (file_header.getUserVersion() == 12 && file_header.getUserVersion2() == 83)
	{
		import_type = ImportType::SkyrimFallout;
	} else if (file_header.getUserVersion() == 12  && file_header.getUserVersion2() == 130)
	{
		import_type = ImportType::Fallout4;
	}
	else
	{
		for (int i = 0; i < block_types.size(); i++)
		{
			if (block_types[i] == BSDismemberSkinInstance::TYPE.GetTypeName() || block_types[i] == BSShaderTextureSet::TYPE.GetTypeName())
			{
				import_type = ImportType::SkyrimFallout;
			}
		}
	}

	if (import_type == ImportType::AnimationKF)
	{
		importer = new NifKFImportingFixture(translator_options, translator_data, translator_utils);
	}
	else if (import_type == ImportType::SkyrimFallout)
	{
		importer = new NifImportingFixtureSkyrim(translator_options, translator_data, translator_utils);
	} else if (import_type == ImportType::Fallout4)
	{
		importer = new NifImportingFixtureFallout4(translator_options, translator_data, translator_utils);
	}
	else if (import_type == ImportType::Default)
	{
		importer = new NifDefaultImportingFixture(translator_data, translator_options, translator_utils);
	} 

	return importer->ReadNodes(file);
}

//--NifTranslator::writer--//

//This routine is called by Maya when it is necessary to save a file of a type supported by this translator.
//Responsible for traversing all objects in the current Maya scene, and writing a representation to the given
//file in the supported format.
MStatus NifTranslator::writer(const MFileObject& file, const MString& optionsString, MPxFileTranslator::FileAccessMode mode)
{
	NifTranslatorOptionsRef translator_options(new NifTranslatorOptions());
	NifTranslatorDataRef translator_data(new NifTranslatorData());
	NifTranslatorUtilsRef translator_utils(new NifTranslatorUtils(translator_data, translator_options));

	translator_options->ParseOptionsString(optionsString);

	NifExportingFixtureRef exporting_fixture;

	string export_type = "geometry";
	if (translator_options->exportType.length() > 1)
	{
		export_type = translator_options->exportType;
	}

	if (export_type == "geometry")
	{
		if (translator_options->exportUserVersion == 12 && translator_options->exportUserVersion2 == 130)
		{
			exporting_fixture = new NifExportingFixtureFallout4(translator_data, translator_options, translator_utils);
		} 
		else if (translator_options->exportMaterialType == "standardmaterial")
		{
			exporting_fixture = new NifDefaultExportingFixture(translator_data, translator_options, translator_utils);
		} 
		else if (translator_options->exportMaterialType == "skyrimmaterial")
		{
			exporting_fixture = new NifExportingFixtureSkyrim(translator_data, translator_options, translator_utils);
		}
	}

	if (export_type == "animation")
	{
		exporting_fixture = new NifKFExportingFixture(translator_options, translator_data, translator_utils);
	}

	if (exporting_fixture != NULL)
	{
		return exporting_fixture->WriteNodes(file);
	}

	return MStatus::kFailure;
}
