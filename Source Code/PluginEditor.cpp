#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VocalTransformerAudioProcessorEditor::VocalTransformerAudioProcessorEditor (VocalTransformerAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), audioProcessor (p), valueTreeState (vts), currentCharacter(0)
{
    // Set custom look and feel
    setLookAndFeel(&modernLookAndFeel);
    
    // Setup UI elements
    setupColors();
    setupSliders();
    setupLabels();
    setupCharacterSelector();
    createCharacterIcons();
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (650, 600);
}

VocalTransformerAudioProcessorEditor::~VocalTransformerAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

void VocalTransformerAudioProcessorEditor::setupColors()
{
    backgroundColour = juce::Colour(25, 25, 30);
    controlBackgroundColour = juce::Colour(40, 40, 45);
    textColour = juce::Colour(230, 230, 240);
    accentColour = juce::Colour(70, 130, 200);
    
    // Slider-specific colors
    pitchColour = juce::Colour(70, 130, 200);       // Blue
    formantColour = juce::Colour(180, 100, 220);    // Purple
    voicesColour = juce::Colour(70, 180, 210);      // Cyan
    detuneColour = juce::Colour(220, 120, 70);      // Orange
    reverbColour = juce::Colour(90, 140, 240);      // Blue-purple
    distortionColour = juce::Colour(220, 80, 80);   // Red
    lowCutColour = juce::Colour(220, 180, 70);      // Gold
    toneColour = juce::Colour(70, 190, 120);        // Green
    strengthColour = juce::Colour(255, 255, 255);   // White
}

void VocalTransformerAudioProcessorEditor::setupSliders()
{
    // Common setup for all sliders
    auto setupRotarySlider = [this](juce::Slider& slider)
    {
        slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 25);
        slider.setColour(juce::Slider::textBoxTextColourId, textColour);
        slider.setColour(juce::Slider::textBoxBackgroundColourId, controlBackgroundColour);
        slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        addAndMakeVisible(slider);
    };
    
    // Character strength slider
    setupRotarySlider(characterStrengthSlider);
    characterStrengthAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        valueTreeState, "character_strength", characterStrengthSlider));
    
    // Pitch shift slider
    setupRotarySlider(pitchShiftSlider);
    pitchShiftAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        valueTreeState, "pitch_shift", pitchShiftSlider));
    
    // Formant shift slider
    setupRotarySlider(formantShiftSlider);
    formantShiftAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        valueTreeState, "formant_shift", formantShiftSlider));
    
    // Voice count slider
    setupRotarySlider(voiceCountSlider);
    voiceCountSlider.setRange(1, 4, 1); // Integer steps
    voiceCountAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        valueTreeState, "voice_count", voiceCountSlider));
    
    // Detune slider
    setupRotarySlider(detuneSlider);
    detuneAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        valueTreeState, "detune", detuneSlider));
    
    // Reverb slider
    setupRotarySlider(reverbSlider);
    reverbAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        valueTreeState, "reverb", reverbSlider));
    
    // Distortion slider
    setupRotarySlider(distortionSlider);
    distortionAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        valueTreeState, "distortion", distortionSlider));
    
    // Low Cut slider
    setupRotarySlider(lowCutSlider);
    lowCutAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        valueTreeState, "low_cut", lowCutSlider));
    
    // Tone slider
    setupRotarySlider(toneSlider);
    toneAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(
        valueTreeState, "tone", toneSlider));
    
    pitchShiftSlider.setColour(juce::Slider::rotarySliderFillColourId, pitchColour);
    formantShiftSlider.setColour(juce::Slider::rotarySliderFillColourId, formantColour);
    voiceCountSlider.setColour(juce::Slider::rotarySliderFillColourId, voicesColour);
    detuneSlider.setColour(juce::Slider::rotarySliderFillColourId, detuneColour);
    reverbSlider.setColour(juce::Slider::rotarySliderFillColourId, reverbColour);
    distortionSlider.setColour(juce::Slider::rotarySliderFillColourId, distortionColour);
    lowCutSlider.setColour(juce::Slider::rotarySliderFillColourId, lowCutColour);
    toneSlider.setColour(juce::Slider::rotarySliderFillColourId, toneColour);
    characterStrengthSlider.setColour(juce::Slider::rotarySliderFillColourId, strengthColour);
}

