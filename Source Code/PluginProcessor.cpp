#include "PluginProcessor.h"
#include "PluginEditor.h"

// Define parameter IDs
const juce::String VocalTransformerAudioProcessor::CHARACTER_ID = "character";
const juce::String VocalTransformerAudioProcessor::PITCH_SHIFT_ID = "pitch_shift";
const juce::String VocalTransformerAudioProcessor::FORMANT_SHIFT_ID = "formant_shift";
const juce::String VocalTransformerAudioProcessor::VOICE_COUNT_ID = "voice_count";
const juce::String VocalTransformerAudioProcessor::DETUNE_ID = "detune";
const juce::String VocalTransformerAudioProcessor::REVERB_ID = "reverb";
const juce::String VocalTransformerAudioProcessor::CHARACTER_STRENGTH_ID = "character_strength";
const juce::String VocalTransformerAudioProcessor::DISTORTION_ID = "distortion";
const juce::String VocalTransformerAudioProcessor::LOW_CUT_ID = "low_cut";
const juce::String VocalTransformerAudioProcessor::TONE_ID = "tone";

//==============================================================================
VocalTransformerAudioProcessor::VocalTransformerAudioProcessor()
     : AudioProcessor (BusesProperties()
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                       ),
       parameters(*this, nullptr, "PARAMETERS", createParameterLayout())
{
    initializeCharacterPresets();
}

VocalTransformerAudioProcessor::~VocalTransformerAudioProcessor()
{
}

void VocalTransformerAudioProcessor::initializeCharacterPresets()
{
    // Normal voice
    characterPresets[NORMAL] = { 0.0f, 0.5f, 1, 0.0f, 0.2f };
    
    // Robot voice
    characterPresets[ROBOT] = { -2.0f, 0.5f, 1, 0.0f, 0.1f };
    
    // Alien voice
    characterPresets[ALIEN] = { 3.0f, 0.7f, 2, 0.7f, 0.6f };
    
    // Child voice
    characterPresets[CHILD] = { 4.0f, 0.8f, 1, 0.2f, 0.3f };
    
    // Giant voice
    characterPresets[GIANT] = { -6.0f, 0.2f, 1, 0.0f, 0.5f };
    
    // Elder voice
    characterPresets[ELDER] = { -1.0f, 0.3f, 1, 0.3f, 0.4f };
    
    // Choir voice
    characterPresets[CHOIR] = { 0.0f, 0.5f, 4, 0.4f, 0.8f };
}

void VocalTransformerAudioProcessor::applyCharacterPreset(int characterIndex, float strength)
{
    if (characterIndex < 0 || characterIndex >= NUM_CHARACTERS)
        return;
        
    auto& preset = characterPresets[characterIndex];
    
    // These calls get the parameter objects from the value tree state
    auto* pitchParam = dynamic_cast<juce::AudioParameterFloat*>(parameters.getParameter(PITCH_SHIFT_ID));
    auto* formantParam = dynamic_cast<juce::AudioParameterFloat*>(parameters.getParameter(FORMANT_SHIFT_ID));
    auto* voiceCountParam = dynamic_cast<juce::AudioParameterInt*>(parameters.getParameter(VOICE_COUNT_ID));
    auto* detuneParam = dynamic_cast<juce::AudioParameterFloat*>(parameters.getParameter(DETUNE_ID));
    auto* reverbParam = dynamic_cast<juce::AudioParameterFloat*>(parameters.getParameter(REVERB_ID));
    
    // Apply the preset values with strength factor
    if (pitchParam)
        *pitchParam = pitchParam->getNormalisableRange().convertFrom0to1(
            juce::jmap(strength, 0.0f, 1.0f,
                      pitchParam->getNormalisableRange().convertTo0to1(pitchParam->get()),
                      pitchParam->getNormalisableRange().convertTo0to1(preset.pitchShift)));
                      
    if (formantParam)
        *formantParam = formantParam->getNormalisableRange().convertFrom0to1(
            juce::jmap(strength, 0.0f, 1.0f,
                      formantParam->getNormalisableRange().convertTo0to1(formantParam->get()),
                      formantParam->getNormalisableRange().convertTo0to1(preset.formantShift)));
                      
    if (voiceCountParam)
        *voiceCountParam = (int)juce::jmap(strength, 0.0f, 1.0f,
                                     (float)voiceCountParam->get(),
                                     (float)preset.voiceCount);
                                     
    if (detuneParam)
        *detuneParam = detuneParam->getNormalisableRange().convertFrom0to1(
            juce::jmap(strength, 0.0f, 1.0f,
                      detuneParam->getNormalisableRange().convertTo0to1(detuneParam->get()),
                      detuneParam->getNormalisableRange().convertTo0to1(preset.detune)));
                      
    if (reverbParam)
        *reverbParam = reverbParam->getNormalisableRange().convertFrom0to1(
            juce::jmap(strength, 0.0f, 1.0f,
                      reverbParam->getNormalisableRange().convertTo0to1(reverbParam->get()),
                      reverbParam->getNormalisableRange().convertTo0to1(preset.reverb)));
}

