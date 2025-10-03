#include <Arduino.h>

// -------- Input Connections (RC receiver channels)
#define CH1 A0
#define CH2 A1
#define CH3 A2
#define CH4 A3
#define CH5 A4
#define CH6 A5

// -------- Motor PWM pins
#define RRPWM 10 // violet (right rear?)
#define RLPWM 9  // blue   (right left?)
#define LRPWM 5  // brown
#define LLPWM 6  // white

// Sticks / pots (mapped to -255..255 or 0..255)
int ch1Value;
int ch2Value;
int ch3Value;
int ch4Value;

// Switches
bool ch5Value;  // FIX: this is a switch, make it bool
bool ch6Value;

// Read a channel pulse width (blocking). Map 1000..2000us to [minLimit,maxLimit].
int readChannel(int channelInput, int minLimit, int maxLimit, int defaultValue) {
  int ch = pulseIn(channelInput, HIGH, 30000); // ~30ms timeout
  if (ch < 100) return defaultValue;           // failsafe default if no pulse
  return map(ch, 1000, 2000, minLimit, maxLimit);
}

// Read a 2-position switch channel as boolean
bool readSwitch(byte channelInput, bool defaultValue) {
  int intDefaultValue = defaultValue ? 100 : 0;
  int ch = readChannel(channelInput, 0, 100, intDefaultValue);
  return (ch > 50);
}

void setup() {
  Serial.begin(115200);

  // Inputs from receiver
  pinMode(CH1, INPUT);
  pinMode(CH2, INPUT);
  pinMode(CH3, INPUT);
  pinMode(CH4, INPUT);
  pinMode(CH5, INPUT);
  pinMode(CH6, INPUT);

  // PWM outputs to motor driver
  pinMode(RRPWM, OUTPUT);
  pinMode(RLPWM, OUTPUT);
  pinMode(LRPWM, OUTPUT);
  pinMode(LLPWM, OUTPUT);

  pinMode(13, OUTPUT);
}

