#include "Controller.hpp"
#include <iostream>

Controller::Controller(View &v, Model &m)
    : view(v), model(m)
{   
    view.addActionListener(this);
    model.addActionListener(this);

    view.setAudioManager(model.getAudioManager());
    model.getAudioManager()->addChangeListener(this);
    changeListenerCallback(model.getAudioManager());
    view.setStopState();
    view.setAmplitude(model.amplitude);
    view.setDuration(model.duration);
}

void Controller::actionListenerCallback(const String &message) 
{
    if(message == "StopClicked")
    {
        model.stop();
        view.setStopState();
        std::cout << "Stop" << std::endl;
    }
    else if(message == "PlayClicked")
    {
        model.start();
        view.setPlayingState();
        std::cout << "Play" << std::endl;
    }
    else if(message == "SaveClicked")
    {
        std::cout << "Save" << std::endl;
        model.save(view.getPath(),view.getFs());
    }
    else if(message == "AmplitudeChanged")
    {
        model.amplitude = view.getAmplitude();
        view.setAmplitude(model.amplitude);
        std::cout << "Amplitude changed : " << model.amplitude << std::endl;
    }
    else if(message == "DurationChanged")
    {
        model.duration = view.getDuration();
        view.setDuration(model.duration);
        std::cout << "Duration changed : " << model.duration << std::endl;
    }
    
}

void Controller::changeListenerCallback(ChangeBroadcaster* source)
{
    auto manager = model.getAudioManager();
    if(source == manager)
    {
        view.updateAudioParam_display(audioManagerToString(manager));
        if(!manager->getCurrentAudioDevice())
        {
            view.setUnvalidState();
        }
        else
        {
            if (model.isPlaying())
            {
                view.setPlayingState();
            }
            else{
                view.setStopState();
            }
        }

    }
}

String Controller::audioManagerToString(AudioDeviceManager* audioManager)
{
    auto type = (audioManager->getCurrentDeviceTypeObject() != nullptr
                                                     ? audioManager->getCurrentDeviceTypeObject()->getTypeName()
                                                     : "<none>");
    auto type_str = String("Type : ") + type;

    if (AudioIODevice* device = audioManager->getCurrentAudioDevice())
    {
        auto device_name = device->getName();
        auto device_name_str = String("Device : ") + device_name;

        auto fs = device->getCurrentSampleRate();
        auto fs_str = String("Sample rate : ") + String(fs) + String(" Hz");

        auto buffSize = device->getCurrentBufferSizeSamples();
        auto buffSize_str = String("Buffer size : ") + String(buffSize) + String(" samples");

        auto latency = device->getOutputLatencyInSamples() / fs *1000;
        auto latency_str = String("Output latency : ") + String(latency) + String(" ms");

        auto bitDepth = device->getCurrentBitDepth();
        auto bitDepth_str = String("Bit depth : ") + String(bitDepth) + String(" bits");

        StringArray activeChannelNames;
        auto channelNames   = device->getOutputChannelNames();
        auto activeChannels = device->getActiveOutputChannels();
        for (int i = 0; i < channelNames.size(); ++i)
        {
            if (activeChannels[i]) // ou activeChannels.contains(i)
                activeChannelNames.add(channelNames[i]);
        }
        auto outChans = activeChannelNames.joinIntoString(", ");

        auto outChans_str = String("Output channels : ") + outChans;

        auto full_str = String("Audio Device : ") + "\n" + 
            String("-----------------------------") + "\n" +  
            type_str + "\n" + 
            device_name_str + "\n" +
            fs_str + "\n" +
            buffSize_str + "\n" +
            latency_str + "\n" +
            bitDepth_str + "\n" +
            outChans_str;

        return full_str;
    }
    else{
        return type_str + "\n" + String("No audio device open");
    }




    
}