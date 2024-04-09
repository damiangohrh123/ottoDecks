/*
  ==============================================================================

    PlaylistComponent.cpp
    Created: 15 Aug 2023 4:59:38pm
    Author:  Liu Junhua

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PlaylistComponent.h"

//==============================================================================
PlaylistComponent::PlaylistComponent(DeckGUI& _deckGUI1, DeckGUI& _deckGUI2)
    : deckGUI1(_deckGUI1), deckGUI2(_deckGUI2)
{   
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    tableComponent.getHeader().addColumn("Playlist Library", 1, 500);
    tableComponent.getHeader().addColumn("", 2, 100);
    tableComponent.getHeader().addColumn(" ", 3, 100);
    tableComponent.getHeader().addColumn("", 4, 100);
    
    tableComponent.setModel(this);
    searchBox.addListener(this);
    
    addAndMakeVisible(tableComponent);
    addAndMakeVisible(loadButton);
    addAndMakeVisible(searchBox);

    // Search box
    searchBox.setColour(TextEditor::backgroundColourId, Colours::white);
    searchBox.setColour(TextEditor::textColourId, Colours::black);
    searchBox.setColour(TextEditor::outlineColourId, Colour(0x5F, 0x5F, 0x5F));

    // Load button
    loadButton.setButtonText("Load Music");
    loadButton.onClick = [this] {loadButtonClicked(); };

    // Load data from txt file
    loadStateFromFile();

    // Update the playlist
    tableComponent.updateContent();
}

PlaylistComponent::~PlaylistComponent()
{
    saveStateToFile();
}

void PlaylistComponent::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    g.setColour (juce::Colours::grey);

    g.setColour (juce::Colours::white);
    g.setFont (14.0f);
    g.drawText ("PlaylistComponent", getLocalBounds(), juce::Justification::centred, true);
}

void PlaylistComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    double rowH = getHeight() / 8;
    double rowW = getWidth() / 6;

    tableComponent.setBounds(0, 0, getWidth(), rowH * 5);
    tableComponent.getHeader().setColumnWidth(1, getWidth() * 0.75f);
    tableComponent.getHeader().setColumnWidth(2, getWidth() * 0.1f);
    tableComponent.getHeader().setColumnWidth(3, getWidth() * 0.1f);
    tableComponent.getHeader().setColumnWidth(4, getWidth() * 0.05f);


    loadButton.setBounds(rowW * 5, rowH * 5.25f, 100, 20);
    searchBox.setBounds(rowW * 5, 0, rowW, 25);
}

int PlaylistComponent::getNumRows()
{
    return trackTitles.size();
}

void PlaylistComponent::paintRowBackground(Graphics & g, int rowNumber, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected) 
    {
        g.fillAll(Colour(0x2b, 0x2b, 0x2b));
    }
    else
    {
        g.fillAll(Colour(0x60, 0x60, 0x60));
    }
}

void PlaylistComponent::paintCell(Graphics & g, int rowNumber, int columnId, int width, int height, bool rowIsSelected)
{
    g.setColour(Colours::white);
    g.drawText(trackTitles[rowNumber],2,0,width-4, height, Justification::centredLeft,true);
}

Component* PlaylistComponent::refreshComponentForCell(int rowNumber, int columnId, bool isRowSelected, Component *existingComponentToUpdate)
{
    if(columnId == 2)
    {
        if(existingComponentToUpdate==nullptr)
        {
            TextButton* btn = new TextButton{"player 1"};
            String id{std::to_string(rowNumber)};
            btn->setComponentID("player1" + id);
            
            btn->addListener(this);
            existingComponentToUpdate=btn;
        }
    }
    if (columnId == 3)
    {
        if (existingComponentToUpdate == nullptr)
        {
            TextButton* btn = new TextButton{ "player 2" };
            String id{ std::to_string(rowNumber) };
            btn->setComponentID("player2" + id);

            btn->addListener(this);
            existingComponentToUpdate = btn;
        }
    }
    if (columnId == 4)
    {
        if (existingComponentToUpdate == nullptr)
        {
            TextButton* btn = new TextButton{ "X" };
            String id{ std::to_string(rowNumber) };
            btn->setComponentID("remove" + id);

            btn->addListener(this);
            existingComponentToUpdate = btn;
        }
    }
    return existingComponentToUpdate;
}

void PlaylistComponent::buttonClicked(Button* button) 
{
    String buttonId = button->getComponentID();

    if (buttonId.startsWith("remove"))
    {
        int rowNumber = buttonId.fromLastOccurrenceOf("remove", false, true).getIntValue();
        
        // Make sure rowNumber is within range
        if (rowNumber >= 0 &&  rowNumber < trackTitles.size())
        {
            trackTitles.erase(trackTitles.begin() + rowNumber);

            if (rowNumber < musicLibraryEntries.size())
            {
                // Delete the entrey from musicLibraryEntries
                musicLibraryEntries.erase(musicLibraryEntries.begin() + rowNumber);
            }

            // Update playlist
            tableComponent.updateContent();
            resized();

            // Save state to text file
            saveStateToFile();
        }
        else
        {
            DBG("Invalid row number to remove.");
        }
    }
    else if (buttonId.startsWith("player1"))
    {
        int rowNumber = buttonId.fromLastOccurrenceOf("player1", false, true).getIntValue();

        // Make sure rowNumber is within range
        if (rowNumber >=0 && rowNumber < musicLibraryEntries.size())
        {
            // Create a file, retrieve song title, then load into deckGUI1
            File localFile(musicLibraryEntries[rowNumber].filePath);
            String songTitle = musicLibraryEntries[rowNumber].title;
            deckGUI1.loadURL(URL{ localFile }, songTitle);
        }
        else
        {
            DBG("Invalid row number to add to player1.");
        }
    }
    else if (buttonId.startsWith("player2"))
    {
        int rowNumber = buttonId.fromLastOccurrenceOf("player2", false, true).getIntValue();
        
        // Make sure rowNumber is within range
        if (rowNumber >= 0 && rowNumber < musicLibraryEntries.size())
        {
            // Create a file, retrieve song title, then load into deckGUI2
            File localFile(musicLibraryEntries[rowNumber].filePath);
            String songTitle = musicLibraryEntries[rowNumber].title;
            deckGUI2.loadURL(URL{ localFile }, songTitle);
        }
        else
        {
            DBG("Invalid row number to add to player2.");
        }
    }
}

// My code
void PlaylistComponent::loadButtonClicked()
{
    auto fileChooserFlags = FileBrowserComponent::canSelectFiles;

    fChooser.launchAsync(fileChooserFlags, [this](const FileChooser& chooser)
    {
        if (chooser.getResult() != File())
        {
            const Array<File>& selectedFiles = chooser.getResults();

            for (const auto& file : selectedFiles)
            {
                // Get title
                String title = file.getFileNameWithoutExtension();

                // Create a new MusicLibrary entry and add it to the musicLibraryEntries vector
                MusicLibraryEntry entry(file.getFullPathName(), title);
                musicLibraryEntries.push_back(entry);

                // Add title to the trackTitles vector
                trackTitles.push_back(title);
            }

            // Update the playlist
            tableComponent.updateContent();

            saveStateToFile();

            /** DEBUGGING PURPOSES
            for (const auto& entry : musicLibraryEntries)
            {
                DBG("File Path: " << entry.filePath);
                DBG("Title: " << entry.title);
                DBG("-------------");
            }
            */
        }
        else
        {
            // User canceled the file chooser
            DBG("File chooser canceled");
        }
    });
}