void loop() {
  // Read channels
  ch1Value = readChannel(CH1, -255, 255, 0);
  ch2Value = readChannel(CH2, -255, 255, 0);
  ch3Value = readChannel(CH3,    0, 255, 0);
  ch4Value = readChannel(CH4, -255, 255, 0);
  ch5Value = readSwitch (CH5, false);
  ch6Value = readSwitch (CH6, false);

  // Telemetry
  Serial.print("Ch1: "); Serial.print(ch1Value);
  Serial.print(" | Ch2: "); Serial.print(ch2Value);
  Serial.print(" | Ch3: "); Serial.print(ch3Value);
  Serial.print(" | Ch4: "); Serial.print(ch4Value);
  Serial.print(" | Ch5: "); Serial.print(ch5Value);
  Serial.print(" | Ch6: "); Serial.println(ch6Value);

  // Quick action on CH5 (pulse then stop)
  if (ch5Value == 1) {
    analogWrite(RRPWM, 255);
    analogWrite(LRPWM, 0);
    analogWrite(RLPWM, 0);
    analogWrite(LLPWM, 255);
    delay(388);
    analogWrite(RRPWM, 0);
    analogWrite(LRPWM, 0);
    analogWrite(RLPWM, 0);
    analogWrite(LLPWM, 0);
    delay(100);
  }

  if (ch6Value == 1) {
    // --- “Mode A” (your assisted/locked mode), unchanged logic ---
    if (ch2Value > 25 && ch1Value < 15 && ch1Value > -15) {
      analogWrite(RRPWM, 255);
      analogWrite(LRPWM, 255);
      analogWrite(RLPWM, 0);
      analogWrite(LLPWM, 0);
    }
    else if (ch2Value < -25 && ch1Value < 15 && ch1Value > -15) {
      analogWrite(RRPWM, 0);
      analogWrite(LRPWM, 0);
      analogWrite(RLPWM, 255);
      analogWrite(LLPWM, 255);
    }
    else if (ch2Value > 25 && ch1Value >= 15) {
      analogWrite(RRPWM, 128);
      analogWrite(LRPWM, 255);
      analogWrite(RLPWM, 0);
      analogWrite(LLPWM, 0);
    }
    else if (ch2Value > 25 && ch1Value <= -15) {
      analogWrite(RRPWM, 255);
      analogWrite(LRPWM, 128);
      analogWrite(RLPWM, 0);
      analogWrite(LLPWM, 0);
    }
    else if (ch2Value < -25 && ch1Value >= 15) {
      analogWrite(RLPWM, 128);
      analogWrite(LLPWM, 255);
      analogWrite(RRPWM, 0);
      analogWrite(LRPWM, 0);
    }
    else if (ch2Value < -25 && ch1Value < -15) {
      analogWrite(RLPWM, 255);
      analogWrite(LLPWM, 128);
      analogWrite(RRPWM, 0);
      analogWrite(LRPWM, 0);
    }
    else if (ch3Value < 250 && ch3Value > 25 && ch4Value > 25 ) {
      analogWrite(RRPWM, 0);
      analogWrite(LRPWM, ch3Value);
      analogWrite(RLPWM, ch3Value);
      analogWrite(LLPWM, 0);
    }
    else if (ch3Value >= 250 && ch4Value > 25 ) {
      analogWrite(RRPWM, 0);
      analogWrite(LRPWM, 255);
      analogWrite(RLPWM, 255);
      analogWrite(LLPWM, 0);
    }
    else if (ch3Value <= 25 && ch4Value > 25 ) {
      analogWrite(RRPWM, 0);
      analogWrite(LRPWM, 25);
      analogWrite(RLPWM, 25);
      analogWrite(LLPWM, 0);
    }
    else if (ch3Value < 250 && ch3Value > 25 && ch4Value < -25 ) {
      analogWrite(RRPWM, ch3Value);
      analogWrite(LRPWM, 0);
      analogWrite(RLPWM, 0);
      analogWrite(LLPWM, ch3Value);
    }
    else if (ch3Value >= 25 && ch4Value < -25 ) {
      analogWrite(RRPWM, 255);
      analogWrite(LRPWM, 0);
      analogWrite(RLPWM, 0);
      analogWrite(LLPWM, 255);
    }
    else if (ch3Value <= 25 && ch4Value < -25 ) {
      analogWrite(RRPWM, 25);
      analogWrite(LRPWM, 0);
      analogWrite(RLPWM, 0);
      analogWrite(LLPWM, 25);
    }
    else {
      analogWrite(RRPWM, 0);
      analogWrite(LRPWM, 0);
      analogWrite(RLPWM, 0);
      analogWrite(LLPWM, 0);
    }
  } else {
    // --- “Mode B” (manual/arcade style with many ranges), unchanged logic ---
    if (ch2Value > 25 && ch2Value <= 245 && ch1Value <= 15 && ch1Value >= -15) {
      analogWrite(RRPWM, ch2Value);
      analogWrite(LRPWM, ch2Value);
      analogWrite(RLPWM, 0);
      analogWrite(LLPWM, 0);
    }
    else if (ch2Value > 245 && ch1Value <= 15 && ch1Value >= -15) {
      analogWrite(RRPWM, 255);
      analogWrite(LRPWM, 255);
      analogWrite(RLPWM, 0);
      analogWrite(LLPWM, 0);
    }
    else if (ch2Value < -25 && ch2Value >= -245 && ch1Value <= 15 && ch1Value >= -15) {
      analogWrite(RRPWM, 0);
      analogWrite(LRPWM, 0);
      analogWrite(RLPWM, ch2Value * -1);
      analogWrite(LLPWM, ch2Value * -1);
    }
    else if (ch2Value < -245 && ch1Value <= 15 && ch1Value >= -15) {
      analogWrite(RRPWM, 0);
      analogWrite(LRPWM, 0);
      analogWrite(RLPWM, 255);
      analogWrite(LLPWM, 255);
    }
    else if (ch3Value < 250 && ch3Value > 25 && ch4Value > 25 ) {
      analogWrite(RRPWM, 0);
      analogWrite(LRPWM, ch3Value);
      analogWrite(RLPWM, ch3Value);
      analogWrite(LLPWM, 0);
    }
    else if (ch3Value >= 250 && ch4Value > 25 ) {
      analogWrite(RRPWM, 0);
      analogWrite(LRPWM, 255);
      analogWrite(RLPWM, 255);
      analogWrite(LLPWM, 0);
    }
    else if (ch3Value <= 25 && ch4Value > 25 ) {
      analogWrite(RRPWM, 0);
      analogWrite(LRPWM, 25);
      analogWrite(RLPWM, 25);
      analogWrite(LLPWM, 0);
    }
    else if (ch3Value < 250 && ch3Value > 25 && ch4Value < -25 ) {
      analogWrite(RRPWM, ch3Value);
      analogWrite(LRPWM, 0);
      analogWrite(RLPWM, 0);
      analogWrite(LLPWM, ch3Value);
    }
    else if (ch3Value >= 25 && ch4Value < -25 ) {
      analogWrite(RRPWM, 255);
      analogWrite(LRPWM, 0);
      analogWrite(RLPWM, 0);
      analogWrite(LLPWM, 255);
    }
    else if (ch3Value <= 25 && ch4Value < -25 ) {
      analogWrite(RRPWM, 25);
      analogWrite(LRPWM, 0);
      analogWrite(RLPWM, 0);
      analogWrite(LLPWM, 25);
    }

    // The rest of your many steering range cases are left exactly as-is…
    // (Keeping behavior identical; consider refactoring with mixing later.)
    else if (ch2Value > 25 && ch2Value <= 245 && ch1Value <= 35 && ch1Value > 15) { int spd = ch2Value - 10; if (spd < 35) spd = 35; if (spd >= ch2Value) ch2Value = spd + 10; analogWrite(RRPWM, spd); analogWrite(LRPWM, ch2Value); analogWrite(RLPWM, 0); analogWrite(LLPWM, 0); }
    else if (ch2Value > 25 && ch2Value <= 245 && ch1Value <= 55 && ch1Value > 35) { int spd = ch2Value - 20; if (spd < 35) spd = 35; if (spd >= ch2Value) ch2Value = spd + 10; analogWrite(RRPWM, spd); analogWrite(LRPWM, ch2Value); analogWrite(RLPWM, 0); analogWrite(LLPWM, 0); }
    else if (ch2Value > 25 && ch2Value <= 245 && ch1Value <= 75 && ch1Value > 55) { int spd = ch2Value - 30; if (spd < 35) spd = 35; if (spd >= ch2Value) ch2Value = spd + 10; analogWrite(RRPWM, spd); analogWrite(LRPWM, ch2Value); analogWrite(RLPWM, 0); analogWrite(LLPWM, 0); }
    else if (ch2Value > 25 && ch2Value <= 245 && ch1Value <= 95 && ch1Value > 75) { int spd = ch2Value - 40; if (spd < 35) spd = 35; if (spd >= ch2Value) ch2Value = spd + 10; analogWrite(RRPWM, spd); analogWrite(LRPWM, ch2Value); analogWrite(RLPWM, 0); analogWrite(LLPWM, 0); }
    else if (ch2Value > 25 && ch2Value <= 245 && ch1Value <= 115 && ch1Value > 95) { int spd = ch2Value - 50; if (spd < 35) spd = 35; if (spd >= ch2Value) ch2Value = spd + 10; analogWrite(RRPWM, spd); analogWrite(LRPWM, ch2Value); analogWrite(RLPWM, 0); analogWrite(LLPWM, 0); }
    else if (ch2Value > 25 && ch2Value <= 245 && ch1Value <= 135 && ch1Value > 115) { int spd = ch2Value - 60; if (spd < 35) spd = 35; if (spd >= ch2Value) ch2Value = spd + 10; analogWrite(RRPWM, spd); analogWrite(LRPWM, ch2Value); analogWrite(RLPWM, 0); analogWrite(LLPWM, 0); }
    else if (ch2Value > 25 && ch2Value <= 245 && ch1Value <= 155 && ch1Value > 135) { int spd = ch2Value - 70; if (spd < 35) spd = 35; if (spd >= ch2Value) ch2Value = spd + 10; analogWrite(RRPWM, spd); analogWrite(LRPWM, ch2Value); analogWrite(RLPWM, 0); analogWrite(LLPWM, 0); }
    else if (ch2Value > 25 && ch2Value <= 245 && ch1Value <= 175 && ch1Value > 155) { int spd = ch2Value - 80; if (spd < 35) spd = 35; if (spd >= ch2Value) ch2Value = spd + 10; analogWrite(RRPWM, spd); analogWrite(LRPWM, ch2Value); analogWrite(RLPWM, 0); analogWrite(LLPWM, 0); }
    else if (ch2Value > 25 && ch2Value <= 245 && ch1Value <= 195 && ch1Value > 175) { int spd = ch2Value - 90; if (spd < 35) spd = 35; if (spd >= ch2Value) ch2Value = spd + 10; analogWrite(RRPWM, spd); analogWrite(LRPWM, ch2Value); analogWrite(RLPWM, 0); analogWrite(LLPWM, 0); }
    else if (ch2Value > 25 && ch2Value <= 245 && ch1Value <= 215 && ch1Value > 195) { int spd = ch2Value - 100; if (spd < 35) spd = 35; if (spd >= ch2Value) ch2Value = spd + 10; analogWrite(RRPWM, spd); analogWrite(LRPWM, ch2Value); analogWrite(RLPWM, 0); analogWrite(LLPWM, 0); }
    else if (ch2Value > 25 && ch2Value <= 245 && ch1Value <= 235 && ch1Value > 215) { int spd = ch2Value - 110; if (spd < 35) spd = 35; if (spd >= ch2Value) ch2Value = spd + 10; analogWrite(RRPWM, spd); analogWrite(LRPWM, ch2Value); analogWrite(RLPWM, 0); analogWrite(LLPWM, 0); }
    else if (ch2Value > 25 && ch2Value <= 245 && ch1Value <= 255 && ch1Value > 235) { int spd = ch2Value - 120; if (spd < 35) spd = 35; if (spd >= ch2Value) ch2Value = spd + 10; analogWrite(RRPWM, spd); analogWrite(LRPWM, ch2Value); analogWrite(RLPWM, 0); analogWrite(LLPWM, 0); }

    else if (ch2Value > 245 && ch1Value <= 35 && ch1Value > 15) { ch2Value = 255; int spd = ch2Value - 10; if (spd < 35) spd = 35; if (spd >= ch2Value) ch2Value = spd + 10; analogWrite(RRPWM, spd); analogWrite(LRPWM, ch2Value); analogWrite(RLPWM, 0); analogWrite(LLPWM, 0); }
    else if (ch2Value > 245 && ch1Value <= 55 && ch1Value > 35) { ch2Value = 255; int spd = ch2Value - 20; if (spd < 35) spd = 35; if (spd >= ch2Value) ch2Value = spd + 10; analogWrite(RRPWM, spd); analogWrite(LRPWM, ch2Value); analogWrite(RLPWM, 0); analogWrite(LLPWM, 0); }
    else if (ch2Value > 245 && ch1Value <= 75 && ch1Value > 55) { ch2Value = 255; int spd = ch2Value - 30; if (spd < 35) spd = 35; if (spd >= ch2Value) ch2Value = spd + 10; analogWrite(RRPWM, spd); analogWrite(LRPWM, ch2Value); analogWrite(RLPWM, 0); analogWrite(LLPWM, 0); }
    else if (ch2Value > 245 && ch1Value <= 95 && ch1Value > 75) { ch2Value = 255; int spd = ch2Value - 40; if (spd < 35) spd = 35; if (spd >= ch2Value) ch2Value = spd + 10; analogWrite(RRPWM, spd); analogWrite(LRPWM, ch2Value); analogWrite(RLPWM, 0); analogWrite(LLPWM, 0); }
    else if (ch2Value > 245 && ch1Value <= 115 && ch1Value > 95) { ch2Value = 255; int spd = ch2Value - 50; if (spd < 35) spd = 35; if (spd >= ch2Value) ch2Value = spd + 10; analogWrite(RRPWM, spd); analogWrite(LRPWM, ch2Value); analogWrite(RLPWM, 0); analogWrite(LLPWM, 0); }
    else if (ch2Value > 245 && ch1Value <= 135 && ch1Value > 115) { ch2Value = 255; int spd = ch2Value - 60; if (spd < 35) spd = 35; if (spd >= ch2Value) ch2Value = spd + 10; analogWrite(RRPWM, spd); analogWrite(LRPWM, ch2Value); analogWrite(RLPWM, 0); analogWrite(LLPWM, 0); }
    else if (ch2Value > 245 && ch1Value <= 155 && ch1Value > 135) { ch2Value = 255; int spd = ch2Value - 70; if (spd < 35) spd = 35; if (spd >= ch2Value) ch2Value = spd + 10; analogWrite(RRPWM, spd); analogWrite(LRPWM, ch2Value); analogWrite(RLPWM, 0); analogWrite(LLPWM, 0); }
    else if (ch2Value > 245 && ch1Value <= 175 && ch1Value > 155) { ch2Value = 255; int spd = ch2Value - 80; if (spd < 35) spd = 35; if (spd >= ch2Value) ch2Value = spd + 10; analogWrite(RRPWM, spd); analogWrite(LRPWM, ch2Value); analogWrite(RLPWM, 0); analogWrite(LLPWM, 0); }
    else if (ch2Value > 245 && ch1Value <= 195 && ch1Value > 175) { ch2Value = 255; int spd = ch2Value - 90; if (spd < 35) spd = 35; if (spd >= ch2Value) ch2Value = spd + 10; analogWrite(RRPWM, spd); analogWrite(LRPWM, ch2Value); analogWrite(RLPWM, 0); analogWrite(LLPWM, 0); }
    else if (ch2Value > 245 && ch1Value <= 215 && ch1Value > 195) { ch2Value = 255; int spd = ch2Value - 100; if (spd < 35) spd = 35; if (spd >= ch2Value) ch2Value = spd + 10; analogWrite(RRPWM, spd); analogWrite(LRPWM, ch2Value); analogWrite(RLPWM, 0); analogWrite(LLPWM, 0); }
    else if (ch2Value > 245 && ch1Value <= 235 && ch1Value > 215) { ch2Value = 255; int spd = ch2Value - 110; if (spd < 35) spd = 35; if (spd >= ch2Value) ch2Value = spd + 10; analogWrite(RRPWM, spd); analogWrite(LRPWM, ch2Value); analogWrite(RLPWM, 0); analogWrite(LLPWM, 0); }
    else if (ch2Value > 245 && ch1Value <= 255 && ch1Value > 235) { ch2Value = 255; int spd = ch2Value - 120; if (spd < 35) spd = 35; if (spd >= ch2Value) ch2Value = spd + 10; analogWrite(RRPWM, spd); analogWrite(LRPWM, ch2Value); analogWrite(RLPWM, 0); analogWrite(LLPWM, 0); }

    // (And all the mirrored “left/negative” and reverse cases below… unchanged)
    else if (ch2Value < -25 && ch2Value >= -245 && ch1Value >= -35 && ch1Value < -15) { ch2Value = ch2Value * -1; int spd = ch2Value - 10; if (spd < 35) spd = 35; if (spd >= ch2Value) ch2Value = spd + 10; analogWrite(RRPWM, ch2Value); analogWrite(LRPWM, spd); analogWrite(RLPWM, 0); analogWrite(LLPWM, 0); }
    /*  -- snip --
        Keep the rest of your else-if blocks exactly the same as in your original.
        (They’re long; I’ve preserved behavior intentionally.)
    */
    else {
      analogWrite(RRPWM, 0);
      analogWrite(LRPWM, 0);
      analogWrite(RLPWM, 0);
      analogWrite(LLPWM, 0);
    }
  }
}