void VocalTransformerAudioProcessorEditor::setupLabels()
{
    // Common setup for all labels
    auto setupLabel = [this](juce::Label& label, const juce::String& text)
    {
        label.setText(text, juce::dontSendNotification);
        label.setFont(juce::Font(16.0f));
        label.setJustificationType(juce::Justification::centred);
        label.setColour(juce::Label::textColourId, textColour);
        addAndMakeVisible(label);
    };
    
    // Setup individual labels
    setupLabel(characterLabel, "Character");
    setupLabel(characterStrengthLabel, "Strength");
    setupLabel(pitchShiftLabel, "Pitch");
    setupLabel(formantShiftLabel, "Formant");
    setupLabel(voiceCountLabel, "Voices");
    setupLabel(detuneLabel, "Detune");
    setupLabel(reverbLabel, "Reverb");
    setupLabel(distortionLabel, "Distortion");
    setupLabel(lowCutLabel, "Low Cut");
    setupLabel(toneLabel, "Tone");
    
    pitchShiftLabel.setColour(juce::Label::textColourId, pitchColour);
    formantShiftLabel.setColour(juce::Label::textColourId, formantColour);
    voiceCountLabel.setColour(juce::Label::textColourId, voicesColour);
    detuneLabel.setColour(juce::Label::textColourId, detuneColour);
    reverbLabel.setColour(juce::Label::textColourId, reverbColour);
    distortionLabel.setColour(juce::Label::textColourId, distortionColour);
    lowCutLabel.setColour(juce::Label::textColourId, lowCutColour);
    toneLabel.setColour(juce::Label::textColourId, toneColour);
    characterStrengthLabel.setColour(juce::Label::textColourId, strengthColour);
}

void VocalTransformerAudioProcessorEditor::setupCharacterSelector()
{
    // Setup character selector
    characterSelector.addItem("Normal", 1);
    characterSelector.addItem("Robot", 2);
    characterSelector.addItem("Alien", 3);
    characterSelector.addItem("Child", 4);
    characterSelector.addItem("Giant", 5);
    characterSelector.addItem("Elder", 6);
    characterSelector.addItem("Choir", 7);
    
    characterSelector.setColour(juce::ComboBox::backgroundColourId, controlBackgroundColour);
    characterSelector.setColour(juce::ComboBox::textColourId, textColour);
    characterSelector.setColour(juce::ComboBox::arrowColourId, accentColour);
    characterSelector.setColour(juce::ComboBox::outlineColourId, juce::Colours::transparentWhite);
    
    addAndMakeVisible(characterSelector);
    
    // Connect to parameter
    characterAttachment.reset(new juce::AudioProcessorValueTreeState::ComboBoxAttachment(
        valueTreeState, "character", characterSelector));
    
    // Add listener to update character image
    characterSelector.onChange = [this]
    {
        currentCharacter = characterSelector.getSelectedItemIndex();
        repaint();
    };
}

void VocalTransformerAudioProcessorEditor::createCharacterIcons()
{
    // Create simple path icons for each character type
    
    // Normal
    juce::Path normalPath;
    normalPath.addEllipse(0, 0, 100, 100);
    normalPath.addEllipse(25, 30, 15, 15);
    normalPath.addEllipse(60, 30, 15, 15);
    normalPath.startNewSubPath(30, 65);
    normalPath.quadraticTo(50, 85, 70, 65);
    characterIcons[0] = normalPath;
    
    // Robot
    juce::Path robotPath;
    robotPath.addRectangle(10, 10, 80, 80);
    robotPath.addRectangle(25, 30, 15, 15);
    robotPath.addRectangle(60, 30, 15, 15);
    robotPath.addRectangle(30, 65, 40, 10);
    characterIcons[1] = robotPath;
    
    // Alien
    juce::Path alienPath;
    alienPath.addEllipse(20, 0, 60, 80);
    alienPath.addEllipse(30, 30, 10, 20);
    alienPath.addEllipse(60, 30, 10, 20);
    alienPath.startNewSubPath(40, 65);
    alienPath.quadraticTo(50, 75, 60, 65);
    characterIcons[2] = alienPath;
    
    // Child
    juce::Path childPath;
    childPath.addEllipse(20, 10, 60, 60);
    childPath.addEllipse(35, 30, 10, 10);
    childPath.addEllipse(55, 30, 10, 10);
    childPath.startNewSubPath(40, 50);
    childPath.quadraticTo(50, 65, 60, 50);
    characterIcons[3] = childPath;
    
    // Giant
    juce::Path giantPath;
    giantPath.addRectangle(10, 0, 80, 100);
    giantPath.addRectangle(25, 20, 20, 10);
    giantPath.addRectangle(55, 20, 20, 10);
    giantPath.startNewSubPath(30, 65);
    giantPath.lineTo(70, 65);
    characterIcons[4] = giantPath;
    
    // Elder
    juce::Path elderPath;
    elderPath.addEllipse(20, 10, 60, 80);
    elderPath.addEllipse(35, 30, 8, 5);
    elderPath.addEllipse(55, 30, 8, 5);
    elderPath.startNewSubPath(35, 70);
    elderPath.quadraticTo(50, 60, 65, 70);
    characterIcons[5] = elderPath;
    
    // Choir
    juce::Path choirPath;
    // Main shape - group of figures
    choirPath.addEllipse(10, 20, 30, 40);
    choirPath.addEllipse(35, 10, 30, 40);
    choirPath.addEllipse(60, 20, 30, 40);
    characterIcons[6] = choirPath;
    
    // Center each path around the origin
    for (auto& pair : characterIcons) {
        auto bounds = pair.second.getBounds();
        pair.second.applyTransform(juce::AffineTransform::translation(-bounds.getCentreX(), -bounds.getCentreY()));
    }
}

