/*
  ==============================================================================

    PlaylistComponent.h
    Created: 15 Aug 2023 4:59:38pm
    Author:  Liu Junhua

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DeckGUI.h"
#include <fstream>
#include <vector>
#include <string>

//==============================================================================
/*
*/
class PlaylistComponent  : public Component, 
                           public TableListBoxModel, 
                           public Button::Listener,
                           public TextEditor::Listener
{
public:
    PlaylistComponent(DeckGUI& _deckGUI1, DeckGUI& _deckGUI2);
    ~PlaylistComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    int getNumRows() override;
    void paintRowBackground(Graphics & g, int rowNumber, int width, int height, bool rowIsSelected) override;
    void paintCell(Graphics & g, int rowNumber, int columnId, int width, int height, bool rowIsSelected) override;

    Component* refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component *existingComponentToUpdate) override;
    
    void buttonClicked(Button* button) override;

    // Load and save files
    void saveStateToFile();
    void loadStateFromFile();

    TextEditor searchBox;

    void filterLibrary(const String& searchTerm);
    void textEditorTextChanged(TextEditor& editor) override;

private:

    struct MusicLibraryEntry
    {
        String filePath;
        String title;

        MusicLibraryEntry(const String& _filePath,
                          const String& _title)
            : filePath(_filePath), title(_title) {}
    };

    FileChooser fChooser{ "Select a file..." };
    TableListBox tableComponent;
    std::vector<String> trackTitles;
    std::vector<MusicLibraryEntry> musicLibraryEntries;

    DeckGUI& deckGUI1;
    DeckGUI& deckGUI2;

    TextButton loadButton;
    void loadButtonClicked();
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlaylistComponent)
};