juce::AudioProcessorValueTreeState::ParameterLayout VocalTransformerAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    // Character selector
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{CHARACTER_ID, 1},
        "Character",
        juce::StringArray("Normal", "Robot", "Alien", "Child", "Giant", "Elder", "Choir"),
        0)); // Default to Normal
    
    // Character strength
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{CHARACTER_STRENGTH_ID, 1},
        "Character Strength",
        0.0f, 1.0f, 1.0f)); // Default to full strength
    
    // Pitch shift in semitones (-12 to +12)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{PITCH_SHIFT_ID, 1},
        "Pitch Shift",
        -12.0f, 12.0f, 0.0f)); // Default to no shift
    
    // Formant shift (0 = low, 0.5 = normal, 1.0 = high)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{FORMANT_SHIFT_ID, 1},
        "Formant Shift",
        0.0f, 1.0f, 0.5f)); // Default to normal
    
    // Voice count (1 to 4 voices)
    params.push_back(std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID{VOICE_COUNT_ID, 1},
        "Voice Count",
        1, 4, 1)); // Default to 1 voice
    
    // Detune amount (0.0 to 1.0)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{DETUNE_ID, 1},
        "Detune",
        0.0f, 1.0f, 0.0f)); // Default to no detune
    
    // Reverb amount (0.0 to 1.0)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{REVERB_ID, 1},
        "Reverb",
        0.0f, 1.0f, 0.2f)); // Default to slight reverb
    
    // Distortion amount (0.0 to 1.0)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{DISTORTION_ID, 1},
        "Distortion",
        0.0f, 1.0f, 0.0f)); // Default to no distortion
    
    // Low cut frequency (20 to 1000 Hz)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{LOW_CUT_ID, 1},
        "Low Cut",
        20.0f, 1000.0f, 20.0f)); // Default to 20 Hz (basically off)
    
    // Tone control (0.0 to 1.0)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{TONE_ID, 1},
        "Tone",
        0.0f, 1.0f, 0.5f)); // Default to middle
    
    return { params.begin(), params.end() };
}

//==============================================================================
const juce::String VocalTransformerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool VocalTransformerAudioProcessor::acceptsMidi() const
{
    return false;
}

bool VocalTransformerAudioProcessor::producesMidi() const
{
    return false;
}

bool VocalTransformerAudioProcessor::isMidiEffect() const
{
    return false;
}

double VocalTransformerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int VocalTransformerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int VocalTransformerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void VocalTransformerAudioProcessor::setCurrentProgram (int index)
{
    // Unused
}

const juce::String VocalTransformerAudioProcessor::getProgramName (int index)
{
    return {};
}

void VocalTransformerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    // Unused
}

//==============================================================================
void VocalTransformerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Prepare DSP modules
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    
    inputGain.prepare(spec);
    outputGain.prepare(spec);
    pitchShifter.prepare(spec);
    formantShifter.prepare(spec);
    voiceMultiplier.prepare(spec);
    
    // Set up reverb
    reverb.reset();
    reverb.setSampleRate(sampleRate);
    
    // Set default gain
    inputGain.setGainLinear(0.9f);
    outputGain.setGainLinear(1.0f);
}

void VocalTransformerAudioProcessor::releaseResources()
{
    // Release resources when the plugin is not being used
}

