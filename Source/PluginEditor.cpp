#include "PluginProcessor.h"
#include "PluginEditor.h"

JucePresetManagerAudioProcessorEditor::JucePresetManagerAudioProcessorEditor (JucePresetManagerAudioProcessor& p) :
	AudioProcessorEditor(&p),
	genericAudioProcessorEditor(p),
	presetPanel(p.getPresetManager()),
    audioProcessor(p)
{
    addAndMakeVisible(genericAudioProcessorEditor);
    addAndMakeVisible(presetPanel);



    //create, show and hide the programList

    addAndMakeVisible(programList);
    programList.ShowHide(false);

    cShow.setColour(TextButton::buttonNormal, Colours::darkblue);
    addAndMakeVisible(cShow);
    cShow.onClick = [&] { programList.ShowHide(true); };
    programList.getCtrlButton(&cShow);

     

    setResizable(true, true);
    setSize (800, 400);
}

JucePresetManagerAudioProcessorEditor::~JucePresetManagerAudioProcessorEditor()
{
}

void JucePresetManagerAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void JucePresetManagerAudioProcessorEditor::resized()
{
    genericAudioProcessorEditor.setBounds(getLocalBounds()
        .withSizeKeepingCentre(getLocalBounds().proportionOfWidth(0.9f), getLocalBounds().proportionOfHeight(0.5f)));
    presetPanel.setBounds(getLocalBounds().removeFromTop(proportionOfHeight(0.1f)));

    const auto rectAbove = presetPanel.getBounds();
    cShow.setBounds(0, rectAbove.getBottom(), rectAbove.getWidth(), rectAbove.getHeight()/2);

    programList.setBounds(getLocalBounds());

}