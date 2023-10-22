/*
  ==============================================================================

	ProgramList.h
	Author: Luigi Felici
    // from JUCE Demo

  ==============================================================================
*/

#pragma once


#include <JuceHeader.h>

//==============================================================================
/**
    This class shows how to implement a TableListBoxModel to show in a TableListBox.
*/
class TableListComponent    : public Component,
                              public TableListBoxModel
{
public:
    TableListComponent()
    {
        // Load some data from an embedded XML file..
        loadData();

        // Create our table component and add it to this component..
        addAndMakeVisible (table);
        table.setModel (this);

        // give it a border
        table.setColour (ListBox::outlineColourId, Colours::grey);
        table.setOutlineThickness (1);

        // Add some columns to the table header, based on the column list in our database..
        for (auto* columnXml : columnList->getChildIterator())
        {
            table.getHeader().addColumn (columnXml->getStringAttribute ("name"),
                                         columnXml->getIntAttribute ("columnId"),
                                         columnXml->getIntAttribute ("width"),
                                         50, 400,
                                         TableHeaderComponent::defaultFlags);
        }

        // we could now change some initial settings..
        table.getHeader().setSortColumnId (1, true); // sort forwards by the ID column
        //table.getHeader().setColumnVisible (7, false); // hide the "length" column until the user shows it

        // un-comment this line to have a go of stretch-to-fit mode
        table.getHeader().setStretchToFitActive (true);

        table.setMultipleSelectionEnabled (false);     
    }

    // This is overloaded from TableListBoxModel, and must return the total number of rows in our table
    int getNumRows() override
    {
        return numRows;
    }

    // This is overloaded from TableListBoxModel, and should fill in the background of the whole row
    void paintRowBackground (Graphics& g, int rowNumber, int /*width*/, int /*height*/, bool rowIsSelected) override
    {
        auto alternateColour = getLookAndFeel().findColour (ListBox::backgroundColourId)
                                               .interpolatedWith (getLookAndFeel().findColour (ListBox::textColourId), 0.03f);
        if (rowIsSelected)
            g.fillAll (Colours::darkturquoise);
        else if (rowNumber % 2)
            g.fillAll (alternateColour);
    }

    // This is overloaded from TableListBoxModel, and must paint any cells that aren't using custom
    // components.
    void paintCell (Graphics& g, int rowNumber, int columnId,
                    int width, int height, bool /*rowIsSelected*/) override
    {
        g.setColour (getLookAndFeel().findColour (ListBox::textColourId));
        g.setFont (font);

        if (auto* rowElement = dataList->getChildElement (rowNumber))
        {
            auto text = rowElement->getStringAttribute (getAttributeNameForColumnId (columnId));

            g.drawText (text, 2, 0, width - 4, height, Justification::centredLeft, true);
        }

        g.setColour (getLookAndFeel().findColour (ListBox::backgroundColourId));
        g.fillRect (width - 1, 0, 1, height);
    }

    // This is overloaded from TableListBoxModel, and tells us that the user has clicked a table header
    // to change the sort order.
    void sortOrderChanged (int newSortColumnId, bool isForwards) override
    {
        if (newSortColumnId != 0)
        {
            DemoDataSorter sorter (getAttributeNameForColumnId (newSortColumnId), isForwards);
            dataList->sortChildElements (sorter);

            table.updateContent();
        }
    }

    // This is overloaded from TableListBoxModel, and must update any custom components that we're using
    Component* refreshComponentForCell (int rowNumber, int columnId, bool isRowSelected,
                                        Component* existingComponentToUpdate) override
    {
        if (columnId == 1 ) // The ID  does not have a custom editable component
        {
            jassert (existingComponentToUpdate == nullptr);
            return nullptr;
        }

        // The other columns are editable text columns, for which we use the custom Label component
        auto* textLabel = static_cast<EditableTextCustomComponent*> (existingComponentToUpdate);

        // same as above...
        if (textLabel == nullptr)
            textLabel = new EditableTextCustomComponent (*this);

        textLabel->setRowAndColumn (rowNumber, columnId);

   
        return textLabel;
    }

    // This is overloaded from TableListBoxModel, and should choose the best width for the specified
    // column.
    int getColumnAutoSizeWidth (int columnId) override
    {
        int widest = 12;

        // find the widest bit of text in this column..
        for (int i = getNumRows(); --i >= 0;)
        {
            if (auto* rowElement = dataList->getChildElement (i))
            {
                auto text = rowElement->getStringAttribute (getAttributeNameForColumnId (columnId));

                widest = jmax (widest, font.getStringWidth (text));
            }
        }

        return widest + 8;
    }

    //// A couple of quick methods to set and get cell values when the user changes them
    //int getRating (const int rowNumber) const
    //{
    //    return dataList->getChildElement (rowNumber)->getIntAttribute ("Rating");
    //}

    //void setRating (const int rowNumber, const int newRating)
    //{
    //    dataList->getChildElement (rowNumber)->setAttribute ("Rating", newRating);
    //}

    // A couple of quick methods to set and get cell values when the user changes them
    String getText (const int columnNumber, const int rowNumber) const
    {
        return dataList->getChildElement (rowNumber)->getStringAttribute ( getAttributeNameForColumnId(columnNumber));
    }

    void setText (const int columnNumber, const int rowNumber, const String& newText)
    {
        auto columnName = table.getHeader().getColumnName (columnNumber);
        dataList->getChildElement (rowNumber)->setAttribute (columnName, newText);
    }

    //==============================================================================
    void resized() override
    {
        // position our table with a gap around its edge
        table.setBoundsInset (BorderSize<int> (4));
    }
    //==============================================================================

  // return the ID of the selected row as int
   const int getIdSelected()
    {
       const auto indexRow = table.getLastRowSelected();       

       if (indexRow > -1 && dataList)
       {
           const String text = dataList->getChildElement(indexRow)->getAttributeValue(0);
           return text.getIntValue();
       }
       else
           return indexRow; 
    }

   // do we need get() and set() methods and use private variables?
   bool bCellChanged = false;
   int editedRow = -1;
   int editedColumn = -1;

private:
    TableListBox table;     // the table component itself
    Font font  { 14.0f };

    std::unique_ptr<XmlElement> presetsData;  // This is the XML document loaded from the file "presets registry.xml"
    XmlElement* columnList = nullptr;         // A pointer to the sub-node of presetsData that contains the list of columns
    XmlElement* dataList   = nullptr;         // A pointer to the sub-node of presetsData that contains the list of data rows
    int numRows = 9;                          // The number of rows of data we've got


  

    //==============================================================================
    // This is a custom Label component, which we use for the table's editable text columns.
    class EditableTextCustomComponent  : public Label
    {
    public:
        EditableTextCustomComponent (TableListComponent& td)  : owner (td)
        {
            // double click to edit the label text; single click handled below
            setEditable (false, true, false);


			Label::onTextChange = [&]
				{
					const auto indexRow = owner.table.getLastRowSelected();

                    owner.editedRow = indexRow;
                    owner.editedColumn = columnId;
                    owner.bCellChanged = true;

					if (indexRow > -1)
					{
						AlertWindow::showMessageBoxAsync(MessageBoxIconType::InfoIcon,
							"Debug",
							"text changed at cell:" + String(indexRow + 1) +
                            ", " + String(owner.editedColumn) +
                            + ". Here we need to hint to save the XML of the registry" 
                            + " by flashing [save changes]"
						);
					}

                                 

				};
		}

        void mouseDown (const MouseEvent& event) override
        {
            // single click on the label should simply select the row
            owner.table.selectRowsBasedOnModifierKeys (row, event.mods, false);

            Label::mouseDown (event);
        }

        void textWasEdited() override
        {
            owner.setText (columnId, row, getText());
        }

        // Our code will call this when we may need to update our contents
        void setRowAndColumn (const int newRow, const int newColumn)
        {
            row = newRow;
            columnId = newColumn;
            setText (owner.getText(columnId, row), dontSendNotification);
        }

        void paint (Graphics& g) override
        {
            auto& lf = getLookAndFeel();
            if (! dynamic_cast<LookAndFeel_V4*> (&lf))
                lf.setColour (textColourId, Colours::black);

            lf.setColour(outlineColourId, Colours::black);

            Label::paint (g);           
        }

    private:
        TableListComponent& owner;
        int row = 0;
        int columnId = 0;
        Colour textColour;
    };

    //==============================================================================
    // A comparator used to sort our data when the user clicks a column header
    class DemoDataSorter
    {
    public:
        DemoDataSorter (const String& attributeToSortBy, bool forwards)
            : attributeToSort (attributeToSortBy),
              direction (forwards ? 1 : -1)
        {
        }

        int compareElements (XmlElement* first, XmlElement* second) const
        {
            auto result = first->getStringAttribute (attributeToSort)
                                .compareNatural (second->getStringAttribute (attributeToSort));

            if (result == 0)
                result = first->getStringAttribute ("ID")
                               .compareNatural (second->getStringAttribute ("ID"));

            return direction * result;
        }

    private:
        String attributeToSort;
        int direction = 0;
    };

    //==============================================================================
    // this loads the database XML file into memory
    void loadData()
    {
        // TO DO : not to use hard coded path
        juce::File xmlTabledata{ "D:\\daHornet_bis_JUCE\\jucePresetManagement-plus\\Source\\Gui\\presets registry.xml" };

        presetsData = XmlDocument::parse(xmlTabledata.loadFileAsString());

        dataList   = presetsData->getChildByName ("DATA");
        columnList = presetsData->getChildByName ("COLUMNS");

        numRows = dataList->getNumChildElements();
    }

    // (a utility method to search our XML for the attribute that matches a column ID)
    String getAttributeNameForColumnId (const int columnId) const
    {
        for (auto* columnXml : columnList->getChildIterator())
        {
            if (columnXml->getIntAttribute ("columnId") == columnId)
                return columnXml->getStringAttribute ("name");
        }

        return {};
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TableListComponent)
};