bool VocalTransformerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    // Support mono or stereo
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // Input and output must match
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void VocalTransformerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear any output channels that don't contain input data
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Get parameters
    auto* characterParam = parameters.getRawParameterValue(CHARACTER_ID);
    auto* characterStrengthParam = parameters.getRawParameterValue(CHARACTER_STRENGTH_ID);
    auto* pitchShiftParam = parameters.getRawParameterValue(PITCH_SHIFT_ID);
    auto* formantShiftParam = parameters.getRawParameterValue(FORMANT_SHIFT_ID);
    auto* voiceCountParam = parameters.getRawParameterValue(VOICE_COUNT_ID);
    auto* detuneParam = parameters.getRawParameterValue(DETUNE_ID);
    auto* reverbParam = parameters.getRawParameterValue(REVERB_ID);
    auto* distortionParam = parameters.getRawParameterValue(DISTORTION_ID);
    auto* lowCutParam = parameters.getRawParameterValue(LOW_CUT_ID);
    auto* toneParam = parameters.getRawParameterValue(TONE_ID);
    
    distortionValue = distortionParam->load();
    lowCutValue = lowCutParam->load();
    toneValue = toneParam->load();
    
    // Apply character preset if needed
    int currentCharacter = (int)characterParam->load();
    float strength = characterStrengthParam->load();
    
    if (currentCharacter > 0 && strength > 0.0f) {
        applyCharacterPreset(currentCharacter, strength);
    }
    
    // Convert pitch shift from semitones to ratio
    float pitchShiftSemitones = pitchShiftParam->load();
    float pitchRatio = std::pow(2.0f, pitchShiftSemitones / 12.0f);
    
    // Apply processing stages
    
    // 1. Input gain
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    inputGain.process(context);
    
    // 2. Apply low cut filter
    applyLowCut(buffer, lowCutValue);
    
    // 3. Pitch shifting
    pitchShifter.setPitchRatio(pitchRatio);
    pitchShifter.processBlock(buffer);
    
    // 4. Formant shifting
    formantShifter.setFormantShift(formantShiftParam->load());
    formantShifter.processBlock(buffer);
    
    // 5. Voice multiplication
    voiceMultiplier.setVoiceCount((int)voiceCountParam->load());
    voiceMultiplier.setDetune(detuneParam->load());
    voiceMultiplier.processBlock(buffer);
    
    // 6. Apply tone control
    applyToneControl(buffer, toneValue);
    
    // 7. Apply distortion effect
    applyDistortion(buffer, distortionValue);
    
    // 8. Apply reverb
    juce::Reverb::Parameters reverbParams;
    reverbParams.roomSize = 0.5f;
    reverbParams.damping = 0.5f;
    reverbParams.wetLevel = reverbParam->load();
    reverbParams.dryLevel = 1.0f - (reverbParam->load() * 0.5f); // Ensure dry signal remains audible
    reverbParams.width = 1.0f;
    reverb.setParameters(reverbParams);
    
    if (buffer.getNumChannels() == 1) {
        reverb.processMono(buffer.getWritePointer(0), buffer.getNumSamples());
    } else {
        reverb.processStereo(buffer.getWritePointer(0), buffer.getWritePointer(1), buffer.getNumSamples());
    }
    
    // 9. Output gain
    outputGain.process(context);
}

void VocalTransformerAudioProcessor::applyDistortion(juce::AudioBuffer<float>& buffer, float amount)
{
    // Skip processing if distortion is set to zero
    if (amount <= 0.001f)
        return;
        
    // Simple distortion algorithm
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            // Soft clipping distortion - more amount means more distortion
            float input = channelData[sample];
            float distorted = std::tanh(input * (1.0f + 20.0f * amount));
            
            // Mix between clean and distorted signal
            channelData[sample] = input * (1.0f - amount) + distorted * amount;
        }
    }
}

void VocalTransformerAudioProcessor::applyLowCut(juce::AudioBuffer<float>& buffer, float frequency)
{
    // Skip processing if low cut is at minimum
    if (frequency <= 21.0f)
        return;
        
    // Very simple high-pass filter (low cut)
    // This is a very basic implementation - a real plugin would use a proper filter
    static float lastSamples[2] = {0.0f, 0.0f}; // For left and right channels
    
    // Convert frequency to a coefficient (very simplified)
    float alpha = 0.01f + 0.5f * (frequency / 1000.0f);
    if (alpha > 0.99f) alpha = 0.99f;
    
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            // Simple high-pass filter
            float filtered = alpha * (lastSamples[channel] + channelData[sample] - lastSamples[channel]);
            lastSamples[channel] = channelData[sample];
            channelData[sample] = filtered;
        }
    }
}

void VocalTransformerAudioProcessor::applyToneControl(juce::AudioBuffer<float>& buffer, float toneAmount)
{
    // Skip processing if tone is in the middle position
    if (toneAmount > 0.49f && toneAmount < 0.51f)
        return;
        
    // Simple tone control - boost high frequencies or low frequencies
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        static float lastSample = 0.0f;
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            // Simple 1-pole filter for tone control
            if (toneAmount > 0.5f) {
                // Boost highs (reduce lows)
                float highAmount = (toneAmount - 0.5f) * 2.0f; // 0 to 1
                float highPass = 0.8f * (channelData[sample] - lastSample);
                channelData[sample] = channelData[sample] * (1.0f - highAmount) + highPass * highAmount;
            }
            else {
                // Boost lows
                float lowAmount = (0.5f - toneAmount) * 2.0f; // 0 to 1
                float lowPass = 0.2f * channelData[sample] + 0.8f * lastSample;
                channelData[sample] = channelData[sample] * (1.0f - lowAmount) + lowPass * lowAmount;
            }
            
            lastSample = channelData[sample];
        }
    }
}

//==============================================================================
bool VocalTransformerAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* VocalTransformerAudioProcessor::createEditor()
{
    return new VocalTransformerAudioProcessorEditor (*this, parameters);
}

//==============================================================================

void VocalTransformerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // Save plugin state
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void VocalTransformerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // Restore plugin state
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VocalTransformerAudioProcessor();
}