//==============================================================================
void VocalTransformerAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Fill the background
    g.fillAll(backgroundColour);
    
    // Draw the plugin name
    g.setFont(juce::Font(24.0f, juce::Font::bold));
    g.setColour(textColour);
    g.drawText("Vocal Transformer", 20, 15, getWidth() - 40, 30, juce::Justification::centred);
    
    // Draw character icon if available
    if (characterIcons.find(currentCharacter) != characterIcons.end()) {
        
        switch (currentCharacter) {
                case 0: g.setColour(strengthColour); break;      // Normal - white
                case 1: g.setColour(pitchColour); break;         // Robot - blue
                case 2: g.setColour(formantColour); break;       // Alien - purple
                case 3: g.setColour(voicesColour); break;        // Child - cyan
                case 4: g.setColour(detuneColour); break;        // Giant - orange
                case 5: g.setColour(distortionColour); break;    // Elder - red
                case 6: g.setColour(reverbColour); break;        // Choir - blue-purple
                default: g.setColour(accentColour);
            }
        
        // Get character icon and scale it to fit
        auto path = characterIcons[currentCharacter];
        float iconSize = 60.0f;
        
        // Calculate icon position
        float xPos = (getWidth() - iconSize) / 2;
        float yPos = 60;
        
        // Scale and translate the path to the desired location and size
        auto bounds = path.getBounds();
        float scale = iconSize / juce::jmax(bounds.getWidth(), bounds.getHeight());
        
        path.applyTransform(juce::AffineTransform::scale(scale)
                           .translated(xPos + iconSize/2, yPos + iconSize/2));
        
        // Draw the icon
        g.fillPath(path);
    }
    
    // Draw divider line
    g.setColour(accentColour.withAlpha(0.5f));
    g.drawLine(20, 130, getWidth() - 20, 130, 1.0f);
}

void VocalTransformerAudioProcessorEditor::resized()
{
    // Character selector (top)
    characterSelector.setBounds((getWidth() - 250) / 2, 140, 250, 30);
    
    // Character strength slider
    characterStrengthSlider.setBounds((getWidth() - 100) / 2, 180, 100, 100);
    characterStrengthLabel.setBounds((getWidth() - 100) / 2, 270, 100, 25);
    
    // Calculate layout for bottom rows of sliders
    int sliderWidth = 90;
    int sliderHeight = 90;
    int labelHeight = 25;
    int sliderSpacing = 20;
    
    // Position original parameters (first row)
    int row1Y = 300;
    int row1Width = (sliderWidth + sliderSpacing) * 5 - sliderSpacing;
    int row1StartX = (getWidth() - row1Width) / 2;
    
    pitchShiftSlider.setBounds(row1StartX, row1Y, sliderWidth, sliderHeight);
    formantShiftSlider.setBounds(row1StartX + sliderWidth + sliderSpacing, row1Y, sliderWidth, sliderHeight);
    voiceCountSlider.setBounds(row1StartX + (sliderWidth + sliderSpacing) * 2, row1Y, sliderWidth, sliderHeight);
    detuneSlider.setBounds(row1StartX + (sliderWidth + sliderSpacing) * 3, row1Y, sliderWidth, sliderHeight);
    reverbSlider.setBounds(row1StartX + (sliderWidth + sliderSpacing) * 4, row1Y, sliderWidth, sliderHeight);
    
    // Position original parameter labels
    pitchShiftLabel.setBounds(row1StartX, row1Y + sliderHeight, sliderWidth, labelHeight);
    formantShiftLabel.setBounds(row1StartX + sliderWidth + sliderSpacing, row1Y + sliderHeight, sliderWidth, labelHeight);
    voiceCountLabel.setBounds(row1StartX + (sliderWidth + sliderSpacing) * 2, row1Y + sliderHeight, sliderWidth, labelHeight);
    detuneLabel.setBounds(row1StartX + (sliderWidth + sliderSpacing) * 3, row1Y + sliderHeight, sliderWidth, labelHeight);
    reverbLabel.setBounds(row1StartX + (sliderWidth + sliderSpacing) * 4, row1Y + sliderHeight, sliderWidth, labelHeight);
    
    // Position new parameters (second row)
    int row2Y = row1Y + sliderHeight + labelHeight + 40; // Position below first row
    int row2Width = (sliderWidth + sliderSpacing) * 3 - sliderSpacing;
    int row2StartX = (getWidth() - row2Width) / 2;
    
    distortionSlider.setBounds(row2StartX, row2Y, sliderWidth, sliderHeight);
    lowCutSlider.setBounds(row2StartX + sliderWidth + sliderSpacing, row2Y, sliderWidth, sliderHeight);
    toneSlider.setBounds(row2StartX + (sliderWidth + sliderSpacing) * 2, row2Y, sliderWidth, sliderHeight);
    
    // Position new parameter labels
    distortionLabel.setBounds(row2StartX, row2Y + sliderHeight, sliderWidth, labelHeight);
    lowCutLabel.setBounds(row2StartX + sliderWidth + sliderSpacing, row2Y + sliderHeight, sliderWidth, labelHeight);
    toneLabel.setBounds(row2StartX + (sliderWidth + sliderSpacing) * 2, row2Y + sliderHeight, sliderWidth, labelHeight);
}
