#pragma once

#include <JuceHeader.h>

#include "../Source/PluginProcessor.h"


//@ The presets_registry_log.xml is a temporary file NOT used to build the presets table layout
// the data in this file are duplicates of the properties stored in each preset file
class PresetsRegistry
{
public:

	String defaultDirectoryPath;

	PresetsRegistry(XmlElement& procPresetReg_) : procPresetReg(procPresetReg_) {

		// TO DO: unify the preset directory setup
		const File defaultDirectoryCopy{ File::getSpecialLocation(
		File::SpecialLocationType::commonDocumentsDirectory)
		.getChildFile(ProjectInfo::companyName)
		.getChildFile(ProjectInfo::projectName)
		};

		defaultDirectoryPath = defaultDirectoryCopy.getFullPathName();
	}

	bool SaveRegistryToXMLFileAndMemory()
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
				line->setAttribute("width", "25");
				break;
			case 1:
				line->setAttribute("name", "Date");
				line->setAttribute("width", "50");
				break;
			case 2:
				line->setAttribute("name", "Preset");
				line->setAttribute("width", "200");
				break;
			case 3:
				line->setAttribute("name", "Type");
				line->setAttribute("width", "50");
				break;
			case 4:
				line->setAttribute("name", "Author");
				line->setAttribute("width", "100");
				break;
			case 5:
				line->setAttribute("name", "Notes");
				line->setAttribute("width", "280");
				break;
			}

			if (line) columns->addChildElement(line);
		}

		XmlElement* data = new XmlElement("DATA");
		//populate data (to be expanded to read from preset files)		
		int count = 0;
		getAllPresetsProps();
		for (auto& prstName : presets_)
		{
				XmlElement* line = new XmlElement("ITEM");
				line->setAttribute("ID", String(count+1));
				line->setAttribute("Preset", prstName);
				line->setAttribute("Type",   "empty");
				line->setAttribute("Date", dates_[count]);
				line->setAttribute("Author", "empty");
				line->setAttribute("Notes",  "empty");

				if (line) data->addChildElement(line);

				++count;			
		}

		// this is not a pointer so when it goes out of scope there are no leaks from the children
		XmlElement main = XmlElement("PRESETS_TABLE_DATA");
		main.addChildElement(columns);
		main.addChildElement(data);

		//store the presets_registry in stack memory by deep copy
		procPresetReg = XmlElement(main);

		File f1(defaultDirectoryPath);
		String fileName = f1.getParentDirectory().getFullPathName() + "//" + "presets_registry_log.xml";

		File f2(fileName);
		if (f2.create().failed()) return false;

		return f2.replaceWithText(main.toString());
	}

private:

	StringArray presets_;
	StringArray authors_;
	StringArray notes_;
	StringArray dates_;
	XmlElement& procPresetReg;

	void getAllPresetsProps()
	{
		const File defaultDirectory(defaultDirectoryPath);

		const auto fileArray = defaultDirectory.findChildFiles(
			File::TypesOfFileToFind::findFiles, false, "*.xml");
		for (const auto& file : fileArray)
		{
			Time fullTime = file.getLastModificationTime();
			String date = String(fullTime.getDayOfMonth()) + "/" +
				String(fullTime.getMonth()) + "/" + String(fullTime.getYear());

			dates_.add(date);

			presets_.add(file.getFileNameWithoutExtension());
		}
	}


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetsRegistry)	
};