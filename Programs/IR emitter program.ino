// =====================================================================
// IR EMITTER (operator's beacon)
// Board: Seeed XIAO ESP32-C6 (select "XIAO_ESP32C6" in the ESP32 package by Espressif)
// Module: Crowtail IR Emitter 2.0 (pins: GND, VCC, W1, SIG)
// Sends a 38 kHz modulated infrared signal in short bursts, which is
// what the TSOP38238 receivers on the robot are tuned to see.
// =====================================================================

// ---------- Pin ----------
// GPIO 20 is the pad labeled D9 on the XIAO ESP32-C6.
// Connect the module's SIG pin here. W1 stays unconnected.
const int EmitterPin = 20;

// ---------- Signal settings ----------
const int CarrierFrequencyHz = 38000;   // Must match the TSOP38238 (38 kHz)
const int PwmResolutionBits = 8;        // Duty value range is 0-255
const int CarrierDuty = 128;            // 50% duty (128 of 255) while a burst is ON

// ---------- Burst timing ----------
// The TSOP38238 lowers its sensitivity if it receives a constant 38 kHz
// signal for a long time, so we send short bursts with short gaps.
const unsigned long BurstOnUs = 600;    // (us) How long the 38 kHz signal is ON
const unsigned long BurstOffUs = 600;   // (us) How long the signal is OFF between bursts

void setup()
{
  Serial.begin(115200);

  // Set up the PWM carrier on the emitter pin (ESP32 Arduino core 3.x)
  ledcAttach(EmitterPin, CarrierFrequencyHz, PwmResolutionBits);
  ledcWrite(EmitterPin, 0);   // Start with the emitter off

  Serial.println("IR emitter running: 38 kHz bursts on GPIO 20");
}

void loop()
{
  ledcWrite(EmitterPin, CarrierDuty);   // Burst ON (38 kHz)
  delayMicroseconds(BurstOnUs);

  ledcWrite(EmitterPin, 0);             // Burst OFF
  delayMicroseconds(BurstOffUs);
}
