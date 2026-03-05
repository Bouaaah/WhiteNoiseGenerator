#include "View.hpp"
#include <limits>

// View

View::View()
{
    mainComponent = std::make_unique<MainComponent>();
    mainWindow = std::make_unique<MainWindow>();
    mainWindow->setContentNonOwned(mainComponent.get(), false);

    audioParamWindow = std::make_unique<AudioParamWindow>();
    audioParamComponent = std::make_unique<AudioParamComponent>();
    audioParamWindow->setContentNonOwned(audioParamComponent.get(), false);

    mainComponent->addStopButtonListener(this);
    mainComponent->addPlayButtonListener(this);
    mainComponent->addSaveButtonListener(this);
    mainComponent->addAudioParamButtonListener(this);
    mainComponent->addFileExplorerButtonListener(this);
    mainComponent->addAmplitudeSliderListener(this);

    mainComponent->addAmplitudeFieldListener(this);
    mainComponent->addDurationFieldListener(this);
}

void View::buttonClicked(Button *btn)
{
    auto name = btn->getName();
    if (name == "Stop")
    {
        sendActionMessage("StopClicked");
    }
    else if (name == "Play")
    {
        sendActionMessage("PlayClicked");
    }
    else if (name == "Save")
    {
        sendActionMessage("SaveClicked");
    }
    else if (name == "AudioParam")
    {
        audioParamWindow->setVisible(true);
        audioParamWindow->toFront(true);
    }
    else if (name == "FileExplorer")
    {
        fileExplorerWindow = std::make_unique<FileChooser>("Test",File(),"*.wav");
        auto folderChooserFlags = FileBrowserComponent::saveMode | FileBrowserComponent::canSelectFiles | FileBrowserComponent::warnAboutOverwriting;
        fileExplorerWindow->launchAsync(folderChooserFlags,
                                        [this](const FileChooser &chooser){ 
                                            mainComponent->setSavePath(chooser.getResult().getFullPathName()); 
                                        });
    }
}

void View::textEditorFocusLost(TextEditor& textEditor)
{
    if(textEditor.getName() == "AmplitudeField")
    {
        amplitude = jlimit(-60.0f,0.0f,mainComponent->getAmplitudeField());
        sendActionMessage("AmplitudeChanged");
    }
    else if (textEditor.getName() == "DurationField")
    {
        duration = jlimit(0.0f,99999999.0f,mainComponent->getDurationField());
        sendActionMessage("DurationChanged");
    }
    
}

void View::textEditorReturnKeyPressed(TextEditor& textEditor)
{
    if(textEditor.getName() == "AmplitudeField")
    {
        textEditor.giveAwayKeyboardFocus();
    }
    else if (textEditor.getName() == "DurationField")
    {
        textEditor.giveAwayKeyboardFocus();
    }
    
}

void View::sliderValueChanged(Slider* slider)
{
    if(slider->getName() == "AmplitudeSlider")
    {
        amplitude = jlimit(-60.0f,0.0f,mainComponent->getAmplitudeSlider());
        sendActionMessage("AmplitudeChanged");
    }
}

void View::setAudioManager(AudioDeviceManager *audioManager)
{
    audioParamComponent->setAudioManager(audioManager);
}

void View::updateAudioParam_display(const String& text)
{
    mainComponent->updateAudioParam_display(text);
}

void View::setPlayingState()
{
    mainComponent->setPlayingState();
}
void View::setStopState()
{
    mainComponent->setStopState();
}
void View::setUnvalidState()
{
    mainComponent->setUnvalidState();
}

void View::setAmplitude(float a)
{
    amplitude = a;
    mainComponent->setAmplitudeField(amplitude);
    mainComponent->setAmplitudeSlider(amplitude);
}
float View::getAmplitude()
{
    return amplitude;
}

void View::setDuration(float d)
{
    duration = d;
    mainComponent->setDurationField(duration);
}
float View::getDuration()
{
    return duration;
}

String View::getPath()
{
    return mainComponent->getPath();
}

int View::getFs()
{
    return mainComponent->getFs();
}

// MainWindow

View::MainWindow::MainWindow()
    : DocumentWindow("",
                     LookAndFeel::getDefaultLookAndFeel().findColour(DocumentWindow::backgroundColourId),
                     allButtons,
                     true)
{
    setName(JUCEApplication::getInstance()->getApplicationName() + String(" - ") + JUCEApplication::getInstance()->getApplicationVersion());
    setVisible(true);
    setResizable(true, false);
    setResizeLimits(580, 250,
                    std::numeric_limits<int>::max(),
                    std::numeric_limits<int>::max());
    centreWithSize(700, 350);
}

void View::MainWindow::closeButtonPressed()
{
    JUCEApplication::getInstance()->systemRequestedQuit();
}

// MainComponent

