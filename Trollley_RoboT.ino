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

/*In progress --->
const int TurnDriveTime =     // (ms) The time needed for the motors to be ON when performing the drive (aprox. 20cm) after turning
const int TurnDrivePower =    // PWM value (0-255) for the motors when performing the drive (aprox. 20cm) after turning
const int RightTurnTime =
const int RightTurnPower =
const int LeftTurnTime =
const int LeftTurnPower =
*/

const unsigned long PrintIntervalMs = 1000;  // How often code prints values to serialmonitor (ms)
unsigned long lastPrintTime = 0;             // millis() value at the last print


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
}
asdasdasdaskfnjalkasjlmzxömcölasmclömslöAM