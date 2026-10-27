// =====================================================================
// ENCODER MOVEMENT TEST SEQUENCE
// Runs this sequence, then pauses and repeats:
//   1. Drive forward 20 cm
//   2. Drive backward 20 cm
//   3. Turn right 90 degrees
//   4. Turn left 90 degrees
//   5. Spin right 360 degrees (easy to judge by eye, used for calibration)
// All movements count encoder ticks instead of using fixed delays.
// Each movement stops when EITHER side reaches the target ticks,
// or when the timeout ends.
// =====================================================================

// ---------- Motor pins (placeholders, update after wiring) ----------
const int RightForward = 9;     // PWM on this pin drives the right side forward
const int RightBackward = 10;   // PWM on this pin drives the right side backward
const int LeftForward = 11;     // PWM on this pin drives the left side forward
const int LeftBackward = 12;    // PWM on this pin drives the left side backward

// ---------- Encoder pins (placeholders, must be interrupt-capable) ----------
const int LeftEncoderPin = 18;
const int RightEncoderPin = 19;

// ---------- Robot measurements (MEASURE THESE ON THE REAL ROBOT) ----------
const float TrackWidthCm = 18.0;        // Distance between left and right wheels, center to center
const float WheelDiameterCm = 6.5;      // Wheel diameter from the parts list
const int TicksPerRevolution = 20;      // Slots on the encoder disk

// ---------- Calibration ----------
// If the 360 spin turns too little, make this number BIGGER.
// If it turns too much, make it SMALLER.
// Example: the spin only reaches 300 degrees -> 1.0 * 360 / 300 = 1.2
const float TurnCalibration = 1.0;

// ---------- Distances ----------
const float DriveDistanceCm = 20.0;     // How far to drive forward and backward

// ---------- Calculated values (computed from the numbers above) ----------
const float CmPerTick = (3.14159 * WheelDiameterCm) / TicksPerRevolution;   // About 1.02 cm

// Ticks each wheel needs for a full 360 degree spin:
// each wheel travels the full circle, with the track width as the diameter
const long Turn360Ticks = ((3.14159 * TrackWidthCm) / CmPerTick) * TurnCalibration;

// A 90 degree turn is a quarter of the full spin
const long Turn90Ticks = Turn360Ticks / 4;

// Ticks each wheel needs to drive the set distance in a straight line
const long DriveTicks = DriveDistanceCm / CmPerTick;                        // About 19 ticks

// ---------- Tuning ----------
const int TurnPower = 150;                  // PWM value (0-255) while turning
const int DrivePower = 150;                 // PWM value (0-255) while driving
const unsigned long TimeoutMs = 5000;       // Safety stop if the encoders never reach the target
const int SettleMs = 500;                   // Short pause between movements so the robot stops coasting
const int PauseMs = 4000;                   // Pause at the end of the sequence (time to check the result)

// ---------- Encoder counters ----------
// volatile = the interrupt can change these at any moment,
// so the compiler must always read the real value
volatile long LeftTicks = 0;
volatile long RightTicks = 0;

// ---------- Interrupt functions ----------
// These run automatically every time an encoder slot passes the sensor.
// IRAM_ATTR keeps them in fast memory so the ESP32 never misses a tick.
// NOTE: a single-channel encoder cannot tell direction, so we just count up.
void IRAM_ATTR CountLeftTick()
{
  LeftTicks++;
}

void IRAM_ATTR CountRightTick()
{
  RightTicks++;
}

