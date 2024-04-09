/*
  ==============================================================================

    DeckGUI.cpp
    Created: 13 Mar 2020 6:44:48pm
    Author:  matthew

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "DeckGUI.h"

//==============================================================================
RotaryStyle::RotaryStyle(const Colour& _colorTheme)
    : colorTheme(_colorTheme)
{
}

// My code
void RotaryStyle::drawRotarySlider(Graphics& g,
    int x, int y, int width, int height,
    float sliderPosProportional,
    float rotaryStartAngle,
    float rotaryEndAngle,
    Slider&)
{
    // Measurements
    const float radius = jmin(width / 2, height / 2) - 10.0f;
    const float centreX = x + width * 0.5f;
    const float centreY = y + height * 0.5f;
    const float angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
    const float lineThickness = 1.0f;

    // Color 
    Colour discColor = Colour(0x2b, 0x2b, 0x2b);
    Colour outlineColor = Colour(0x1c, 0x1c, 0x1c);
    const std::vector<float> innerCircleRadius = { 0.15f, 0.32f, 0.4f };
    const std::vector<Colour> innerCircleColors = { Colour(0x40, 0x40, 0x40), 
                                                    colorTheme, 
                                                    Colours::black };

    // Draw the disc
    g.setColour(discColor);
    g.fillEllipse(centreX - radius, centreY - radius, radius * 2, radius * 2);

    // Draw the disc lines
    for (float i = 1.0f; i > 0.0f; i -= 0.2f)
    {
        float circleRadius = i * radius;
        float lineCornerX = centreX - circleRadius;
        float lineCornerY = centreY - circleRadius;
        float lineDiameter = circleRadius * 2.0f;
        g.setColour(outlineColor);
        g.drawEllipse(lineCornerX, lineCornerY, lineDiameter, lineDiameter, lineThickness);
    }

    // Draw the inner circles
    for (int i = 3; i > 0; i--)
    {
        int currentIndex = i - 1;
        float currentRadius = radius * innerCircleRadius[currentIndex];
        g.setColour(innerCircleColors[currentIndex]);
        g.fillEllipse(centreX - currentRadius, centreY - currentRadius,
            currentRadius * 2.0f, currentRadius * 2.0f);
    }

    // Draw the indicator line
    g.setColour(Colours::white);
    g.drawLine(centreX, centreY,
        centreX + radius * std::cos(angle - MathConstants<float>::halfPi),
        centreY + radius * std::sin(angle - MathConstants<float>::halfPi),
        lineThickness);
}

// My code
LinearStyle::LinearStyle(const Colour& _colorTheme)
    : colorTheme(_colorTheme)
{
    setColour(Slider::backgroundColourId, Colour(0x40, 0x40, 0x40));
    setColour(Slider::trackColourId, Colour(0x25, 0x25, 0x25));
    setColour(Slider::thumbColourId, colorTheme);
}

// My code
void LinearStyle::drawLinearSlider(juce::Graphics& g,
    int x, int y, int width, int height,
    float sliderPos,
    float minSliderPos,
    float maxSliderPos,
    const Slider::SliderStyle style,
    Slider& slider)
{
    g.setColour(Colours::white);
    int numLines = 10;
    float regularLineWidth = 14.0f;
    float longerLineWidth = 22.0f;

    if (slider.isHorizontal())
    {
        // Draw vertical lines at specified intervals
        for (int i = 1; i < numLines; ++i)
        {
            float xPos = x + i * width / numLines;
            float centerY = y + height / 2.0f;
            // Every 5 lines, draw a longer line
            float lineWidth = (i % 5 == 0) ? longerLineWidth : regularLineWidth;
            g.drawLine(xPos, centerY - lineWidth / 2.0f, xPos, centerY + lineWidth / 2.0f, 0.8f);
        }
    }
    else
    {
        // Draw horizontal lines ar specifies intervals
        for (int i = 1; i < numLines; ++i)
        {
            float yPos = y + i * height / numLines;
            float centerX = x + width / 2.0f;
            // Every 5 lines, draw a longer line
            float lineWidth = (i % 5 == 0) ? longerLineWidth : regularLineWidth;
            g.drawLine(centerX - lineWidth / 2.0f, yPos, centerX + lineWidth / 2.0f, yPos, 0.8f);
        }
    }
    LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
}

DeckGUI::DeckGUI(DJAudioPlayer* _player,
    AudioFormatManager& formatManagerToUse,
    AudioThumbnailCache& cacheToUse,
    const Colour& _colorTheme
           ) : player(_player), 
               waveformDisplay(formatManagerToUse, cacheToUse, colorTheme),
               colorTheme(_colorTheme)
{
    // Get directory where application is located
    File appDir = File::getSpecialLocation(File::SpecialLocationType::currentApplicationFile).getParentDirectory();
    DBG("Application Directory: " << appDir.getFullPathName());

    // Load images
    playImage = ImageFileFormat::loadFrom(appDir.getChildFile("play.png"));
    pauseImage = ImageFileFormat::loadFrom(appDir.getChildFile("pause.png"));

    // Check if images loaded successfully
    if (playImage.isValid() && pauseImage.isValid())
    {
        playImageComponent.setImage(playImage);
        playImageComponent.setInterceptsMouseClicks(false, false);
        pauseImageComponent.setImage(pauseImage);
        pauseImageComponent.setInterceptsMouseClicks(false, false);
    }
    else
    {
        DBG("Error loading images!");
    }

    // Add visibile for objects
    addAndMakeVisible(playPauseButton);
    addAndMakeVisible(rockButton);
    addAndMakeVisible(jazzButton);
    addAndMakeVisible(hiphopButton);
    addAndMakeVisible(classicalButton);
    addAndMakeVisible(titleLabel);
    addAndMakeVisible(volSlider);
    addAndMakeVisible(speedSlider);
    addAndMakeVisible(posSlider);
    addAndMakeVisible(lowSlider);
    addAndMakeVisible(highSlider);
    addAndMakeVisible(lowLabel);
    addAndMakeVisible(highLabel);
    addAndMakeVisible(waveformDisplay);
    addAndMakeVisible(playImageComponent);
    addAndMakeVisible(pauseImageComponent);
    addAndMakeVisible(loopButton);

    // Set visibility for play and pause images
    playImageComponent.setVisible(true);
    pauseImageComponent.setVisible(false); // Make pause image not visible by default

    // Button Listeners
    playPauseButton.addListener(this);
    loopButton.addListener(this);
    rockButton.addListener(this);
    jazzButton.addListener(this);
    hiphopButton.addListener(this);
    classicalButton.addListener(this);

    // Button backgrounds
    playPauseButton.setColour(TextButton::buttonColourId, colorTheme);

    // Title
    titleLabel.setText("Track", dontSendNotification);

    // Slider Listeners
    volSlider.addListener(this);
    speedSlider.addListener(this);
    posSlider.addListener(this);
    lowSlider.addListener(this);
    highSlider.addListener(this);

    // Set ranges for sliders
    volSlider.setRange(0.0f, 100.0f);
    speedSlider.setRange(0.1f, 10.0f);
    posSlider.setRange(0.0f, 1.0f);
    lowSlider.setRange(20.0, 20000.0);
    highSlider.setRange(20.0, 20000.0);

    // Set default value for slider
    volSlider.setValue(100.0f);
    speedSlider.setValue(1.0f);
    lowSlider.setValue(2000.0);
    highSlider.setValue(2000.0);

    // Set slider styles
    posSlider.setSliderStyle(Slider::SliderStyle::Rotary);
    posSlider.setRotaryParameters(0.0f, 2.0f * MathConstants<float>::pi, true);
    lowSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
    highSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
    posSlider.setLookAndFeel(new RotaryStyle(colorTheme));
    lowSlider.setLookAndFeel(new LinearStyle(colorTheme));
    highSlider.setLookAndFeel(new LinearStyle(colorTheme));
    volSlider.setLookAndFeel(new LinearStyle(colorTheme));
    speedSlider.setLookAndFeel(new LinearStyle(colorTheme));

    // Remove slider default labels
    posSlider.setTextBoxStyle(Slider::NoTextBox, false, 100, 20);
    lowSlider.setTextBoxStyle(Slider::NoTextBox, false, 100, 20);
    highSlider.setTextBoxStyle(Slider::NoTextBox, false, 100, 20);

    // Position slider labels
    lowLabel.setText("Low Pass", dontSendNotification);
    lowLabel.setJustificationType(juce::Justification::centred);
    highLabel.setText("High Pass", dontSendNotification);
    highLabel.setJustificationType(juce::Justification::centred);

    // Set decimal places
    volSlider.setNumDecimalPlacesToDisplay(0);
    speedSlider.setNumDecimalPlacesToDisplay(2);
    lowSlider.setNumDecimalPlacesToDisplay(0);
    highSlider.setNumDecimalPlacesToDisplay(0);

    startTimer(50);
}

DeckGUI::~DeckGUI()
{
    stopTimer();
}

void DeckGUI::paint (Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll(Colour(0x60, 0x60, 0x60));   // clear the background

    g.setColour(Colour(0x50, 0x50, 0x50));
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (Colours::white);
    g.setFont (14.0f);

    /** Draw outlines to visualize GUI bounds
    g.setColour(Colours::red);
    g.drawRect(getLocalBounds(), 1);
    g.drawRect(title.getBounds(), 1); 
    g.drawRect(waveformDisplay.getBounds(), 1); 
    g.drawRect(volSlider.getBounds(), 1);  
    g.drawRect(posSlider.getBounds(), 1);  
    g.drawRect(lowSlider.getBounds(), 1);
    g.drawRect(midSlider.getBounds(), 1);
    g.drawRect(highSlider.getBounds(), 1);
    g.drawRect(lowSliderLabel.getBounds(), 1);
    g.drawRect(midSliderLabel.getBounds(), 1);
    g.drawRect(highSliderLabel.getBounds(), 1);
    */
}

