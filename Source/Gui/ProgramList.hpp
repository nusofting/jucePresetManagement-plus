/*
  ==============================================================================

	ProgramList.h
	Author: Luigi Felici

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "TableListComponent.hpp"
#include "../Service/PresetsRegistry.hpp"
#include "../PluginProcessor.h"


class ProgramList : public Component, KeyListener, Timer
{
public:
	ProgramList(JucePresetManagerAudioProcessor& p) : audioProcessor(p)
	{
		setSize(200, 100);
		addKeyListener(this);
		setWantsKeyboardFocus(true);

		cSomeFeat.setColour(TextButton::buttonColourId, Colours::darkgreen);
		addAndMakeVisible(cSomeFeat);

		cSomeFeat.onClick = [&] {

			AlertWindow::showMessageBoxAsync(MessageBoxIconType::InfoIcon,
				"Debug",
				"You clicked it!");

			cTableList.bCellChanged = false;
			cSomeFeat.setColour(TextButton::buttonColourId, Colours::darkgreen);
			};

		cPassiveMsg.setEditable(false, false, false);
		cPassiveMsg.setColour(Label::outlineColourId, Colours::transparentBlack);
		cPassiveMsg.setColour(Label::textColourId, Colours::red);
		cPassiveMsg.setText("<<< Unsaved changes will be lost", NotificationType::dontSendNotification);
		addAndMakeVisible(cPassiveMsg);

		cTableList.setName("nTableList");	
		cTableList.buildTable(p.getProcPresetRegistry());
		addAndMakeVisible(cTableList);


		setTitle(); // can be changed

		startTimer(40);
	}

	~ProgramList()
	{
		stopTimer();
	}

	// This is called to show or hide the list; when it's requested to show, the currently selected program number is passed
	void ShowHide(bool b, int theCurrentProgram = -1, int selectedBank = -1, bool isToLoad = true)
	{
		bCalledToLoad = isToLoad;

		selectedBank_ = selectedBank;

		if (isToLoad) repaint();

		String curName;
		if (selectedBank >= 0 && selectedBank < 8)
			//     curName = String(String::CharPointerType(thisBankName[selectedBank]));

			if (curName.isNotEmpty())
				setTitle("PROGRAM LIST : " + curName + " Bank  : click a slot to load a preset");
			else
				setTitle();


		if (theCurrentProgram >= 0) CurrentProgram = theCurrentProgram;
		const int animationTime = 150;

		if (b)
		{
			Desktop::getInstance().getAnimator().fadeIn(this, animationTime);
			grabKeyboardFocus();
		}
		else
		{
			Desktop::getInstance().getAnimator().fadeOut(this, animationTime);
		}

		if (showButton)
		{
			showButton->setVisible(!b);
		}
	}

	void setTitle(String title_ = "Presets ToolBar")
	{
		titleOfBankWindow = title_;
	}

	// Attach a callback that will be called when a Program row is clicked
	std::function<void(int)> onClickProgram;

	std::function<void(void)> onClickToGet;

	int getSlotNumber() {
		return DestProgram;
	}
	void getProgNameString(const String& display)
	{
		progNameCurrentlyOnDisplay = display;
	}
	// get the pointer to the TextButton used to activate this component
	void getCtrlButton(TextButton* ctrl_)
	{
		showButton = ctrl_;
	}

private:

	JucePresetManagerAudioProcessor& audioProcessor;

	int CurrentProgram = 0;
	int PrevProgram = 0;
	int DestProgram = -1;


	bool bCalledToLoad = true;
	String progNameCurrentlyOnDisplay;
	int selectedBank_ = -1;

	Array<juce::String> ProgramsNames;

	//ToolBar objects
	TextButton cSomeFeat{ "save changes" };
	Label cPassiveMsg;
	String titleOfBankWindow;

	// main GUI launcher
	TextButton* showButton = nullptr;

	int winx = 0;
	int winy = 0;
	int winw = 200;
	int winh = 100;
	int barHeight = winh / 18;

	TableListComponent  cTableList;

	void paint(Graphics& g) override
	{
		// Transparent black background
		g.fillAll(Colour(0xA0000000));

		// Window background
		g.setColour(Colours::black);
		g.fillRect(winx, winy, winw, winh);

		// Top bar
		g.setColour(Colours::darkgrey);
		g.fillRect(winx, winy, winw, barHeight);

		// Outline
		g.setColour(Colours::grey);
		g.drawRect(winx, winy, winw, winh, 2);

		// Window title
		g.setColour(Colour(0xffbbaa00));
		g.setFont(float(barHeight) * 0.8f);
		g.drawFittedText(titleOfBankWindow, winx, winy, winw, barHeight, Justification::centred, 1.0f);

		g.setColour(Colour(0xffff0000));
		g.setFont(float(barHeight) * 0.8f);
		g.drawText("[X]", winx, winy, winw, barHeight, Justification::right);
	}

	void resized() override
	{
		// const auto winRect = getLocalBounds().reduced(4);

		winx = 0;
		winy = 0;
		winw = getParentWidth();
		winh = getParentHeight();

		barHeight = winh / 16;

		auto areaGrid = juce::Rectangle<int>(2, barHeight, winw, winh - barHeight);

		cTableList.setBounds(areaGrid);

		cSomeFeat.setBounds(winx + 2, winy + 2, barHeight * 2, barHeight - 4);
		cSomeFeat.changeWidthToFitText();

		Font msgFont(float(barHeight) * 0.6f);
		cPassiveMsg.setFont(msgFont);
		const int w = msgFont.getStringWidth("<<< Usaved changes will be lost");
		cPassiveMsg.setBounds(cSomeFeat.getRight(), winy + 2, w, barHeight - 4);
	}

	void timerCallback() override
	{
		if (cTableList.hasKeyboardFocus(true))
		{
			DestProgram = cTableList.getIdSelected();

			if (DestProgram > -1 && DestProgram != PrevProgram)
			{
				//AlertWindow::showMessageBoxAsync(MessageBoxIconType::InfoIcon,
				//	"Debug",
				//	String(DestProgram));

				PrevProgram = DestProgram;
			}	
		}

		if (cTableList.bCellChanged)
		{
			cSomeFeat.setColour(TextButton::buttonColourId, Colours::red);
		}

		cPassiveMsg.setVisible(cTableList.bCellChanged);
	}

	bool bCellChangedHistory = true;

	// Catch the mouse up event, close the list at any click not inside a child
	void mouseUp(const MouseEvent& event) override
	{
		ShowHide(false);
	}

	// Catch the ESC key to hide this component
	bool keyPressed(const KeyPress& key, Component* originatingComponent) override
	{		
		if (key == KeyPress::escapeKey)
			ShowHide(false);

		return false;
	}

	// -------------------------------------------------------
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProgramList)
};