#include "daisy_seed.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

// Declare a DaisySeed object called hardware
DaisySeed hardware;

// Constants
const int sample_rate = 48000;  // Sample rate in Hz

// Audio buffers for left and right channels
float left_in, right_in;
float left_out, right_out;  // Output channels

// Chorus effect object
Chorus chorus;

// Potentiometer connected to pin 22 (A7)
const dsy_gpio_pin pot_pin = {DSY_GPIOB, 1};

void AudioCallback(const float *const *in, float **out, size_t size)
{
    float pot_value = hardware.adc.GetFloat(0);  // Read potentiometer value
    float depth = pot_value;  // Map pot value to depth (0.0 to 1.0)

    chorus.SetLfoDepth(depth);

    // Process each sample in the buffer
    for (size_t i = 0; i < size; i++)
    {
        // Read stereo input
        left_in = in[0][i];
        right_in = in[1][i];

        // Apply chorus to the input
        left_out = chorus.Process(left_in);
        right_out = chorus.Process(right_in);

        // Output the processed signal
        out[0][i] = left_out;
        out[1][i] = right_out;
    }
}

int main(void)
{
    // Configure and Initialize the Daisy Seed
    hardware.Configure();
    hardware.Init();

    // Set up the ADC for reading potentiometer values
    AdcChannelConfig adc_cfg;
    adc_cfg.InitSingle(pot_pin);
    hardware.adc.Init(&adc_cfg, 1);
    hardware.adc.Start();

    // Initialize the chorus effect
    chorus.Init(sample_rate);
    chorus.SetLfoFreq(0.3);  // Set rate of the chorus modulation
    chorus.SetLfoDepth(0.5); // Set initial depth of the modulation

    // Start audio callback
    hardware.StartAudio(AudioCallback);

    // Loop forever (the audio processing is handled in the callback)
    for(;;) {}
}