// My code
void PlaylistComponent::saveStateToFile()
{
    File appDir = File::getSpecialLocation(File::SpecialLocationType::currentApplicationFile).getParentDirectory();
    File file = appDir.getChildFile("file.txt");

    std::ofstream outputFile(file.getFullPathName().toStdString());

    if (outputFile.is_open())
    {
        // Write the file path and title of each entry in
        // musicLibraryEntries to the output file
        for (const auto& entry : musicLibraryEntries)
        {
            outputFile << entry.filePath.toStdString() << "," << entry.title.toStdString() << "\n";
        }

        // Close file
        outputFile.close();
        DBG("Data saved to file: " << file.getFullPathName());
    }
    else
    {
        DBG("Failed to open file for saving.");
    }
}

// My code
void PlaylistComponent::loadStateFromFile() {
    File appDir = File::getSpecialLocation(File::SpecialLocationType::currentApplicationFile).getParentDirectory();
    File file = appDir.getChildFile("file.txt");

    std::ifstream inputFile(file.getFullPathName().toStdString());

    if (inputFile.is_open()) 
    {
        // Clear musicLibraryEntries vector (Make a clean slate).
        musicLibraryEntries.clear();

        std::string line;
        while (std::getline(inputFile, line)) 
        {

            // Tokenize the lines
            size_t commaPos = line.find(',');
            if (commaPos != std::string::npos)
            {
                String filePath = line.substr(0, commaPos).c_str();
                String title = line.substr(commaPos + 1).c_str();

                // Create music libry entry and push it back to MusicLibraryEntries vector
                MusicLibraryEntry entry(filePath, title);
                musicLibraryEntries.push_back(entry);

                // Add title to the trackTitles vector
                trackTitles.push_back(title);
            }
            else
            {
                DBG("Invalid line in file: " << line);
            }
        }

        // Close file
        inputFile.close();
        DBG("Data loaded from file: " << file.getFullPathName());
    }
    else 
    {
        DBG("Failed to open file for loading.");
    }
}

void PlaylistComponent::filterLibrary(const String& searchTerm)
{
    // Clear the trackTitles vector
    trackTitles.clear();

    // Filter music library based on the search term
    for (const auto& entry : musicLibraryEntries)
    {
        if (entry.title.containsIgnoreCase(searchTerm))
        {
            trackTitles.push_back(entry.title);
        }
    }
    // Update playlist table
    tableComponent.updateContent();
}

void PlaylistComponent::textEditorTextChanged(TextEditor& editor)
{
    filterLibrary(editor.getText());
}