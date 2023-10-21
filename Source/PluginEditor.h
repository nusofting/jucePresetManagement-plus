#pragma once




#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Gui/PresetPanel.h"
#include "Gui/ProgramList.h"
#include "Gui/TableListComponent.h"

class JucePresetManagerAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    JucePresetManagerAudioProcessorEditor (JucePresetManagerAudioProcessor&);
    ~JucePresetManagerAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    GenericAudioProcessorEditor genericAudioProcessorEditor;
    Gui::PresetPanel presetPanel;

    ProgramList programList;
    juce::TextButton cShow{ "show" };

    TableListComponent  cTableList;

    JucePresetManagerAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JucePresetManagerAudioProcessorEditor)
};
