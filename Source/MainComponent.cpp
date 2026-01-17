#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    setSize (800, 600);

    // Audio output only
    setAudioChannels (0, 2);

    // Register audio formats
    formatManager.registerBasicFormats();

    // Buttons
    addAndMakeVisible (playButton);
    addAndMakeVisible (stopButton);
    addAndMakeVisible (loadButton);

    
    playButton.addListener (this);
    stopButton.addListener (this);
    loadButton.addListener (this);

    // Volume slider
    volumeSlider.setRange (0.0, 1.0);
    volumeSlider.setValue (0.8);
    volumeSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    volumeSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 20);
    volumeSlider.addListener (this);
    addAndMakeVisible (volumeSlider);
    
    volumeSlider.setRange(0.0, 1.0);
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay (samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    if(!playing)
    {
    bufferToFill.clearActiveBufferRegion();
    return;
    }
    transportSource.getNextAudioBlock (bufferToFill);
}

void MainComponent::releaseResources()
{
    transportSource.releaseResources();
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);

    g.setColour (juce::Colours::green);
    g.setFont (24.0f);
    g.drawText ("JUCE Audio Player",
                getLocalBounds(),
                juce::Justification::centredTop,
                true);
}

void MainComponent::resized()
{
    auto area = getLocalBounds().reduced (20);
    auto rowHeight = 50;

    playButton.setBounds (area.removeFromTop (rowHeight));
    stopButton.setBounds (area.removeFromTop (rowHeight));
    volumeSlider.setBounds (area.removeFromTop (rowHeight + 20));
    loadButton.setBounds (area.removeFromTop (rowHeight));
}

//==============================================================================
void MainComponent::buttonClicked (juce::Button* button)
{
    if (button == &playButton)
        transportSource.start();

    if (button == &stopButton)
        transportSource.stop();

    if (button == &loadButton)
    {
        fileChooser.launchAsync (
            juce::FileBrowserComponent::canSelectFiles,
            [this] (const juce::FileChooser& chooser)
            {
                auto file = chooser.getResult();
                if (file.existsAsFile())
                    loadURL (juce::URL { file });
            });
    }
}

void MainComponent::sliderValueChanged (juce::Slider* slider)
{
    if (slider == &volumeSlider)
        transportSource.setGain (slider->getValue());
}

//==============================================================================
void MainComponent::loadURL (juce::URL audioURL)
{
    std::unique_ptr<juce::InputStream> stream (
        audioURL.createInputStream (false));

    if (stream == nullptr)
        return;

    auto* reader = formatManager.createReaderFor (std::move (stream));
    if (reader == nullptr)
        return;

    auto newSource =
        std::make_unique<juce::AudioFormatReaderSource> (reader, true);

    transportSource.setSource (newSource.get(), 0, nullptr, reader->sampleRate);
    readerSource.reset (newSource.release());
}













