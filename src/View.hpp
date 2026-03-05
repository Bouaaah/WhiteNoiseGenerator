#pragma once
#include <JUCEHeader.h>

class View : public ActionBroadcaster, public Button::Listener, public TextEditor::Listener, public Slider::Listener
{
public:
View();

void setAudioManager(AudioDeviceManager* audioManager);
void updateAudioParam_display(const String& text);
void setPlayingState();
void setStopState();
void setUnvalidState();
void setAmplitude(float a);
float getAmplitude();
void setDuration(float a);
float getDuration();
String getPath();
int getFs();

private:
    class MainComponent: public Component
    {
    public:
        MainComponent();

        void resized() override;
        void paint(Graphics& g) override;

        void setSavePath(const String& path);
        void updateAudioParam_display(const String& text);

        void addStopButtonListener(Button::Listener* listener);
        void addPlayButtonListener(Button::Listener* listener);
        void addSaveButtonListener(Button::Listener* listener);
        void addAudioParamButtonListener(Button::Listener* listener);
        void addFileExplorerButtonListener(Button::Listener *listener);

        void addAmplitudeFieldListener(TextEditor::Listener* listener);
        void addDurationFieldListener(TextEditor::Listener* listener);
        void addAmplitudeSliderListener(Slider::Listener* listener);

        void setAmplitudeField(float a);
        float getAmplitudeField();
        void setDurationField(float a);
        float getDurationField();

        void setAmplitudeSlider(float a);
        float getAmplitudeSlider();

        void setPlayingState();
        void setStopState();
        void setUnvalidState();

        String getPath();
        int getFs();

    private:
        Label duration_label;
        TextEditor duration_field;

        Label amplitude_label;
        TextEditor amplitude_field;
        Slider amplitude_slider;

        TextEditor audioParam_display;
        TextButton audioParam_button;

        TextButton  play_button;
        TextButton stop_button;

        TextEditor savePath_field;
        TextButton fileExplorer_button;
        
        Label fs_label;
        ComboBox fs_field;

        TextButton save_button;
    };

    class MainWindow : public DocumentWindow
    {
    public:
        MainWindow();
        void closeButtonPressed() override;
    };
    
    class AudioParamWindow : public ResizableWindow
    {
    public:
        AudioParamWindow();
        void userTriedToCloseWindow() override;
        void mouseDown(const MouseEvent &e) override;
        void mouseDrag(const MouseEvent &e) override;
    private:
        ComponentDragger dragger;
    };
    
    class AudioParamComponent : public Component
    {
    public:
        AudioParamComponent();
        void resized() override;
        void setAudioManager(AudioDeviceManager* audioManager);
    private:
        TextButton ok_button{"Ok"};
        std::unique_ptr<AudioDeviceSelectorComponent> audioDeviceSelector;
    };

    void buttonClicked (Button * btn) override;

    void textEditorFocusLost(TextEditor& textEditor) override;
    void textEditorReturnKeyPressed(TextEditor& textEditor) override;

    void sliderValueChanged(Slider* slider) override;
    float amplitude;
    float duration;

    std::unique_ptr<AudioParamWindow> audioParamWindow;
    std::unique_ptr<AudioParamComponent> audioParamComponent;
    std::unique_ptr<MainWindow> mainWindow;
    std::unique_ptr<MainComponent> mainComponent;
    std::unique_ptr<FileChooser> fileExplorerWindow;
};

