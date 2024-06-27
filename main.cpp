
#include <DaisySeed.h>
#include <DaisySP.h>


// Define the DaisySeed and DaisySP objects
DaisySeed seed;
DaisySP sp;
const int sample_rate = 48000;  // Sample rate in Hz


// Audio buffers for left and right channels
float left_in, right_in;
float left_out, right_out;


// Reverb object
dsy_reverb_s reverb;
const size_t reverb_buf_size = 8192;
float reverb_buf[reverb_buf_size];


// Potentiometer connected to pin 22 (A7)
const float pot_scale = 1.0 / 4096.0; // Scale factor for potentiometer value


void AudioCallback(float **in, float **out, size_t size)
{
   float dry_wet = seed.GetPin(22).Process();  // Read potentiometer value (0 to 4095)
   dry_wet *= pot_scale;  // Scale potentiometer value to 0.0 to 1.0


   // Process each sample in the buffer
   for (size_t i = 0; i < size; i++)
   {
       // Read stereo input
       left_in = in[0][i];
       right_in = in[1][i];


       // Apply reverb to the wet channel
       sp.Process(reverb, &left_in, &right_in, &left_out, &right_out, 1);


       // Mix dry and wet channels based on potentiometer value
       left_out = (1.0 - dry_wet) * left_in + dry_wet * left_out;
       right_out = (1.0 - dry_wet) * right_in + dry_wet * right_out;


       // Output stereo channels
       out[0][i] = left_out;
       out[1][i] = right_out;
   }
}


void setup()
{
   // Initialize Daisy Seed
   seed.Configure();
   seed.Init();
  
   // Initialize DaisySP
   sp.Init(sample_rate);


   // Initialize reverb effect
   sp.InitReverb(&reverb, reverb_buf, reverb_buf_size);
   reverb.SetRoomSize(0.8);
   reverb.SetDamping(0.5);
   reverb.SetBandwidth(0.5);
   reverb.SetWetLevel(0.5);
   reverb.SetDryLevel(1.0);


   // Start audio
   seed.StartAudio(AudioCallback);
}


void loop()
{
   // Main loop - nothing needed here as all processing is handled in AudioCallback
}