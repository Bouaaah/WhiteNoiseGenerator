#include "Model.hpp"



Model::Model()
{
    audioManager.initialiseWithDefaultDevices(0,1);
    audioManager.addAudioCallback(&player);
}
AudioDeviceManager* Model::getAudioManager()
{
    return &audioManager;
}
void Model::start()
{
    whiteNoiseSource.amplitude = powf(10,amplitude/20);
    player.setSource(&whiteNoiseSource);
    timer.startTimer(duration*1000);
}
void Model::stop()
{
    player.setSource(nullptr);
    timer.stopTimer();
}

bool Model::isPlaying()
{
    if(player.getCurrentSource() == nullptr)
    {
        return false;
    }
    else
    {
        return true;
    }
}

void Model::WhiteNoiseAudioSource::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    std::cout <<"Prepared" << std::endl;
}
void Model::WhiteNoiseAudioSource::getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill)
{
    auto* buffer = bufferToFill.buffer;
    for (int channel = 0; channel < buffer->getNumChannels(); ++channel)
    {
        auto* data = buffer->getWritePointer(channel, bufferToFill.startSample);

        for (int i = 0; i < bufferToFill.numSamples; ++i)
        
            data[i] = juce::Random::getSystemRandom().nextFloat() * amplitude*2 - amplitude;
    }
}
void Model::WhiteNoiseAudioSource::releaseResources()
{
    std::cout <<"Release" << std::endl;
}

void Model::save(String path, int sampleRate)
{
    whiteNoiseSource.amplitude = powf(10,amplitude/20);

    juce::File file(path);

    if (file.exists())
        file.deleteFile();

    juce::WavAudioFormat format;

    std::unique_ptr<juce::FileOutputStream> stream(file.createOutputStream());

    if (stream == nullptr)
        return;

    int blockSize = 512;
    int numChannels = 1;
    int bitsPerSample = 16;

    juce::AudioBuffer<float> buffer(numChannels, blockSize);

    int samplesGenerated = 0;
    int totalSamples = duration * sampleRate;

    std::unique_ptr<juce::AudioFormatWriter> writer(
        format.createWriterFor(stream.get(),
                               sampleRate,
                               numChannels,
                               bitsPerSample,
                               {},
                               0));
    if (writer == nullptr)
        return;

    stream.release(); // ownership donné au writer

    while (samplesGenerated < totalSamples)
    {
        int samplesThisBlock = std::min(blockSize, totalSamples - samplesGenerated);

        juce::AudioSourceChannelInfo info(&buffer, 0, samplesThisBlock);

        whiteNoiseSource.getNextAudioBlock(info);

        writer->writeFromAudioSampleBuffer(buffer, 0, samplesThisBlock);

        samplesGenerated += samplesThisBlock;
    }
    whiteNoiseSource.releaseResources();

}