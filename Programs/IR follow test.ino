// =====================================================================
// IR STEERING TEST (with gradual turn ramp-up)
// Two TSOP38238 receivers (digital, active LOW: LOW = signal seen).
//   Both see signal   -> drive forward
//   Only left sees    -> turn left until both see it
//   Only right sees   -> turn right until both see it
//   Neither sees      -> use the memory of where it was last seen
// Turns start gently and ramp up to full turn power over 2 seconds,
// so a future payload doesn't get jerked around.
// No ultrasonic sensors and no encoders in this test.
// =====================================================================

// ---------- Motor pins (same as the movement test) ----------
const int RightForward = 25;    // PWM on this pin drives the right side forward
const int RightBackward = 26;   // PWM on this pin drives the right side backward
const int LeftForward = 33;     // PWM on this pin drives the left side forward
const int LeftBackward = 32;    // PWM on this pin drives the left side backward

// ---------- IR receiver pins (placeholders, update after wiring) ----------
// Input-capable pins only. 34 and 35 are fine here because these are inputs.
const int LeftIRPin = 34;
const int RightIRPin = 35;

// ---------- Memory values (what the robot remembers) ----------
const int WaitingForSignal = 1;   // Startup: no signal seen yet, motors stay off
const int LastSeenLeft = 2;       // Operator was last seen on the left
const int LastSeenRight = 3;      // Operator was last seen on the right
const int LastSeenAhead = 4;      // Operator was last seen straight ahead (both receivers)

// ---------- Tuning ----------
const int DrivePower = 150;                 // PWM value (0-255) while driving forward
const int TurnPower = 150;                  // PWM value (0-255) at the END of the turn ramp
const int TurnStartPower = 60;              // PWM value (0-255) where the ramp starts (high enough that the motors don't stall)
const unsigned long TurnRampMs = 2000;      // (ms) How long the turn takes to ramp from start power to full power
const unsigned long SearchTimeoutMs = 5000; // (ms) How long to search or coast before giving up

// ---------- Variables ----------
int Memory = WaitingForSignal;      // Starts in the waiting state
bool SignalLost = false;            // True while neither receiver sees the signal
unsigned long SignalLostTime = 0;   // millis() when the signal was first lost

// Turn ramp memory
bool Turning = false;               // True while the robot is in a turn
int TurnDirection = 0;              // -1 = turning left, +1 = turning right
unsigned long TurnStartTime = 0;    // millis() when the current turn started

void setup()
{
  Serial.begin(115200);

  pinMode(RightForward, OUTPUT);
  pinMode(RightBackward, OUTPUT);
  pinMode(LeftForward, OUTPUT);
  pinMode(LeftBackward, OUTPUT);

  pinMode(LeftIRPin, INPUT);
  pinMode(RightIRPin, INPUT);

  StopMotors();

  // Time to place the robot down
  delay(5000);
}

void loop()
{
  // Active LOW: the pin reads LOW when the 38 kHz signal is seen
  bool LeftSees = (digitalRead(LeftIRPin) == LOW);
  bool RightSees = (digitalRead(RightIRPin) == LOW);

  if (LeftSees && RightSees)
  {
    // Operator straight ahead
    Memory = LastSeenAhead;
    SignalLost = false;
    DriveForward();
  }
  else if (LeftSees)
  {
    // Operator on the left, turn left until both see it
    Memory = LastSeenLeft;
    SignalLost = false;
    TurnLeft();
  }
  else if (RightSees)
  {
    // Operator on the right, turn right until both see it
    Memory = LastSeenRight;
    SignalLost = false;
    TurnRight();
  }
  else
  {
    // Neither receiver sees the signal, use the memory
    HandleLostSignal();
  }

  // Print the memory when it changes, so you can follow what the robot thinks
  PrintMemoryIfChanged();
}

// Called every loop while neither receiver sees the signal
void HandleLostSignal()
{
  // Startup: nothing seen yet, so do nothing
  if (Memory == WaitingForSignal)
  {
    StopMotors();
    return;
  }

  // First moment of the loss: start the timer
  if (!SignalLost)
  {
    SignalLost = true;
    SignalLostTime = millis();
  }

  // Give up after the timeout and go back to waiting
  if (millis() - SignalLostTime >= SearchTimeoutMs)
  {
    StopMotors();
    Memory = WaitingForSignal;
    SignalLost = false;
    return;
  }

  // Still inside the search time: act on the memory
  if (Memory == LastSeenLeft)
  {
    TurnLeft();
  }
  else if (Memory == LastSeenRight)
  {
    TurnRight();
  }
  else if (Memory == LastSeenAhead)
  {
    DriveForward();
  }
}

// Works out the current turn power. Starts at TurnStartPower and climbs
// to TurnPower over TurnRampMs. Called every loop while turning.
int CurrentTurnPower()
{
  unsigned long elapsed = millis() - TurnStartTime;

  // Ramp finished: use full power
  if (elapsed >= TurnRampMs)
  {
    return TurnPower;
  }

  // Still ramping: map the elapsed time onto the power range
  return TurnStartPower + (TurnPower - TurnStartPower) * elapsed / TurnRampMs;
}

// Starts a fresh ramp if we weren't already turning in this direction.
// direction: -1 = left, +1 = right
void StartTurnIfNeeded(int direction)
{
  if (!Turning || TurnDirection != direction)
  {
    Turning = true;
    TurnDirection = direction;
    TurnStartTime = millis();
  }
}

// Remembers the last printed value so we only print on a change
int LastPrintedMemory = 0;

void PrintMemoryIfChanged()
{
  if (Memory != LastPrintedMemory)
  {
    LastPrintedMemory = Memory;
    Serial.print("Memory is now: ");
    Serial.println(Memory);
  }
}

// Both sides forward
void DriveForward()
{
  Turning = false;   // Not turning any more, so the next turn ramps up from the start
  StopMotors();
  analogWrite(LeftForward, DrivePower);
  analogWrite(RightForward, DrivePower);
}

// Left side backward, right side forward, with a gradual ramp-up
void TurnLeft()
{
  StartTurnIfNeeded(-1);
  int power = CurrentTurnPower();
  StopMotors();
  analogWrite(LeftBackward, power);
  analogWrite(RightForward, power);
}

// Left side forward, right side backward, with a gradual ramp-up
void TurnRight()
{
  StartTurnIfNeeded(1);
  int power = CurrentTurnPower();
  StopMotors();
  analogWrite(LeftForward, power);
  analogWrite(RightBackward, power);
}

// All four pins off
void StopMotors()
{
  analogWrite(LeftForward, 0);
  analogWrite(LeftBackward, 0);
  analogWrite(RightForward, 0);
  analogWrite(RightBackward, 0);
}
