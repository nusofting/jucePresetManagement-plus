#pragma once

#include <JuceHeader.h>

//@ The presets_registry_log.xml is a temporary file used to build the presets table layout
// the data in this file are duplicates of the properties store in each preset file
class PresetsRegistry
{
public:

	PresetsRegistry() {

		// TO DO: unify the preset directory setup
		const File defaultDirectoryCopy{ File::getSpecialLocation(
		File::SpecialLocationType::commonDocumentsDirectory)
		.getChildFile(ProjectInfo::companyName)
		.getChildFile(ProjectInfo::projectName)
		};

		defaultDirectoryPath = defaultDirectoryCopy.getFullPathName();
	};

	StringArray getAllPresets() const
	{
		const File defaultDirectory(defaultDirectoryPath);

		StringArray presets;
		const auto fileArray = defaultDirectory.findChildFiles(
			File::TypesOfFileToFind::findFiles, false, "*.xml");
		for (const auto& file : fileArray)
		{
			presets.add(file.getFileNameWithoutExtension());
		}
		return presets;
	}

	bool SaveRegistryToXMLFile()
	{
		XmlElement* columns = new XmlElement("COLUMNS");
		//populate columns
		for (int i = 0; i < 6; ++i)
		{
			XmlElement* line = new XmlElement("COLUMN");
			line->setAttribute("columnId", String(i + 1));

			switch (i)
			{
			case 0:
				line->setAttribute("name", "ID");
				line->setAttribute("width", "50");
				break;
			case 1:
				line->setAttribute("name", "Preset");
				line->setAttribute("width", "200");
				break;
			case 2:
				line->setAttribute("name", "Type");
				line->setAttribute("width", "50");
				break;
			case 3:
				line->setAttribute("name", "Date");
				line->setAttribute("width", "50");
				break;
			case 4:
				line->setAttribute("name", "Author");
				line->setAttribute("width", "100");
				break;
			case 5:
				line->setAttribute("name", "Notes");
				line->setAttribute("width", "200");
				break;
			}

			if (line) 	columns->addChildElement(line);
		}

		XmlElement* data = new XmlElement("DATA");
		//populate data (to be expanded to read from preset files)
		StringArray presetNames = getAllPresets();
		int count = 1;
		for (auto& prstName : presetNames)
		{
			if (prstName != String("presets_registry_log"))
			{
				XmlElement* line = new XmlElement("ITEM");
				line->setAttribute("ID", String(count++));
				line->setAttribute("Preset", prstName);
				line->setAttribute("Type",   "empty");
				line->setAttribute("Date",   "empty");
				line->setAttribute("Author", "empty");
				line->setAttribute("Notes",  "empty");

				if (line) 	data->addChildElement(line);
			}
		}

		XmlElement main("PRESETS_TABLE_DATA");
		main.addChildElement(columns);
		main.addChildElement(data);

		String fileName = defaultDirectoryPath + "//" + "presets_registry_log.xml";

		File f2(fileName);
		if (f2.create().failed()) return false;

		return f2.replaceWithText(main.toString());
	}

	String defaultDirectoryPath;


private:
	//void valueTreeRedirected(ValueTree& treeWhichHasBeenChanged) override;

	//AudioProcessorValueTreeState& valueTreeState;
	//Value currentPreset;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetsRegistry)	
};