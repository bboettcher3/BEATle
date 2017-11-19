/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "maximilian.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainContentComponent : public AudioAppComponent,
	public Button::Listener,
	public Slider::Listener,
	private MultiTimer
							   
{
public:
    //==============================================================================
    MainContentComponent()
    {
        setSize (800, 600);
		startTimer(0, 500);
		startTimer(1, 30);

		//init gui components
		submitButton.setButtonText("Submit");
		submitButton.setBounds(10, 70, 100, 30);
		submitButton.addListener(this);
		addAndMakeVisible(&submitButton);
		stepsSlider.setSliderStyle(Slider::LinearHorizontal);
		stepsSlider.setBounds(60, 10, 300, 20);
		stepsSlider.setRange(0, 15, 1);
		stepsSlider.addListener(this);
		addAndMakeVisible(&stepsSlider);
		stepsLabel.setText("Steps:", dontSendNotification);
		stepsLabel.setBounds(10, 0, 50, 50);
		stepsLabel.setFont(15.0f);
		addAndMakeVisible(&stepsLabel);
		beatsSlider.setSliderStyle(Slider::LinearHorizontal);
		beatsSlider.setBounds(60, 40, 300, 20);
		beatsSlider.setRange(0, 15, 1);
		beatsSlider.addListener(this);
		addAndMakeVisible(&beatsSlider);
		beatsLabel.setText("Beats:", dontSendNotification);
		beatsLabel.setBounds(10, 30, 50, 50);
		beatsLabel.setFont(15.0f);
		addAndMakeVisible(&beatsLabel);
		bpmSlider.setSliderStyle(Slider::LinearVertical);
		bpmSlider.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 40);
		bpmSlider.setBounds(400, 40, 50, 150);
		bpmSlider.setRange(80, 160, 1);
		bpmSlider.setValue(120, dontSendNotification);
		bpmSlider.addListener(this);
		addAndMakeVisible(&bpmSlider);
		bpmLabel.setText("BPM:", dontSendNotification);
		bpmLabel.setBounds(400, 10, 50, 50);
		bpmLabel.setFont(15.0f);
		addAndMakeVisible(&bpmLabel);
		result.setText("00000000", dontSendNotification);
		result.setBounds(10, 100, 200, 50);
		result.setFont(20.0f);
		addAndMakeVisible(&result);

		osc.phaseReset(0);
		env.setAttack(100);
		env.setDecay(300);
		env.setSustain(.5f);
		env.setRelease(20);

        // specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }

    ~MainContentComponent()
    {
        shutdownAudio();
    }

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override
    {
        // This function will be called when the audio device is started, or when
        // its settings (i.e. sample rate, block size, etc) are changed.

        // You can use this function to initialise any resources you might need,
        // but be careful - it will be called on the audio thread, not the GUI thread.

        // For more details, see the help for AudioProcessor::prepareToPlay()
    }

    void getNextAudioBlock (const AudioSourceChannelInfo& bufferToFill) override
    {
        // Your audio-processing code goes here!

        // For more details, see the help for AudioProcessor::getNextAudioBlock()

        // Right now we are not producing any data, in which case we need to clear the buffer
        // (to prevent the output of random noise)
		bufferToFill.buffer->clear();
		int numChannels = bufferToFill.buffer->getNumChannels();
		int numSamples = bufferToFill.buffer->getNumSamples();
		float currSampleVal = 0;
		int currSample = 0;
		while (--numSamples >= 0)
		{
			currSampleVal = 0;

			currSampleVal = osc.sawn(440) * env.adsr(1, env.trigger);
			

			for (int i = numChannels; --i >= 0;) {
				bufferToFill.buffer->addSample(i, currSample, currSampleVal);
			}
			currSample++;
		}		
		if (env.trigger == 1) {
			env.trigger = 0;
		}
    }

    void releaseResources() override
    {
        // This will be called when the audio device stops, or when it is being
        // restarted due to a setting change.

        // For more details, see the help for AudioProcessor::releaseResources()
    }

    //==============================================================================
    void paint (Graphics& g) override
    {
        // (Our component is opaque, so we must completely fill the background with a solid colour)
        g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
        // You can add your drawing code here!
    }

    void resized() override
    {
        // This is called when the MainContentComponent is resized.
        // If you add any child components, this is where you should
        // update their positions.
		
    }

	void buttonClicked(Button* button) override
	{
		if (button == &submitButton) {
			std::string resultText = euclidean((int)beatsSlider.getValue(), (int)stepsSlider.getValue());
			result.setText(resultText, dontSendNotification);
			currStep = 0;
		}
		
	}

	void sliderValueChanged(Slider* slider) override 
	{
		if (slider == &beatsSlider) {
			if (beatsSlider.getValue() > stepsSlider.getValue()) {
				beatsSlider.setValue(stepsSlider.getValue(), dontSendNotification);
			}
		}
		else if (slider == &bpmSlider) {
			int newMS = ((1 / (bpmSlider.getValue() / 60)) * 1000); //convert bpm to milliseconds
			startTimer(0, newMS);
		}
	}

private:
    //==============================================================================

    // Your private member variables go here...
	TextButton submitButton;
	Slider stepsSlider, beatsSlider;
	Label stepsLabel, beatsLabel;
	Slider bpmSlider;
	Label bpmLabel;
	int currStep = 0;
	Label result;

	maxiOsc osc;
	maxiEnv env;

	std::string euclidean(int beats, int steps)
	{
		//We can only have as many beats as we have steps (0 <= beats <= steps)
		if (beats > steps)
			beats = steps;

		//Each iteration is a process of pairing strings X and Y and the remainder from the pairings
		//X will hold the "dominant" pair (the pair that there are more of)
		std::string x = "1";
		int x_amount = beats;

		std::string y = "0";
		int y_amount = steps - beats;

		do
		{
			//Placeholder variables
			int x_temp = x_amount;
			int y_temp = y_amount;
			std::string y_copy = y;

			//Check which is the dominant pair 
			if (x_temp >= y_temp)
			{
				//Set the new number of pairs for X and Y
				x_amount = y_temp;
				y_amount = x_temp - y_temp;

				//The previous dominant pair becomes the new non dominant pair
				y = x;
			}
			else
			{
				x_amount = x_temp;
				y_amount = y_temp - x_temp;
			}

			//Create the new dominant pair by combining the previous pairs
			x = x + y_copy;
		} while (x_amount > 1 && y_amount > 1);//iterate as long as the non dominant pair can be paired (if there is 1 Y left, all we can do is pair it with however many Xs are left, so we're done)

											   //By this point, we have strings X and Y formed through a series of pairings of the initial strings "1" and "0"
											   //X is the final dominant pair and Y is the second to last dominant pair
		std::string rhythm;
		for (int i = 1; i <= x_amount; i++)
			rhythm += x;
		for (int i = 1; i <= y_amount; i++)
			rhythm += y;
		return rhythm;
	}

	void timerCallback(int timerID) override {
		if (timerID == 0) {
			if (currStep >= (result.getText().length())) {
				currStep = 0;
			}
			char currBeat = result.getText()[currStep];
			bool beat = atoi(&currBeat);
			env.amplitude = 0;
			if (beat) {
				env.trigger = 1;
			}
			else {
				env.trigger = 0;
			}

			currStep++;
		}
		else if (timerID == 1) {
			
		}
		
	}

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};


// (This function is called by the app startup code to create our main component)
Component* createMainContentComponent()     { return new MainContentComponent(); }
