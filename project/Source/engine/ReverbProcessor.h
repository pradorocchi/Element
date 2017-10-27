
#pragma once

#include "engine/BaseProcessor.h"

namespace Element {

    class ReverbProcessor : public BaseProcessor
    {
        AudioParameterFloat* roomSize;
        AudioParameterFloat* damping;
        AudioParameterFloat* wetLevel;
        AudioParameterFloat* dryLevel;
        AudioParameterFloat* width;
        
    public:
        explicit ReverbProcessor()
            : BaseProcessor()
        {
            setPlayConfigDetails (2, 2, 44100.0, 1024);
            addParameter (roomSize = new AudioParameterFloat ("roomSize", "Room Size", 0.0f, 1.0f, params.roomSize));
            addParameter (damping  = new AudioParameterFloat ("damping",  "Damping",   0.0f, 1.0f, params.damping));
            addParameter (wetLevel = new AudioParameterFloat ("wetLevel", "Wet Level", 0.0f, 1.0f, params.wetLevel));
            addParameter (dryLevel = new AudioParameterFloat ("dryLevel", "Dry Level", 0.0f, 1.0f, params.dryLevel));
            addParameter (width    = new AudioParameterFloat ("width",    "Width",     0.0f, 1.0f, params.width));
        }

        virtual ~ReverbProcessor() { }

        const String getName() const override { return "eVerb"; }
        
        void fillInPluginDescription (PluginDescription& desc) const override
        {
            desc.name = getName();
            desc.fileOrIdentifier   = "element.reverb";
            desc.descriptiveName    = "Simple Reverb";
            desc.numInputChannels   = 2;
            desc.numOutputChannels  = 2;
            desc.hasSharedContainer = false;
            desc.isInstrument       = false;
            desc.manufacturerName   = "Element";
            desc.pluginFormatName   = "Element";
            desc.version            = "1.0.0";
        }
        
        void prepareToPlay (double sampleRate, int maxBlockSize) override
        {
            setPlayConfigDetails (2, 2, sampleRate, maxBlockSize);
            verb.setSampleRate (sampleRate);
        }
        
        void releaseResources() override { }
        void processBlock (AudioBuffer<float>& buffer, MidiBuffer&) override
        {
            params.roomSize = (float) *roomSize;
            params.damping  = (float) *damping;
            params.wetLevel = (float) *wetLevel;
            params.dryLevel = (float) *dryLevel;
            params.width    = (float) *width;
            
            if (paramsChanged())
                verb.setParameters (params);
            
            verb.processStereo (buffer.getWritePointer (0),
                                buffer.getWritePointer (1),
                                buffer.getNumSamples());
            
            lastParams.roomSize = params.roomSize;
            lastParams.damping  = params.damping;
            lastParams.wetLevel = params.wetLevel;
            lastParams.dryLevel = params.dryLevel;
            lastParams.width    = params.width;
        }
        
        AudioProcessorEditor* createEditor() override   { return new GenericAudioProcessorEditor (this); }
        bool hasEditor() const override                 { return true; }
        
        double getTailLengthSeconds() const override    { return 0.0; };
        bool acceptsMidi() const override               { return false; }
        bool producesMidi() const override              { return false; }
        
        int getNumPrograms() override                                      { return 1; };
        int getCurrentProgram() override                                   { return 1; };
        void setCurrentProgram (int index) override                        { ignoreUnused (index); };
        const String getProgramName (int index) override                   { ignoreUnused (index); return "Default"; }
        void changeProgramName (int index, const String& newName) override { ignoreUnused (index, newName); }
        
        void getStateInformation (juce::MemoryBlock& destData) override
        {
            ValueTree state (Tags::state);
            state.setProperty ("roomSize", (float)*roomSize, nullptr);
            state.setProperty ("damping",  (float)*damping,  nullptr);
            state.setProperty ("wetLevel", (float)*wetLevel, nullptr);
            state.setProperty ("dryLevel", (float)*dryLevel, nullptr);
            state.setProperty ("width",    (float)*width,    nullptr);
            if (ScopedPointer<XmlElement> e = state.createXml())
                AudioProcessor::copyXmlToBinary (*e, destData);
        }
        
        void setStateInformation (const void* data, int sizeInBytes) override
        {
            if (ScopedPointer<XmlElement> e = AudioProcessor::getXmlFromBinary (data, sizeInBytes))
            {
                auto state = ValueTree::fromXml (*e);
                if (state.isValid())
                {
                    *roomSize = params.roomSize = ((float) state.getProperty ("roomSize", 0.0));
                    *damping  = params.damping  = ((float) state.getProperty ("damping", 0.0));
                    *wetLevel = params.wetLevel = ((float) state.getProperty ("wetLevel", 0.0));
                    *dryLevel = params.dryLevel = ((float) state.getProperty ("dryLevel", 0.0));
                    *width    = params.width    = ((float) state.getProperty ("width", 0.0));
                }
            }
        }
        
    private:
        Reverb verb;
        Reverb::Parameters params, lastParams;
        
        bool paramsChanged() const noexcept
        {
            return params.roomSize != (float) *roomSize ||
            params.damping  != (float) *damping ||
            params.wetLevel != (float) *wetLevel ||
            params.dryLevel != (float) *dryLevel ||
            params.width    != (float) *width;
        }
    };
}
