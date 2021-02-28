
#include "Ultrasonic.h"
#include "VibrationMotor.h"

void SendJson();

CUltrasonic Ultrasonic;

C_VIBRATION_MOTOR VibrationMotors;

void setup() {
  Serial.begin(9600); // Starts the serial communication
 // VibrationMotors.TestMotors();
}


void loop() {
 //Read sensor values and calculates it 
  Ultrasonic.ReadDistance();
  // Calculate motor vibration magnitute
  
  if(Ultrasonic.pos_3d.valid){
    VibrationMotors.CalculateMotors(Ultrasonic.pos_3d);
  }
  else{
    VibrationMotors.PulseMod(Ultrasonic.Distance0, Ultrasonic.Distance1, Ultrasonic.Distance2);
  }  
  //Send to PC to monitor it
  SendJson(); 
}



void SendJson()
{
  C3DPoint obj = Ultrasonic.pos_3d;
  
  int s0 = Ultrasonic.Distance0;
  int s1 = Ultrasonic.Distance1;
  int s2 = Ultrasonic.Distance2;
  int sM = Ultrasonic.DistanceC;

  int X = obj.X;
  int Y = obj.Y;
  int Z = obj.Z;
  int V = obj.valid;
 
  char json[64];  //sprinf float print not implemented in arduino
  sprintf(json, "{\"x\":%d, \"y\":%d, \"z\":%d, \"s0\":%d, \"s1\":%d, \"s2\":%d, \"sM\":%d, \"v\":%d}", 
  X, Y, Z, s0, s1, s2, sM, V);
  Serial.println(json);

}