//#include "MainComponent.h"
//
////==============================================================================
//MainComponent::MainComponent()
//{
//    // Make sure you set the size of the component after
//    // you add any child components.
//    setSize (800, 600);
//
//    // Some platforms require permissions to open input channels so request that here
//    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
//        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
//    {
//        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
//                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
//    }
//    else
//    {
//        // Specify the number of input and output channels that we want to open
//        setAudioChannels (0, 2);
//    }
//    addAndMakeVisible(playButton);
//    addAndMakeVisible(stopButton);
//    addAndMakeVisible(loadButton);
//    addAndMakeVisible(volSlider);
//    playButton.addListener(this);
//    stopButton.addListener(this);
//    loadButton.addListener(this);
//    volSlider.addListener(this);
//}
//
//MainComponent::~MainComponent()
//{
//    // This shuts down the audio device and clears the audio source.
//    shutdownAudio();
//}
//
////==============================================================================
//void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
//{
//    // This function will be called when the audio device is started, or when
//    // its settings (i.e. sample rate, block size, etc) are changed.
//
//    // You can use this function to initialise any resources you might need,
//    // but be careful - it will be called on the audio thread, not the GUI thread.
//
//    // For more details, see the help for AudioProcessor::prepareToPlay()
//    phase = 0.0;
//    dphase = 0.0001;
//    formatManager.registerBasicFormats();
////    juce::URL audioURL{"file///home/matthew/aon_inspired.mp3"};
////
////    auto* reader = formatManager.createReaderFor(audioURL.createInputStream(juce::URL::InputStreamOptions{}));
////    if(reader != nullptr){
////        std::unique_ptr<juce::AudioFormatReaderSource> newSource(new juce::AudioFormatReader (reader, true));
////        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
////        readerSource.reset(newSource.release());
////        transportSource.start();
////    }
//    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
//
//}
//
//
//
//void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
//{
//    setAudioChannels(0, 2);
//    transportSource.setSource(nullptr);
//    // Your audio-processing code goes here!
//
//    // For more details, see the help for AudioProcessor::getNextAudioBlock()
//
//    // Right now we are not producing any data, in which case we need to clear the buffer
//    // (to prevent the output of random noise)
//    //    bufferToFill.clearActiveBufferRegion();
//    //    auto* leftChan = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
//    //    auto* rightChan = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
//    //    for(auto i=0; i< bufferToFill.numSamples; ++i){
//    ////        double sample = rand.nextDouble() * 0.25;
//    ////        double sample =fmod(phase, 0.2);
//    //        double sample = sin(phase) *0.1;
//    //        leftChan[i] = sample;
//    //        rightChan[i] = sample;
//    //        phase += dphase;
//    //    }
//    //}
//}
//
//
//
//
//
//
//
////void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
////{
////    // Your audio-processing code goes here!
////
////    // For more details, see the help for AudioProcessor::getNextAudioBlock()
////
////    // Right now we are not producing any data, in which case we need to clear the buffer
////    // (to prevent the output of random noise)
//////    bufferToFill.clearActiveBufferRegion();
////    auto* leftChan = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
////    auto* rightChan = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
////    for(auto i=0; i< bufferToFill.numSamples; ++i){
//////        double sample = rand.nextDouble() * 0.25;
//////        double sample =fmod(phase, 0.2);
////        double sample = sin(phase) *0.1;
////        leftChan[i] = sample;
////        rightChan[i] = sample;
////        phase += dphase;
////    }
////}
//
//void MainComponent::releaseResources()
//{
//    // This will be called when the audio device stops, or when it is being
//    // restarted due to a setting change.
//
//    // For more details, see the help for AudioProcessor::releaseResources()
//    transportSource.releaseResources();
//}
//
////==============================================================================
//void MainComponent::paint (juce::Graphics& g)
//{
//    // (Our component is opaque, so we must completely fill the background with a solid colour)
//    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
//
//    // You can add your drawing code here!
//    g.setColour (juce::Colour (0, 122, 61));
//    g.setFont (20.0f);
//    g.drawText ("Hello from Kenya", getLocalBounds(),
//                 juce::Justification::centred, true);
//
//}
//
//void MainComponent::resized()
//{
//    // This is called when the MainContentComponent is resized.
//    // If you add any child components, this is where you should
//    // update their positions.
//    double rowH = getHeight()/5;
//    playButton.setBounds(0,0,getWidth(), rowH);
//    stopButton.setBounds(0,rowH , getWidth(), rowH);
//    volSlider.setBounds(0,rowH *2,getWidth(), rowH);
//    loadButton.setBounds(0,rowH*3, getWidth(), rowH);
//}
//
//void MainComponent::buttonClicked(juce::Button* button){
//
//    if(button == &playButton){
//        std::cout <<"Play button was clicked" <<std::endl;
//        transportSource.start();
//    }
//    if(button == &stopButton){
//        std::cout <<"Stop button was clicked" <<std::endl;
//        transportSource.stop();
//    }
//    if (button == &loadButton)
//    {
//
//        // this does work in 6.1 but the syntax is a little funky
//        // https://docs.juce.com/master/classFileChooser.html#ac888983e4abdd8401ba7d6124ae64ff3
//        // - configure the dialogue
//        auto fileChooserFlags =
//        juce::FileBrowserComponent::canSelectFiles;
//        // - launch out of the main thread
//        // - note how we use a lambda function which you've probably
//        // not seen before. Please do not worry too much about that.
//        fChooser.launchAsync(fileChooserFlags, [this](const juce::FileChooser& chooser)
//        {
//            juce::File chosenFile = chooser.getResult();
//            loadURL(juce::URL{chosenFile});
//        });
//    }
//
//}
//
//void MainComponent::sliderValueChanged(juce::Slider *slider){
//    if(slider == &volSlider){
////        std::cout <<"vol slider moved " <<slider -> getValue() <<std::endl;
//        dphase = volSlider.getValue() * 0.01;
//    }
//}
//
//void MainComponent::loadURL(juce::URL loadURL)
//{
//    auto* reader = formatManager.createReaderFor(
//        loadURL.createInputStream(juce::URL::InputStreamOptions{})
//    );
//      if(reader != nullptr){
//          auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
//          transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
//          readerSource.reset(newSource.release());
//
//      }
//}
