/*
==============================================================================

DJAudioPlayer.cpp
Created: 13 Mar 2020 4:22:22pm
Author:  matthew

==============================================================================
*/

#include "DJAudioPlayer.h"

DJAudioPlayer::DJAudioPlayer(AudioFormatManager& _formatManager) 
: formatManager(_formatManager)
{
    basefilterSource.setCoefficients(IIRCoefficients::makeLowPass(44100.0, 2000.0));
    treblefilterSource.setCoefficients(IIRCoefficients::makeBandPass(44100.0, 2000.0));
}
DJAudioPlayer::~DJAudioPlayer()
{

}

void DJAudioPlayer::prepareToPlay (int samplesPerBlockExpected, double sampleRate) 
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    basefilterSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    treblefilterSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    resampleSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void DJAudioPlayer::getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill)
{
    if (readerSource.get() == nullptr)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }
    resampleSource.getNextAudioBlock(bufferToFill);
}
void DJAudioPlayer::releaseResources()
{
    basefilterSource.releaseResources();
    treblefilterSource.releaseResources();
    resampleSource.releaseResources();
}

void DJAudioPlayer::loadURL(URL audioURL)
{
    auto* reader = formatManager.createReaderFor(audioURL.createInputStream(false));
    if (reader != nullptr) // good file!
    {       
        std::unique_ptr<AudioFormatReaderSource> newSource (new AudioFormatReaderSource (reader, true)); 
        transportSource.setSource (newSource.get(), 0, nullptr, reader->sampleRate);             
        readerSource.reset (newSource.release());       
        audioLoaded = true;
    }
}

void DJAudioPlayer::setGain(double gain)
{
    // Convert gain to a value from 0-100 to 0-1
    float gainValue = gain / 100;
    if (gainValue < 0 || gainValue > 1.0)
    {
        std::cout << "DJAudioPlayer::setGain gain should be between 0 and 1" << std::endl;
    }
    else 
    {
        transportSource.setGain(gainValue);
    }
}

void DJAudioPlayer::setSpeed(double ratio)
{
    if (ratio < 0 || ratio > 10.0)
    {
        std::cout << "DJAudioPlayer::setSpeed ratio should be between 0 and 100" << std::endl;
    }
    else 
    {
        resampleSource.setResamplingRatio(ratio);
    }
}

void DJAudioPlayer::setPosition(double posInSecs)
{
    transportSource.setPosition(posInSecs);
}

void DJAudioPlayer::setPositionRelative(double pos)
{
    if (pos < 0 || pos > 1.0)
    {
        std::cout << "DJAudioPlayer::setPositionRelative pos should be between 0 and 1" << std::endl;
    }
    else {
        double posInSecs = transportSource.getLengthInSeconds() * pos;
        setPosition(posInSecs);
    }
}

void DJAudioPlayer::start()
{
    transportSource.start();
    audioPlaying = true;

}
void DJAudioPlayer::stop()
{
    transportSource.stop();
    audioPlaying = false;
}

double DJAudioPlayer::getPositionRelative()
{
    return transportSource.getCurrentPosition() / transportSource.getLengthInSeconds();
}

// My code
void DJAudioPlayer::setLowPass(double hertz)
{
    // Retrieve audio format reader
    auto* reader = readerSource->getAudioFormatReader();

    // Set low-pass filter coefficient
    basefilterSource.setCoefficients(IIRCoefficients::makeLowPass(reader->sampleRate, hertz));
}

// My code
void DJAudioPlayer::setHighPass(double hertz)
{
    // Retrieve audio format reader
    auto* reader = readerSource->getAudioFormatReader();

    // Set band pass filter coefficient
    treblefilterSource.setCoefficients(IIRCoefficients::makeBandPass(reader->sampleRate, hertz));
}

// My code
bool DJAudioPlayer::isAudioLoaded()
{
    return audioLoaded;
}

// My code
bool DJAudioPlayer::isAudioPlaying()
{
    return audioPlaying;
}

// My code
bool DJAudioPlayer::hasplaybackFinished()
{
    // Check if position is more than the total length
    if (transportSource.getNextReadPosition() >= transportSource.getTotalLength())
    {
        return true;
    }
    else
    {
        return false;
    }
}