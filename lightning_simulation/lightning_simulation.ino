#include <Wire.h>

/*
 * Copyright (c) 2015 Molly Nicholas
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted
 * (subject to the limitations in the disclaimer below) provided that the following conditions are
 * met:
 *
 * Redistributions of source code must retain the above copyright notice, this list of conditions
 * and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions
 * and the following disclaimer in the documentation and/or other materials provided with the
 * distribution.
 *
 * Neither the name of Molly Nicholas nor the names of its contributors may be used to
 * endorse or promote products derived from this software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE. THIS
 * SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <Adafruit_NeoPixel.h>

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
int NUM_LEDS = 45;
int LED_PIN = 6;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

//const int HIGH_STRIKE_LIKELIHOOD = 3;
//const int LOW_STRIKE_LIKELIHOOD = 10;
//const int commFromAutopress = 9;
int currentDataPoint = 0;
int chance = 0;
int startTime = -1;
// Simple moving average plot
int NUM_Y_VALUES = 17;

float yValues[] = {
  0,
  7,
  10,
  9,
  7.1,
  7.5,
  7.4,
  12,
  15,
  10,
  0,
  3,
  3.5,
  4,
  1,
  7,
  1
};

float simple_moving_average_previous = 0;
float random_moving_average_previous = 0;

float (*functionPtrs[10])(); //the array of function pointers
int NUM_FUNCTIONS = 2; // used to be 2
int door_closed = 0;

void setup() {
  Serial.begin(9600);
  // Neopixel setup
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  // initializes the array of function pointers.
  functionPtrs[0] = simple_moving_average;
  functionPtrs[1] = random_moving_average;
  startTime = millis();
//  pinMode(commFromAutopress, INPUT_PULLUP);

  Wire.begin(9);
  Serial.println("Opened wire");
  Wire.onReceive(receiveEvent);
}

void receiveEvent(int bytes) {
  Serial.println("received byte from wire");
  door_closed = Wire.read(); // read one char from I2C
  Serial.println(door_closed);
}

void loop() {
  if (door_closed == 1) { // door closed, lightning on!
    Serial.println("Door closed");
    if (startTime == -1) {
      startTime = millis();
    }
    if (random(chance) <= 2) {
      Serial.println("lightning strike");
      int led = random(NUM_LEDS);
      for (int i = 0; i < 10; i++) {
        // Use this line to keep the lightning focused in one LED.
        if (random(2) == 0) {
          // double strike
        } else {
          // lightningStrike(led):
          // Use this line if you want the lightning to spread out among multiple LEDs.
          lightningStrike(random(NUM_LEDS));
        }
      }
      chance = 2;
      // Once there's been one strike, I make it more likely that there will be a second.
      // int min_elapsed = (int) ((millis() - startTime) / 60000.);
      // chance = (int) map(min_elapsed, 0, 15, 15, 1);
//      Serial.print("chance = ");
//      Serial.println(chance);
    }
    turnAllPixelsOff();     
  }
  delay(1000);
}

void turnAllPixelsOff() {
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, 0);
  }
  strip.show();
}

void lightningStrike(int pixel) {
  float brightness = callFunction(random(NUM_FUNCTIONS));
  float scaledWhite = abs(brightness*500);
  
  strip.setPixelColor(pixel, strip.Color(scaledWhite, scaledWhite, scaledWhite));
  strip.show();
  delay(random(5, 100));
  currentDataPoint++;
  currentDataPoint = currentDataPoint%NUM_Y_VALUES;
}

float callFunction(int index) {
  return (*functionPtrs[index])(); //calls the function at the index of `index` in the array
}

// https://en.wikipedia.org/wiki/Moving_average#Simple_moving_average
float simple_moving_average() {
  uint32_t startingValue = currentDataPoint;
  uint32_t endingValue = (currentDataPoint+1)%NUM_Y_VALUES;
  float simple_moving_average_current = simple_moving_average_previous + 
                                  (yValues[startingValue])/NUM_Y_VALUES - 
                                  (yValues[endingValue])/NUM_Y_VALUES;

  simple_moving_average_previous = simple_moving_average_current;
  return simple_moving_average_current;
}


// Same as simple moving average, but with randomly-generated data points.
float random_moving_average() {
  float firstValue = random(1, 10);
  float secondValue = random(1, 10);
  float random_moving_average_current = random_moving_average_previous +
                                  firstValue/NUM_Y_VALUES -
                                  secondValue/NUM_Y_VALUES;
  random_moving_average_previous = random_moving_average_current;

  return random_moving_average_current;
}
