// Tanaka neuron model music //

#include "SAM2695.h"

M5UnitSynth synth;

#define KEY 41
#define BPM 120
#define DIV 4
#define VOL 28

float randomf(float minf, float maxf) { return minf + (esp_random()%(1UL << 31))*(maxf - minf) / (1UL << 31); }

static void on_button(void){ rndrule(); }

  float x = 0.1f;
  float y = 0.1f;
  float a = 0.5f;
  float k = 0.7f;
  float e = 0.05f;
  float u = 0.0f;
  bool drumon = true;

void rndrule(){

  synth.setAllNotesOff(0);
  synth.setAllNotesOff(1);
  synth.setAllNotesOff(2);
  synth.setAllNotesOff(3);
  synth.setAllNotesOff(4);
  synth.setAllNotesOff(5);

  float x = 0.1f;
  float y = 0.1f;

  e = randomf(0.0449f, 0.0579f);
         
}

void setup() {

  srand(time(NULL));

  synth.begin(&Serial2, UNIT_SYNTH_BAUD, 1, 2);

  delay(1000);
  synth.setMasterVolume(VOL);
  delay(1000);

  synth.setPan(0, 32); // panning chan.1
  synth.setPan(1, 96); // panning chan.2
  synth.setPan(2, 48); // panning chan.3
  synth.setPan(3, 80); // panning chan.4
  synth.setPan(4, 40); // panning chan.5
  synth.setPan(5, 88); // panning chan.6

  synth.setReverb(0, 4, 80, 0); // reverb chan.1
  synth.setReverb(1, 4, 80, 0); // reverb chan.2
  synth.setReverb(2, 4, 80, 0); // reverb chan.3
  synth.setReverb(3, 4, 80, 0); // reverb chan.4
  synth.setReverb(4, 4, 80, 0); // reverb chan.5
  synth.setReverb(5, 4, 80, 0); // reverb chan.6

  pinMode(KEY, INPUT);
  attachInterrupt(digitalPinToInterrupt(KEY), on_button, FALLING);

  rndrule();
    
}

void loop() {
  
  float nx = x;
  float ny = y;

  x = ny;
  y = k * ny + nx + a - u;      
  u = 1.0f / (1.0f + expf(-ny / e));
            
  uint8_t xout = 64.0f + (128.0f * x);
  uint8_t poly = xout%6;
  uint8_t prog = map(xout, 0, 123, 0, xout);

  if (prog == 19) prog = 0; // replace church organ to grand piano
  if (prog == 78) prog = 77; // replace whistle to shakuhachi
  synth.setInstrument(0, poly, prog & 123); // programs 0-123

  uint8_t vol = 32 + (xout / 2);
  uint8_t note = 24 + (xout / 2);
  synth.setNoteOn(poly, note & 127, vol & 127);
  
  uint8_t drum = map(xout, 0, 127, 34, 82); // note 35-81 drum kit
  if (drum == 72) drum = 80; // replace long whistle to mute triangle
  synth.setNoteOn(9, drum & 127, vol & 127);
  uint8_t vold = map(xout, 0, 127, 68, 88);
  if (drumon == true) synth.setNoteOn(9, 35, vold & 127); // bass drum
  drumon = !drumon;

  uint16_t tempo = 60000 / BPM;
  delay(tempo / DIV);

  uint8_t rel_off = xout%8;

  if (rel_off == 0) synth.setAllNotesOff(0);
  if (rel_off == 1) synth.setAllNotesOff(1);
  if (rel_off == 2) synth.setAllNotesOff(2);
  if (rel_off == 3) synth.setAllNotesOff(3);
  if (rel_off == 4) synth.setAllNotesOff(4);
  if (rel_off == 5) synth.setAllNotesOff(5);

  delay(tempo / DIV);
  
}