View::MainComponent::MainComponent()
{
    duration_field.setName("DurationField");
    addAndMakeVisible(duration_field);

    amplitude_field.setName("AmplitudeField");
    addAndMakeVisible(amplitude_field);

    amplitude_slider.setName("AmplitudeSlider");
    amplitude_slider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    amplitude_slider.setRange(-60,0,0.1);
    addAndMakeVisible(amplitude_slider);

    stop_button.setButtonText("Stop");
    stop_button.setName("Stop");
    addAndMakeVisible(stop_button);

    play_button.setButtonText("Play");
    play_button.setName("Play");
    addAndMakeVisible(play_button);

    save_button.setButtonText("Save");
    save_button.setName("Save");
    addAndMakeVisible(save_button);

    audioParam_button.setButtonText("");
    audioParam_button.setName("AudioParam");
    addAndMakeVisible(audioParam_button);

    audioParam_display.setReadOnly(true);
    audioParam_display.setMultiLine(true);
    addAndMakeVisible(audioParam_display);

    duration_label.setText("Duration (s)", dontSendNotification);
    duration_label.setJustificationType(Justification::right);
    addAndMakeVisible(duration_label);

    amplitude_label.setText("Amplitude (dB)", dontSendNotification);
    amplitude_label.setJustificationType(Justification::right);
    addAndMakeVisible(amplitude_label);

    addAndMakeVisible(savePath_field);

    fileExplorer_button.setName("FileExplorer");
    addAndMakeVisible(fileExplorer_button);

    fs_field.addItem(String("44100"),1);
    fs_field.addItem(String("48000"),2);
    fs_field.addItem(String("96000"),3);
    fs_field.addItem(String("192000"),4);
    fs_field.setSelectedId(1);
    addAndMakeVisible(fs_field);

    fs_label.setText("Fs (Hz)", dontSendNotification);
    fs_label.setJustificationType(Justification::right);
    addAndMakeVisible(fs_label);
}

void View::MainComponent::resized()
{
    Point<int> centreMain = getLocalBounds().getCentre();
    int border = 10;
    int paddingField = 5;
    int paddingSmallButton = 5;
    int paddingBigButton = 15;
    int widthField = 60;
    int heightField = 20;
    int widthBigButton = 50;
    int heightBigButton = 50;
    int widthSmallButton = 20;
    int heightSmallButton = 20;

    duration_field.setBounds(Rectangle<int>(widthField, heightField)
                                 .withX(centreMain.getX())
                                 .withY(border));

    duration_label.setBounds(Rectangle<int>(duration_label.getFont().getStringWidth(duration_label.getText()), heightField)
                                 .withRightX(duration_field.getX() - paddingField)
                                 .withY(duration_field.getY()));

    amplitude_field.setBounds(Rectangle<int>(widthField, heightField)
                                  .withX(centreMain.getX())
                                  .withY(duration_field.getBottom() + paddingField));

    amplitude_label.setBounds(Rectangle<int>(amplitude_label.getFont().getStringWidth(amplitude_label.getText()), heightField)
                                  .withRightX(amplitude_field.getX() - paddingField)
                                  .withY(amplitude_field.getY()));

    amplitude_slider.setBounds(Rectangle<int>(widthField + 10, heightField) // On ajoute 10 à la width pour un meilleur alignement avec les fields.
                                   .withX(centreMain.getX() - 5)
                                   .withY(amplitude_field.getBottom() + paddingField / 2));

    audioParam_button.setBounds(Rectangle<int>(widthSmallButton, heightSmallButton)
                                    .withRightX(centreMain.getX() - border)
                                    .withBottomY(getHeight() - border - heightBigButton - paddingBigButton));

    audioParam_display.setBounds(Rectangle<int>(0, 0)
                                     .withBottom(getHeight() - border - heightBigButton - paddingBigButton)
                                     .withTop(amplitude_slider.getBottom() + border + 5)
                                     .withLeft(border)
                                     .withRight(audioParam_button.getX() - paddingSmallButton));

    stop_button.setBounds(Rectangle<int>(widthBigButton, heightBigButton)
                              .withX(audioParam_display.getBounds().getCentreX() + paddingBigButton / 2)
                              .withBottomY(getHeight() - border));

    play_button.setBounds(Rectangle<int>(widthBigButton, heightBigButton)
                              .withRightX(audioParam_display.getBounds().getCentreX() - paddingBigButton / 2)
                              .withBottomY(getHeight() - border));

    save_button.setBounds(Rectangle<int>(widthBigButton, heightBigButton)
                              .withCentre(Point<int>(getWidth() * 3 / 4, 0))
                              .withBottomY(getHeight() - border));

    savePath_field.setBounds(Rectangle<int>(0, heightField)
                                 .withLeft(getWidth() / 2 + border)
                                 .withRight(getWidth() - border - widthSmallButton - paddingSmallButton)
                                 .withY(amplitude_slider.getBottom() + border + 5));

    fileExplorer_button.setBounds(Rectangle<int>(widthSmallButton, heightSmallButton)
                                      .withRightX(getWidth() - border)
                                      .withY(savePath_field.getY()));

    fs_field.setBounds(Rectangle<int>(widthField * 2, heightField)
                           .withY(savePath_field.getBottom() + paddingField)
                           .withX(savePath_field.getBounds().getCentreX()));

    fs_label.setBounds(Rectangle<int>(fs_label.getFont().getStringWidth(fs_label.getText()), heightField)
                           .withRightX(fs_field.getX() - paddingField)
                           .withY(fs_field.getY()));
}

