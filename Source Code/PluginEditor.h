#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

// Custom LookAndFeel class for modern appearance
class ModernLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ModernLookAndFeel()
    {
        setColour(juce::Slider::thumbColourId, juce::Colour(65, 180, 230));
        setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(65, 180, 230));
        setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(30, 30, 30));
        setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentWhite);
        setColour(juce::ComboBox::backgroundColourId, juce::Colour(45, 45, 45));
        setColour(juce::ComboBox::outlineColourId, juce::Colour(65, 180, 230));
        setColour(juce::ComboBox::textColourId, juce::Colour(220, 220, 220));
        setColour(juce::ComboBox::arrowColourId, juce::Colour(65, 180, 230));
    }
    
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider)
    {
        // Calculate radius and center
        const float radius = juce::jmin(width / 2, height / 2) - 4.0f;
        const float centerX = x + width * 0.5f;
        const float centerY = y + height * 0.5f;
        const float rx = centerX - radius;
        const float ry = centerY - radius;
        const float rw = radius * 2.0f;
        
        // Calculate rotation angle
        const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        
        // Background circle
        g.setColour(slider.findColour(juce::Slider::rotarySliderOutlineColourId));
        g.fillEllipse(rx, ry, rw, rw);
        
        // Outer ring
        g.setColour(juce::Colour(60, 60, 60));
        g.drawEllipse(rx, ry, rw, rw, 1.0f);
        
        // Draw filled arc for value
        if (radius > 12.0f)
        {
            const float innerRadius = radius * 0.7f;
            
            g.setColour(slider.findColour(juce::Slider::rotarySliderFillColourId));
            
            juce::Path valueArc;
            valueArc.addPieSegment(rx, ry, rw, rw, rotaryStartAngle, angle, 0.0f);
            g.fillPath(valueArc);
            
            // Draw pointer
            juce::Path p;
            const float pointerLength = radius * 0.33f;
            const float pointerThickness = 4.0f;
            
            p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
            p.applyTransform(juce::AffineTransform::rotation(angle).translated(centerX, centerY));
            
            g.setColour(juce::Colours::white);
            g.fillPath(p);
            
            // Draw center dot
            g.setColour(juce::Colours::white.withAlpha(0.5f));
            g.fillEllipse(centerX - 4.0f, centerY - 4.0f, 8.0f, 8.0f);
        }
    }
};

//==============================================================================
/**
*/
class VocalTransformerAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    VocalTransformerAudioProcessorEditor (VocalTransformerAudioProcessor&, juce::AudioProcessorValueTreeState& vts);
    ~VocalTransformerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::Colour pitchColour;
    juce::Colour formantColour;
    juce::Colour voicesColour;
    juce::Colour detuneColour;
    juce::Colour reverbColour;
    juce::Colour distortionColour;
    juce::Colour lowCutColour;
    juce::Colour toneColour;
    juce::Colour strengthColour;
    
    // Reference to the processor
    VocalTransformerAudioProcessor& audioProcessor;
    
    // Reference to the value tree state
    juce::AudioProcessorValueTreeState& valueTreeState;
    
    // Custom look and feel
    ModernLookAndFeel modernLookAndFeel;
    
    // GUI Components
    juce::ComboBox characterSelector;
    juce::Slider characterStrengthSlider;
    juce::Slider pitchShiftSlider;
    juce::Slider formantShiftSlider;
    juce::Slider voiceCountSlider;
    juce::Slider detuneSlider;
    juce::Slider reverbSlider;
    juce::Slider distortionSlider;
    juce::Slider lowCutSlider;
    juce::Slider toneSlider;
    
    // Labels
    juce::Label characterLabel;
    juce::Label characterStrengthLabel;
    juce::Label pitchShiftLabel;
    juce::Label formantShiftLabel;
    juce::Label voiceCountLabel;
    juce::Label detuneLabel;
    juce::Label reverbLabel;
    juce::Label distortionLabel;
    juce::Label lowCutLabel;
    juce::Label toneLabel;
    
    // Parameter attachments - these connect our GUI controls to parameters
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> characterAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> characterStrengthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> pitchShiftAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> formantShiftAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> voiceCountAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> detuneAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> reverbAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distortionAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowCutAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> toneAttachment;
    
    // Character icons for visualization
    std::map<int, juce::Path> characterIcons;
    int currentCharacter;
    
    // Custom colors
    juce::Colour backgroundColour;
    juce::Colour controlBackgroundColour;
    juce::Colour textColour;
    juce::Colour accentColour;
    
    // Helper methods
    void setupColors();
    void setupSliders();
    void setupLabels();
    void setupCharacterSelector();
    void createCharacterIcons();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VocalTransformerAudioProcessorEditor)
};
