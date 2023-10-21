/*
  ==============================================================================

	ProgramList.h
	Author: Luigi Felici

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>





class ProgramList : public Component, KeyListener
{
public:
	ProgramList()
	{
		setSize(200, 100);
		addKeyListener(this);
		setWantsKeyboardFocus(true);

		ProgramsNames.ensureStorageAllocated(128);

		for (int i = 0; i < 128; i++)
		{
			ProgramsNames.add(" prg " + String(i));
		}

		addAndMakeVisible(cSomeFeat);

		cSomeFeat.onClick = [&] {

			AlertWindow::showMessageBoxAsync(MessageBoxIconType::InfoIcon,
				"Debug",
				"You clicked it!");
		};

	}

	~ProgramList()
	{
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


		if (theCurrentProgram >= 0) CurrentProgram = theCurrentProgram;
		const int animationTime = 150;

		if (b)
		{
			MakeProgramGrid();
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

	void setTitle(String title_)
	{
		//titleOfBankWindow = title_;
	}
	// Attach a callback that will be called when a Program label is clicked
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
	OwnedArray<Label> ProgramLabels;
	int CurrentProgram = 0;
	int swapSrc = -1, DestProgram = -1;
	bool bCalledToLoad = true;
	String progNameCurrentlyOnDisplay;
	int selectedBank_ = -1;

	Array<juce::String> ProgramsNames;


	TextButton cSomeFeat{ "example" };

	void MakeProgramGrid()
	{
		int row = 0, col = 0;

		// This function is called each time the program list is invoked, 
		// so in order to make sure that we have the current list we must destroy and create it each time.
		ProgramLabels.clear();

		// Create all 128 labels
		for (int i = 0; i < 128; i++)
		{
			bool bChangeColour = i & 1;// (i == CurrentProgram) || String(Programs[i].ProgramName) == progNameCurrentlyOnDisplay;

			// Get color for the active program
			Colour color = bChangeColour ? Colour(0xFF575500) : (col & 1 ? Colour(0xFF202020) : Colour(0xFF404035));

			// 8 * 16 = 128

			const int PROG_LABEL_WIDTH = winw / 8;
			const int PROG_LABEL_HEIGHT = winh / (16 + 1);

			// Get label coords
			int px = 2 + winx + col * (PROG_LABEL_WIDTH);
			int py = barHeight + winy + row * (PROG_LABEL_HEIGHT);

			// Create and draw label (automatically deleted by OwnedArray)
			auto l = new Label("Entry_" + String(i), ProgramsNames[i]);
			ProgramLabels.add(l);
			Rectangle<int> rect(px, py, PROG_LABEL_WIDTH, PROG_LABEL_HEIGHT);
			l->setBounds(rect.toNearestIntEdges());
			l->setColour(Label::backgroundColourId, color);
			l->setColour(Label::outlineColourId, color.contrasting());
			l->addMouseListener(this, true);
			addAndMakeVisible(ProgramLabels.getLast());

			// Next row
			if (++row > 15) { row = 0; col++; }
		}
	}

	// Catch the mouse up event, close the list at any click
	void mouseUp(const MouseEvent& event) override
	{
		if (event.eventComponent->getName().startsWith("Entry_"))
		{
			// Get the numerical ID from the component literal name
			DestProgram = event.eventComponent->getName().substring(6).getIntValue();
			if (!bCalledToLoad) { onClickToGet();  /*ShowHide(false); */ return; }

			// Do we have a program number?
			if (DestProgram > -1)
			{
				if (onClickProgram != nullptr)
					onClickProgram(DestProgram);

				AlertWindow::showMessageBoxAsync(MessageBoxIconType::InfoIcon,
					"Debug",
					String(DestProgram));

				//ShowHide(false);
			}
		}
		// Not clicked on any label? Just hide this component.
		else
			ShowHide(false);
	}

	bool keyPressed(const KeyPress& key, Component* originatingComponent) override
	{
		// Catch the ESC key to hide this component
		if (key == KeyPress::escapeKey)
			ShowHide(false);

		return false;
	}

	void resized() override
	{
		// const auto winRect = getLocalBounds().reduced(4);

		winx = 0;
		winy = 0;
		winw = getParentWidth();
		winh = getParentHeight();

		barHeight = winh / 18;

		MakeProgramGrid();

		cSomeFeat.setBounds(winx + 2, winy + 2, barHeight * 2, barHeight - 4);
	}

	TextButton* showButton = nullptr;

	int winx = 0;
	int winy = 0;
	int winw = 200;
	int winh = 100;
	int barHeight = winh / 18;

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
		g.setFont(float(barHeight) * 0.9f);
		g.drawFittedText("Presets", winx, winy, winw, barHeight, Justification::centred, 1.0f);

		g.setColour(Colour(0xffff0000));
		g.setFont(float(barHeight) * 0.9f);
		g.drawText("[X]", winx, winy, winw, barHeight, Justification::right);

	}

	// -------------------------------------------------------
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProgramList)
};
