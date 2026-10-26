const int FrontTrigPin = 1;   //
const int FrontEchoPin = 2;   // 
const int RightTrigPin = 3;   // Ultrasound sensor pins
const int RightEchoPin = 4;   //
const int LeftTrigPin = 5;    //
const int LeftEchoPin = 6;    //

const int RightIR = 7;        //IR reciever pins
const int LeftIR = 8;         //

const int RightForward = 9;   // Pin to control Right wheels forward drive
const int RightBackward = 10; // Pin to control Right wheels backward drive
const int LeftForward = 11;   // Pin to control Left wheels forward drive
const int LeftBackward = 12;  // Pin to control Left wheels backward drive

const int RightSpeed = 13;    // PWM pins that controls Right wheel speeds
const int LeftSpeed = 14;     // PWM pin that controls Left wheel speeds

const int ObstacleCm = 30;    // Distance detected by US-Sensors that force wheels to stop
const int Wait = 1000;        // Delay for stopping wheels when emitter signal is lost 
const int StopIR = 2000;      // Signal strength (0-4095) that is too strong = emitter too close = stop wheels 
const int IRLostStrength = 50;        // The strength of signal (0-4095) where there is no signal = operator lost

const int TurnTimeMs = 400;        // (ms) Time to spin for a 90° turn. PLACEHOLDER, tune by testing
const int TurnPower = 150;         // PWM value (0-255) for the motors during a turn. PLACEHOLDER, tune by testing

const int TurnDriveTimeMs = 300;   // (ms) Time motors are ON for the ~20 cm drive after a turn. PLACEHOLDER, tune by testing
const int TurnDrivePower = 150;    // PWM value (0-255) for the drive after a turn. PLACEHOLDER, tune by testing



const unsigned long PrintIntervalMs = 1000;    // How often code prints values to serialmonitor (ms)
unsigned long lastPrintTime = 0;               // millis() value at the last print
bool IRLost = false;                       // true once we've noticed the signal is gone
unsigned long IRLostTime = 0;              // millis() value when the signal first went missing


void setup()
{
Serial.begin(115200);

pinMode(FrontTrigPin, OUTPUT);   
pinMode(FrontEchoPin, INPUT);    
pinMode(RightTrigPin, OUTPUT);      
pinMode(RightEchoPin, INPUT);    
pinMode(LeftTrigPin, OUTPUT);     
pinMode(LeftEchoPin, INPUT); 

pinMode(RightIR, INPUT);
pinMode(LeftIR, INPUT);

pinMode(RightForward, OUTPUT);
pinMode(RightBackward, OUTPUT);
pinMode(LeftForward, OUTPUT);
pinMode(LeftBackward, OUTPUT);

pinMode(RightSpeed, OUTPUT);
pinMode(LeftSpeed, OUTPUT);
}

void loop ()
{
int RightIRStrength = analogRead(RightIR);
int LeftIRStrength = analogRead(LeftIR);

// This box is for serialprinting, it prevents giving delay to the actual void loop that handles other tasks
if (millis() - lastPrintTime >= PrintIntervalMs)
  {
    Serial.print("Right IR sensor strength: ");
    Serial.println(RightIRStrength);
    Serial.print("Left IR sensor strength: ");
    Serial.println(LeftIRStrength);

    lastPrintTime = millis();
  }
  
if (RightIRStrength <= IRLostStrength && LeftIRStrength <= IRLostStrength)
{
  if (!IRLost)
  {
    IRLost = true;
    IRLostTime = millis();          
  }
  else if (millis() - IRLostTime >= Wait)
  {
    analogWrite (RightSpeed, 0);  // Cut power to wheels when there is no signal after the wait
    analogWrite (LeftSpeed, 0);
  }
}
else
{
  IRLost = false;           // signal is back, reset the memory
}

// Here starts the obstacle detection and obstacle steering
float FrontDistance = MeasureDistance (FrontTrigPin, FrontEchoPin);
float RightDistance = MeasureDistance (RightTrigPin, RightEchoPin);
float LeftDistance = MeasureDistance (LeftTrigPin, LeftEchoPin);
if (FrontDistance<ObstacleCm)
{
analogWrite(RightSpeed, 0);
analogWrite(LeftSpeed, 0);
  if (RightDistance<ObstacleCm)
  {
    if(LeftDistance<ObstacleCm)
    {
      // Here goes the commands for motor drivers to perform a 2x90° turn to the right
    }
    else
    {
      // Here goes to commands for motor drivers to perform a 90° turn to the Left and drive 20cm forward
    }
  }
    
}
  
} // THIS IS THE VOID LOOP END

//This function is for turning to the right
void TurnRight90()
{
  // Make sure the pins that we dont want to be on are off
  analogWrite(LeftBackward, 0);
  analogWrite(RightForward, 0);
  // Turn on the pins that are needed for turn
  analogWrite(LeftForward, TurnPower); //  Power is tested and tuned in the defined constants
  analogWrite(RightBackward, TurnPower);
  // The duration that the motors excecute the turn for
  delay(TurnTimeMs);
  // Stop
  analogWrite(LeftForward, 0);
  analogWrite(RightBackward, 0);
}

//This function is for turning to the left
void TurnLeft90()
{
   // Make sure the pins that we dont want to be on are off
  analogWrite(LeftForward, 0);
  analogWrite(RightBackward, 0);
  // Turn on the pins that are needed for turn
  analogWrite(LeftBackward, TurnPower); //  Power is tested and tuned in the defined constants
  analogWrite(RightForward, TurnPower);
  // The duration that the motors excecute the turn for
  delay(TurnTimeMs);
  // Stop
  analogWrite(LeftBackward, 0);
  analogWrite(RightForward, 0);
}

//This function is for turning 180° to the right
void TurnRight180()
{
  // Make sure the pins that we dont want to be on are off
  analogWrite(LeftBackward, 0);
  analogWrite(RightForward, 0);
  // Turn on the pins that are needed for turn
  analogWrite(LeftForward, TurnPower); //  Power is tested and tuned in the defined constants
  analogWrite(RightBackward, TurnPower);
  // The duration that the motors excecute the turn for
  delay(2*TurnTimeMs);
  // Stop
  analogWrite(LeftForward, 0);
  analogWrite(RightBackward, 0);
}

// This function is for measuring distance using the US-Sensors
float MeasureDistance(int trigPin, int echoPin)
{
  // Clear the table: trig low, short wait
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Send the "go" pulse: trig high for 10 microseconds, then low
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Listen: how long did the echo pin stay high? (microseconds)
  long Duration = pulseIn(echoPin, HIGH);
  // Convert to cm: speed of sound times time, divided by 2 for the round trip
  float Distance = Duration * 0.0343 / 2;
  return Distance;  // hand the number back to whoever called the function
}
