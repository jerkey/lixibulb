#define BAUD_RATE 57600
char versionStr[] = "lixibulb 1.0"; // this is the text we say over the serial port when we boot up

#define DISPLAY_INTERVAL 1000 // how often to speak our status on the serial port

#include "sinewave.h" // unsigned char sinewave_data[sinewave_length]  sinewave_length=256

const int ledPins[6] = { 3, 5, 6, 9, 10, 11 };
#define NUM_LEDS 5 // Number of LED outputs used

const int ledPhase[NUM_LEDS] = { 0, ( 256 / 5 ) * 1,
                                    ( 256 / 5 ) * 2,
                                    ( 256 / 5 ) * 3,
                                    ( 256 / 5 ) * 4 };

int ledPWMval[6] = { 0, 0, 0, 0, 0, 0 }; // stores the last analogWrite() value for each LED
                                         // so we don't analogWrite unnecessarily!

unsigned long time, timeDisplay = 0;

/*void doKnob(){ // look in calcWatts() to see if this is commented out
  knobAdc = analogRead(KNOBPIN) - 10; // make sure not to add if knob is off
  if (knobAdc < 0) knobAdc = 0; // values 0-10 count as zero
}*/


void setup() {
  Serial.begin(BAUD_RATE);
  Serial.println(versionStr);

  for(int i = 0; i < NUM_LEDS; i++) {
    pinMode(ledPins[i],OUTPUT); // init LED pins as outputs
  }

  setPwmFrequency(3,1); // this sets the frequency of PWM on pins 3 and 11 to 31,250 Hz
  setPwmFrequency(6,1); // this sets the frequency of PWM on pins 5 and 6 to 62,500 Hz
  setPwmFrequency(9,1); // this sets the frequency of PWM on pins 9 and 10 to 31,250 Hz

  timeDisplay = millis();
}

void loop() {
  time = millis();
  doLeds();

  if(time - timeDisplay > DISPLAY_INTERVAL){
    Serial.println(time);
    timeDisplay = time;
  }

}

void doLeds() {
  static uint8_t phase = 0; // how far through the sinewave table is the first LED
  for(uint8_t which_led = 0; which_led < NUM_LEDS; which_led++) {
    uint8_t pwmVal = sinewave_data[ ( phase + ledPhase[ which_led ] ) % sinewave_length ]; // what's our desired LED brightness value
    if ( pwmVal != ledPWMval[ which_led ] ) {
      analogWrite( ledPins[ which_led ], pwmVal );
      ledPWMval = pwmVal;
    }
  }
}

void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch(divisor) {
    case 1: 
      mode = 0x01; 
      break;
    case 8: 
      mode = 0x02; 
      break;
    case 64: 
      mode = 0x03; 
      break;
    case 256: 
      mode = 0x04; 
      break;
    case 1024: 
      mode = 0x05; 
      break;
    default: 
      return;
    }
    if(pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } 
    else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } 
  else if(pin == 3 || pin == 11) {
    switch(divisor) {
    case 1: 
      mode = 0x01; 
      break;
    case 8: 
      mode = 0x02; 
      break;
    case 32: 
      mode = 0x03; 
      break;
    case 64: 
      mode = 0x04; 
      break;
    case 128: 
      mode = 0x05; 
      break;
    case 256: 
      mode = 0x06; 
      break;
    case 1024: 
      mode = 0x7; 
      break;
    default: 
      return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}