void DeckGUI::resized()
{
    double rowH = getHeight() / 5;
    double rowW = getWidth() / 5;

    titleLabel.setBounds(0, 0, rowW * 4, rowH / 2);
    waveformDisplay.setBounds(0, rowH / 2, getWidth(), rowH * 1.5);

    // Sliders
    posSlider.setBounds(0, rowH * 2, rowW * 2, rowH * 2);
    lowSlider.setBounds(rowW * 2, rowH * 2.2f, rowW, rowH * 1.7f);
    highSlider.setBounds(rowW * 3, rowH * 2.2f, rowW, rowH * 1.7f);
    volSlider.setBounds(rowW * 2, rowH * 4, rowW * 3, rowH / 2);
    speedSlider.setBounds(rowW * 2, rowH * 4.5f, rowW * 3, rowH / 2);

    // Labels
    lowLabel.setBounds(rowW * 2, rowH * 2, rowW, rowH / 4);
    highLabel.setBounds(rowW * 3, rowH * 2, rowW, rowH / 4);

    // Buttons
    playPauseButton.setBounds(5, rowH * 4.2f, rowW - 10, rowH * 0.6f);
    playImageComponent.setBounds(playPauseButton.getBounds());
    pauseImageComponent.setBounds(playPauseButton.getBounds());
    loopButton.setBounds(rowW, rowH * 4.2f, rowW - 5, rowH * 0.6f);
    rockButton.setBounds(rowW * 4, rowH * 2, rowW, rowH / 2);
    jazzButton.setBounds(rowW * 4, rowH * 2.5f, rowW, rowH / 2);
    hiphopButton.setBounds(rowW * 4, rowH * 3, rowW, rowH / 2);
    classicalButton.setBounds(rowW * 4, rowH * 3.5f, rowW, rowH / 2);
}

