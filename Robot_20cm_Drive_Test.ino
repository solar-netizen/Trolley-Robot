// ---------- Motor pins (placeholders, update after wiring) ----------
const int RightForward = 9;    // Pin to control Right wheels forward drive
const int RightBackward = 10;  // Pin to control Right wheels backward drive
const int LeftForward = 11;    // Pin to control Left wheels forward drive
const int LeftBackward = 12;   // Pin to control Left wheels backward drive

// ---------- Drive tuning (tune by testing) ----------
const int TurnDriveTimeMs = 600;   // (ms) How long the motors run to drive about 20 cm
const int TurnDrivePower = 150;    // PWM value (0-255) for the motors while driving

// Pause between test runs
const int TestPauseMs = 4000;      // (ms) Time to measure the result before the next run

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
  Serial.println("Driving forward");
  DriveAfterTurn();

  Serial.println("Done, measure the distance");
  delay(TestPauseMs);
}

// This function is for driving straight forward
void DriveAfterTurn()
{
  // Make sure the backward pins are off
  analogWrite(LeftBackward, 0);
  analogWrite(RightBackward, 0);
  // Both sides forward
  analogWrite(LeftForward, TurnDrivePower);
  analogWrite(RightForward, TurnDrivePower);
  // How long the motors run
  delay(TurnDriveTimeMs);
  // Stop
  analogWrite(LeftForward, 0);
  analogWrite(RightForward, 0);
}
