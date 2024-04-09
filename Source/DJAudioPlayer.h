/*
  ==============================================================================

    DJAudioPlayer.h
    Created: 13 Mar 2020 4:22:22pm
    Author:  matthew

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

class DJAudioPlayer : public AudioSource,
                      public ChangeBroadcaster 
{
public:

    DJAudioPlayer(AudioFormatManager& _formatManager);
    ~DJAudioPlayer();

    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void loadURL(URL audioURL);
    void start();
    void stop();

    void setGain(double gain);
    void setSpeed(double ratio);
    void setPosition(double posInSecs);
    void setPositionRelative(double pos);
    void setLowPass(double hertz);
    void setHighPass(double hertz);

    /** Checks if an audio file is loaded */
    bool isAudioLoaded();

    /** Checks if the audio source is playing */
    bool isAudioPlaying();

    /** Checks if the audio source has completed playback */
    bool hasplaybackFinished();

    /** Get the relative position of the playhead */
    double getPositionRelative();

private:
    AudioFormatManager& formatManager;
    std::unique_ptr<AudioFormatReaderSource> readerSource;
    AudioTransportSource transportSource; 

    IIRFilterAudioSource basefilterSource{ &transportSource, false };
    IIRFilterAudioSource treblefilterSource{ &basefilterSource, false };

    ResamplingAudioSource resampleSource{ &treblefilterSource, false, 2 };

    bool audioLoaded = false;
    bool audioPlaying = false;
};