void View::MainComponent::paint(Graphics &g)
{
    auto horizontalSeparatorY = amplitude_slider.getBottom() + 5;
    auto verticalSeparatorX = getWidth() / 2;
    float thickness = 1.0;
    g.setColour(Colours::lightgrey);
    g.drawLine(verticalSeparatorX, horizontalSeparatorY, verticalSeparatorX, getHeight(), thickness);
    g.drawLine(0, horizontalSeparatorY, getWidth(), horizontalSeparatorY, thickness);
}

void View::MainComponent::addStopButtonListener(Button::Listener *listener)
{
    stop_button.addListener(listener);
}

void View::MainComponent::addPlayButtonListener(Button::Listener *listener)
{
    play_button.addListener(listener);
}

void View::MainComponent::addSaveButtonListener(Button::Listener *listener)
{
    save_button.addListener(listener);
}

void View::MainComponent::addAudioParamButtonListener(Button::Listener *listener)
{
    audioParam_button.addListener(listener);
}

void View::MainComponent::addFileExplorerButtonListener(Button::Listener *listener)
{
    fileExplorer_button.addListener(listener);
}

void View::MainComponent::addAmplitudeFieldListener(TextEditor::Listener* listener)
{
    amplitude_field.addListener(listener);
}

void View::MainComponent::addDurationFieldListener(TextEditor::Listener* listener)
{
    duration_field.addListener(listener);
}

void View::MainComponent::addAmplitudeSliderListener(Slider::Listener* listener)
{
    amplitude_slider.addListener(listener);
}

void View::MainComponent::setSavePath(const String& path)
{
    savePath_field.setText(path,dontSendNotification);
}

void View::MainComponent::updateAudioParam_display(const String& text)
{
    audioParam_display.setText(text,dontSendNotification);
}

void View::MainComponent::setPlayingState()
{
    play_button.setEnabled(false);
    stop_button.setEnabled(true);
    duration_field.setEnabled(false);
    amplitude_field.setEnabled(false);
    amplitude_slider.setEnabled(false);
}
void View::MainComponent::setStopState()
{
    play_button.setEnabled(true);
    stop_button.setEnabled(false);
    duration_field.setEnabled(true);
    amplitude_field.setEnabled(true);
    amplitude_slider.setEnabled(true);
}
void View::MainComponent::setUnvalidState()
{
    play_button.setEnabled(false);
    stop_button.setEnabled(false);
    duration_field.setEnabled(true);
    amplitude_field.setEnabled(true);
    amplitude_slider.setEnabled(true);
}

float View::MainComponent::getAmplitudeField()
{
    return amplitude_field.getText().getFloatValue();
}

void View::MainComponent::setAmplitudeField(float a)
{
    amplitude_field.setText(String(a));
}

float View::MainComponent::getDurationField()
{
    return duration_field.getText().getFloatValue();
}

void View::MainComponent::setDurationField(float a)
{
    duration_field.setText(String(a));
}

float View::MainComponent::getAmplitudeSlider()
{
    return amplitude_slider.getValue();
}

void View::MainComponent::setAmplitudeSlider(float a)
{
    amplitude_slider.setValue(a, dontSendNotification);
}

String View::MainComponent::getPath()
{
    return savePath_field.getText();
}

int View::MainComponent::getFs()
{
    return fs_field.getText().getIntValue();
}

// AudioParamWindow

View::AudioParamWindow::AudioParamWindow()
    : ResizableWindow("Audio Parameters", true)
{
    centreWithSize(300, 300);
    setResizable(true, false);
    setUsingNativeTitleBar(false);
    addMouseListener(this, true);
}

void View::AudioParamWindow::mouseDown(const MouseEvent &e)
{
    dragger.startDraggingComponent(this, e);
}

void View::AudioParamWindow::mouseDrag(const MouseEvent &e)
{
    dragger.dragComponent(this, e, nullptr);
}

void View::AudioParamWindow::userTriedToCloseWindow()
{
    setVisible(false);
}
// AudioParamComponent

View::AudioParamComponent::AudioParamComponent()
{
    ok_button.onClick = [this]
    { getTopLevelComponent()->setVisible(false); };
    addAndMakeVisible(ok_button);
}

void View::AudioParamComponent::resized()
{
    ok_button.setBounds(Rectangle<int>(50, 20)
                            .withCentre(getLocalBounds().getCentre())
                            .withBottomY(getHeight() - 10));
    if (audioDeviceSelector)
    {
        audioDeviceSelector->setBounds(getLocalBounds());
    }
}

void View::AudioParamComponent::setAudioManager(AudioDeviceManager *audioManager)
{
    audioDeviceSelector = std::make_unique<AudioDeviceSelectorComponent>(*audioManager, 0, 0, 0, std::numeric_limits<int>::max(), false, false, false, false);
    if (audioDeviceSelector)
    {
        addAndMakeVisible(*audioDeviceSelector.get(),0);
        resized();
    }
}