void DeckGUI::buttonClicked(Button* button)
{
    if (button == &playPauseButton && player->isAudioLoaded())
    {
        if (player->isAudioPlaying() == false)
        {
            player->start();
            playImageComponent.setVisible(false);
            pauseImageComponent.setVisible(true);

        }
        else if (player->isAudioPlaying())
        {
            player->stop();
            playImageComponent.setVisible(true);
            pauseImageComponent.setVisible(false);
        }
    }
    if (button == &loopButton)
    {
        isLoop = !isLoop;
        updateLoopButton();
    }
    if (button == &rockButton)
    {
        lowSlider.setValue(16000.0);
        highSlider.setValue(2000.0);
    }
    if (button == &jazzButton)
    {
        lowSlider.setValue(8000.0);
        highSlider.setValue(2000.0);
    }
    if (button == &hiphopButton)
    {
        lowSlider.setValue(15000.0);
        highSlider.setValue(10000.0);
    }
    if (button == &classicalButton)
    {
        lowSlider.setValue(10000.0);
        highSlider.setValue(12000.0);
    }
}

void DeckGUI::sliderValueChanged (Slider *slider)
{
    if (slider == &volSlider)
    {
        player->setGain(slider->getValue());
    }
    if (slider == &speedSlider)
    {
        player->setSpeed(slider->getValue());
    }
    if (slider == &posSlider)
    {
        player->setPositionRelative(slider->getValue());
    }
    if (slider == &lowSlider && player->isAudioLoaded())
    {
        player->setLowPass(slider->getValue());
    }
    if (slider == &highSlider && player->isAudioLoaded())
    {
        player->setHighPass(slider->getValue());
    }
}

