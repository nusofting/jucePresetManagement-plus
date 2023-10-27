#pragma once




#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Gui/PresetPanel.h"
#include "Gui/ProgramList.hpp"


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

  

    JucePresetManagerAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JucePresetManagerAudioProcessorEditor)
};
