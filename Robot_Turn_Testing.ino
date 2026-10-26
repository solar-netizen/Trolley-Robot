// ---------- Motor pins (placeholders, update after wiring) ----------
const int RightForward = 9;    // Pin to control Right wheels forward drive
const int RightBackward = 10;  // Pin to control Right wheels backward drive
const int LeftForward = 11;    // Pin to control Left wheels forward drive
const int LeftBackward = 12;   // Pin to control Left wheels backward drive

// ---------- Turn tuning (tune by testing) ----------
const int TurnTimeMs = 500;    // (ms) How long the motors run for a 90 degree turn
const int TurnPower = 150;     // PWM value (0-255) for the motors while turning

// Pause between test turns
const int TestPauseMs = 3000;  // (ms) Time to watch the result before the next turn

void setup()
{
  Serial.begin(115200);

  pinMode(RightForward, OUTPUT);
  pinMode(RightBackward, OUTPUT);
  pinMode(LeftForward, OUTPUT);
  pinMode(LeftBackward, OUTPUT);

  // Give yourself time to place the robot on the floor
  delay(5000);
}

void loop()
{
  Serial.println("Turning right 90");
  TurnRight90();
  delay(TestPauseMs);

  Serial.println("Turning left 90");
  TurnLeft90();
  delay(TestPauseMs);

  Serial.println("Turning right 180");
  TurnRight180();
  delay(TestPauseMs);
}

// This function is for turning to the right
void TurnRight90()
{
  // Make sure the pins that we don't want on are off
  analogWrite(LeftBackward, 0);
  analogWrite(RightForward, 0);
  // Turn on the pins needed for the turn
  analogWrite(LeftForward, TurnPower);
  analogWrite(RightBackward, TurnPower);
  // How long the motors run
  delay(TurnTimeMs);
  // Stop
  analogWrite(LeftForward, 0);
  analogWrite(RightBackward, 0);
}

// This function is for turning to the left
void TurnLeft90()
{
  analogWrite(LeftForward, 0);
  analogWrite(RightBackward, 0);
  analogWrite(LeftBackward, TurnPower);
  analogWrite(RightForward, TurnPower);
  delay(TurnTimeMs);
  analogWrite(LeftBackward, 0);
  analogWrite(RightForward, 0);
}

// This function is for turning 180 degrees to the right
void TurnRight180()
{
  analogWrite(LeftBackward, 0);
  analogWrite(RightForward, 0);
  analogWrite(LeftForward, TurnPower);
  analogWrite(RightBackward, TurnPower);
  delay(2 * TurnTimeMs);
  analogWrite(LeftForward, 0);
  analogWrite(RightBackward, 0);
}
