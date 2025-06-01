#pragma once

#include <JuceHeader.h>

// Define the character presets
enum CharacterType {
    NORMAL = 0,
    ROBOT,
    ALIEN,
    CHILD,
    GIANT,
    ELDER,
    CHOIR,
    NUM_CHARACTERS
};

//==============================================================================
class VocalTransformerAudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    VocalTransformerAudioProcessor();
    ~VocalTransformerAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // Voice transformation parameters
    juce::AudioProcessorValueTreeState parameters;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    // Parameter IDs - used to access our parameters
    static const juce::String CHARACTER_ID;
    static const juce::String PITCH_SHIFT_ID;
    static const juce::String FORMANT_SHIFT_ID;
    static const juce::String VOICE_COUNT_ID;
    static const juce::String DETUNE_ID;
    static const juce::String REVERB_ID;
    static const juce::String CHARACTER_STRENGTH_ID;
    static const juce::String DISTORTION_ID;
    static const juce::String LOW_CUT_ID;
    static const juce::String TONE_ID;
    
    // DSP objects
    juce::dsp::Gain<float> inputGain;
    juce::dsp::Gain<float> outputGain;
    
    // Additional effect values
    float distortionValue = 0.0f;
    float lowCutValue = 20.0f;
    float toneValue = 0.5f;
    
    // Simple distortion processor
    void applyDistortion(juce::AudioBuffer<float>& buffer, float amount);

    // Simple filter processor
    void applyLowCut(juce::AudioBuffer<float>& buffer, float frequency);

    // Simple tone control
    void applyToneControl(juce::AudioBuffer<float>& buffer, float toneAmount);
    
    // Placeholder for pitch shifter - in a real implementation this would be more complex
    class SimpleShifter {
    public:
        void prepare(const juce::dsp::ProcessSpec& spec) {
            sampleRate = spec.sampleRate;
            reset();
        }
        
        void reset() {
            phase = 0.0f;
            phaseIncrement = 1.0f;
        }
        
        void setPitchRatio(float newRatio) {
            pitchRatio = newRatio;
            phaseIncrement = pitchRatio;
        }
        
        void processBlock(juce::AudioBuffer<float>& buffer) {
            // This is a very simplified pitch-shifting simulation
            // A real implementation would use more sophisticated techniques
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
                auto* channelData = buffer.getWritePointer(channel);
                
                for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
                    // Apply a very basic effect based on the pitch ratio
                    channelData[sample] *= (0.8f + 0.2f * std::sin(phase));
                    phase += phaseIncrement * 0.1f;
                    
                    // Keep phase in a reasonable range
                    if (phase > 1000.0f)
                        phase -= 1000.0f;
                }
            }
        }
        
    private:
        float sampleRate = 44100.0f;
        float phase = 0.0f;
        float phaseIncrement = 1.0f;
        float pitchRatio = 1.0f;
    };
    
    // Simple formant shifter placeholder
    class SimpleFormantShifter {
    public:
        void prepare(const juce::dsp::ProcessSpec& spec) {
            // Set up filters here in a real implementation
        }
        
        void reset() {
            // Reset filter states
        }
        
        void setFormantShift(float newShift) {
            formantShift = newShift;
        }
        
        void processBlock(juce::AudioBuffer<float>& buffer) {
            // This is a very simplified formant shifting simulation
            // A real implementation would use filter banks or spectral processing
            float filterFreq = 500.0f + 1000.0f * formantShift;
            
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
                auto* channelData = buffer.getWritePointer(channel);
                
                for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
                    // Just apply a simple effect based on formant shift
                    if (formantShift > 0.5f) {
                        // Brighten for higher formants
                        channelData[sample] *= (1.0f + 0.2f * (formantShift - 0.5f));
                    } else if (formantShift < 0.5f) {
                        // Darken for lower formants
                        channelData[sample] *= (0.8f + 0.4f * formantShift);
                    }
                }
            }
        }
        
    private:
        float formantShift = 0.5f; // 0 = low, 0.5 = neutral, 1.0 = high
    };
    
    // Voice multiplier placeholder
    class SimpleVoiceMultiplier {
    public:
        void prepare(const juce::dsp::ProcessSpec& spec) {
            sampleRate = spec.sampleRate;
            buffer.resize((int)(sampleRate * 0.05)); // 50ms max delay
            reset();
        }
        
        void reset() {
            std::fill(buffer.begin(), buffer.end(), 0.0f);
            writePos = 0;
        }
        
        void setVoiceCount(int newCount) {
            voiceCount = juce::jlimit(1, 4, newCount);
        }
        
        void setDetune(float newDetune) {
            detune = newDetune;
        }
        
        void processBlock(juce::AudioBuffer<float>& audioBuffer) {
            // Simple delay-based voice multiplication
            if (voiceCount <= 1)
                return;
                
            int numSamples = audioBuffer.getNumSamples();
            int numChannels = audioBuffer.getNumChannels();
            
            for (int channel = 0; channel < numChannels; ++channel) {
                auto* channelData = audioBuffer.getWritePointer(channel);
                
                // Store current samples
                for (int i = 0; i < numSamples; ++i) {
                    buffer[writePos] = channelData[i];
                    writePos = (writePos + 1) % buffer.size();
                }
                
                // Add delayed voices
                for (int voice = 1; voice < voiceCount; ++voice) {
                    int delaySamples = (int)(10.0f * detune * voice);
                    float gain = 0.7f / voiceCount;
                    
                    for (int i = 0; i < numSamples; ++i) {
                        int readPos = (writePos - delaySamples - i + buffer.size()) % buffer.size();
                        channelData[i] += buffer[readPos] * gain;
                    }
                }
            }
        }
        
    private:
        float sampleRate = 44100.0f;
        std::vector<float> buffer;
        int writePos = 0;
        int voiceCount = 1;
        float detune = 0.0f;
    };
    
    SimpleShifter pitchShifter;
    SimpleFormantShifter formantShifter;
    SimpleVoiceMultiplier voiceMultiplier;
    juce::Reverb reverb;
    
    // Character preset values
    struct CharacterPreset {
        float pitchShift;
        float formantShift;
        int voiceCount;
        float detune;
        float reverb;
    };
    
    std::array<CharacterPreset, NUM_CHARACTERS> characterPresets;
    void initializeCharacterPresets();
    void applyCharacterPreset(int characterIndex, float strength);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VocalTransformerAudioProcessor)
};
