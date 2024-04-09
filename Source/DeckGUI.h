/*
  ==============================================================================

    DeckGUI.h
    Created: 13 Mar 2020 6:44:48pm
    Author:  matthew

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DJAudioPlayer.h"
#include "WaveformDisplay.h"
#include <vector>

//==============================================================================

// Style classes
class RotaryStyle : public LookAndFeel_V4
{
public:
    RotaryStyle(const Colour& _colorTheme);

    void drawRotarySlider(Graphics& g,
        int x, int y, int width, int height,
        float sliderPosProportional,
        float rotaryStartAngle,
        float rotaryEndAngle,
        Slider&) override;
private:
    Colour colorTheme;
};

class LinearStyle : public LookAndFeel_V4
{
public:
    LinearStyle(const Colour& _colorTheme);

    void drawLinearSlider(juce::Graphics& g,
        int x, int y, int width, int height,
        float sliderPos,
        float minSliderPos,
        float maxSliderPos,
        const Slider::SliderStyle style,
        Slider& slider) override;
private:
    Colour colorTheme;
};

// Main class
class DeckGUI    : public Component,
                   public Button::Listener, 
                   public Slider::Listener, 
                   public FileDragAndDropTarget, 
                   public Timer
{
public:
    DeckGUI(DJAudioPlayer* player, 
           AudioFormatManager & 	formatManagerToUse,
           AudioThumbnailCache & 	cacheToUse,
           const Colour& _colorTheme);
    ~DeckGUI();

    void paint (Graphics&) override;
    void resized() override;

    /** implement Button::Listener */
    void buttonClicked (Button *) override;

    /** implement Slider::Listener */
    void sliderValueChanged (Slider *slider) override;

    void updateLoopButton();

    bool isInterestedInFileDrag (const StringArray &files) override;
    void filesDropped (const StringArray &files, int x, int y) override;
    void loadURL(URL audioURL, const String& title);

    void timerCallback() override; 

private:
    Colour colorTheme;
    Image playImage;
    Image pauseImage;
    ImageComponent playImageComponent;
    ImageComponent pauseImageComponent;
    TextButton playPauseButton;
    TextButton loopButton{"LOOP"};

    TextButton rockButton{"Rock"};
    TextButton jazzButton{"Jazz"};
    TextButton hiphopButton{"Hip-Hop"};
    TextButton classicalButton{"Classical"};

    String songTitle;
    Label titleLabel;
  
    Slider volSlider;
    Slider speedSlider;
    Slider posSlider;

    Slider lowSlider;
    Slider highSlider;
    Label lowLabel;
    Label highLabel;

    FileChooser fChooser{"Select a file..."};

    WaveformDisplay waveformDisplay;
    DJAudioPlayer* player;

    bool isLoop = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeckGUI)
};