void setup()
{
  Serial.begin(115200);

  pinMode(RightForward, OUTPUT);
  pinMode(RightBackward, OUTPUT);
  pinMode(LeftForward, OUTPUT);
  pinMode(LeftBackward, OUTPUT);

  pinMode(LeftEncoderPin, INPUT_PULLUP);
  pinMode(RightEncoderPin, INPUT_PULLUP);

  // Run the counting functions on every RISING edge of the encoder signal
  attachInterrupt(digitalPinToInterrupt(LeftEncoderPin), CountLeftTick, RISING);
  attachInterrupt(digitalPinToInterrupt(RightEncoderPin), CountRightTick, RISING);

  StopMotors();

  Serial.print("Target ticks for 360 degree spin: ");
  Serial.println(Turn360Ticks);
  Serial.print("Target ticks for 90 degree turn: ");
  Serial.println(Turn90Ticks);
  Serial.print("Target ticks for the drive: ");
  Serial.println(DriveTicks);

  // Time to place the robot down
  delay(5000);
}

void loop()
{
  Serial.println("Driving forward");
  DriveForwardByTicks(DriveTicks);
  delay(SettleMs);

  Serial.println("Driving backward");
  DriveBackwardByTicks(DriveTicks);
  delay(SettleMs);

  Serial.println("Turning right 90");
  TurnRightByTicks(Turn90Ticks);
  delay(SettleMs);

  Serial.println("Turning left 90");
  TurnLeftByTicks(Turn90Ticks);
  delay(SettleMs);

  Serial.println("Spinning right 360");
  TurnRightByTicks(Turn360Ticks);

  Serial.println("Sequence done, check the result");
  delay(PauseMs);
}

// Drive straight forward until a side reaches the target ticks, or the timeout ends
void DriveForwardByTicks(long targetTicks)
{
  ResetTicks();
  StopMotors();
  analogWrite(LeftForward, DrivePower);
  analogWrite(RightForward, DrivePower);
  WaitForTicks(targetTicks);
  StopMotors();
  PrintResult();
}

// Drive straight backward until a side reaches the target ticks, or the timeout ends
void DriveBackwardByTicks(long targetTicks)
{
  ResetTicks();
  StopMotors();
  analogWrite(LeftBackward, DrivePower);
  analogWrite(RightBackward, DrivePower);
  WaitForTicks(targetTicks);
  StopMotors();
  PrintResult();
}

// Turn right until a side reaches the target ticks, or the timeout ends
void TurnRightByTicks(long targetTicks)
{
  ResetTicks();
  StopMotors();
  analogWrite(LeftForward, TurnPower);
  analogWrite(RightBackward, TurnPower);
  WaitForTicks(targetTicks);
  StopMotors();
  PrintResult();
}

// Turn left until a side reaches the target ticks, or the timeout ends
void TurnLeftByTicks(long targetTicks)
{
  ResetTicks();
  StopMotors();
  analogWrite(LeftBackward, TurnPower);
  analogWrite(RightForward, TurnPower);
  WaitForTicks(targetTicks);
  StopMotors();
  PrintResult();
}

// Waits here until EITHER encoder reaches the target, or the timeout runs out.
// The motors are already running when this is called.
void WaitForTicks(long targetTicks)
{
  unsigned long startTime = millis();

  while (true)
  {
    // Copy the counts safely: stop interrupts for a moment while reading
    noInterrupts();
    long left = LeftTicks;
    long right = RightTicks;
    interrupts();

    // First side to hit the target ends the movement
    if (left >= targetTicks || right >= targetTicks)
    {
      return;
    }

    // Safety: if nothing reaches the target in time, give up
    if (millis() - startTime >= TimeoutMs)
    {
      Serial.println("TIMEOUT - encoders did not reach the target");
      return;
    }
  }
}

// Sets both tick counters back to zero
void ResetTicks()
{
  noInterrupts();
  LeftTicks = 0;
  RightTicks = 0;
  interrupts();
}

// Prints how many ticks each side actually counted
void PrintResult()
{
  noInterrupts();
  long left = LeftTicks;
  long right = RightTicks;
  interrupts();

  Serial.print("Left ticks: ");
  Serial.print(left);
  Serial.print("  Right ticks: ");
  Serial.println(right);
}

// All four pins off
void StopMotors()
{
  analogWrite(LeftForward, 0);
  analogWrite(LeftBackward, 0);
  analogWrite(RightForward, 0);
  analogWrite(RightBackward, 0);
}
