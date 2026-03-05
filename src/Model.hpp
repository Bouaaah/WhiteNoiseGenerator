#pragma once
#include <JuceHeader.h>

class Model : public ActionBroadcaster
{
public:
    Model();
    AudioDeviceManager* getAudioManager();
    void start();
    void stop();
    bool isPlaying();
    void save(String path, int sampleRate);

    float amplitude = -6;
    float duration = 2;

private:
    class WhiteNoiseAudioSource : public AudioSource
    {
    public:
        void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
        void getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) override;
        void releaseResources() override;
        float amplitude;
    private:
        
    };

    WhiteNoiseAudioSource whiteNoiseSource;
    AudioSourcePlayer player;
    AudioDeviceManager audioManager;
    TimedCallback timer{[this]() { sendActionMessage("StopClicked"); }};


};