bool DeckGUI::isInterestedInFileDrag (const StringArray &files)
{
    std::cout << "DeckGUI::isInterestedInFileDrag" << std::endl;
    return true; 
}

void DeckGUI::filesDropped (const StringArray &files, int x, int y)
{
    std::cout << "DeckGUI::filesDropped" << std::endl;
    if (files.size() == 1)
    {
        player->loadURL(URL{File{files[0]}});
    }
}

void DeckGUI::timerCallback()
{
    waveformDisplay.setPositionRelative(player->getPositionRelative());

    if (player->isAudioLoaded())
    {
        // Set position of posSlider according to the player's position
        posSlider.setValue(player->getPositionRelative());

        if (player->hasplaybackFinished() && isLoop == false)
        {
            player->stop();
            playImageComponent.setVisible(true);
            pauseImageComponent.setVisible(false);
            posSlider.setValue(0);
        }
        else if (player->hasplaybackFinished() && isLoop == true)
        {
            playImageComponent.setVisible(false);
            pauseImageComponent.setVisible(true);
            posSlider.setValue(0);
            player->start();
        }
    }
}

void DeckGUI::loadURL(URL audioURL, const String& title)
{
    // Set the song title
    songTitle = title;
    titleLabel.setText(songTitle, dontSendNotification);

    // Update the button
    playImageComponent.setVisible(true);
    pauseImageComponent.setVisible(false);

    // Load player with song URL
    player->loadURL(audioURL);
    // and now the waveformDisplay as well
    waveformDisplay.loadURL(audioURL);
}

// My code
void DeckGUI::updateLoopButton()
{
    if (isLoop)
    {
        loopButton.setColour(TextButton::buttonColourId, Colour(colorTheme));
    }
    else
    {
        loopButton.setColour(TextButton::buttonColourId, LookAndFeel::getDefaultLookAndFeel().findColour(TextButton::buttonColourId));
    }
}

    

