
#include "Trigger.h"
#include "Signal.h"
#include "SimpleKalmanFilter.h"


#define LENGTH 16		//CM
#define MAX_RANGE 500 //CM

#define X1 -8    //0
#define Y1 4.618  //0
//use Point1 as origin than calculate offset
#define X2 16.0
#define Y2 0.0
#define X3 8.0
#define Y3 -13.86


SimpleKalmanFilter KalmanFilter0(8, 1, 0.5);
SimpleKalmanFilter KalmanFilter1(8, 1, 0.5);
SimpleKalmanFilter KalmanFilter2(8, 1, 0.5);

//Sensor pin depinition
const int trigPin0 = A5;
const int echoPin0 = A4;

const int trigPin1 = A3;
const int echoPin1 = A2;

const int trigPin2 = A1;
const int echoPin2 = A0;

const int trigPinMaster = 3;

// define function prototoypes
void triggerPin();
void calXYZ(float r1, float r2, float r3);
void SendJson(int x, int y, int z, int s0, int s1, int s2, int v);
void read_pulse(float *data0, float *data1, float *data2);

void setup() {
  pinMode(trigPin0, OUTPUT); 
  pinMode(echoPin0, INPUT); 

  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT); 

  pinMode(trigPin2, OUTPUT); 
  pinMode(echoPin2, INPUT); 

  pinMode(trigPinMaster, OUTPUT);
  Serial.begin(9600); // Starts the serial communication
  InitTimer();
  disable_timer();

}

float d0f = 0, d1f = 0, d2f = 0;

float PosX, PosY, PosZ;
int valid = 0;
void loop() {
 
  delay(10);  
  read_pulse(&d0f, &d1f, &d2f);

  calXYZ(d0f, d1f, d2f);
  SendJson(PosX, PosY , PosZ, d0f * 10 , d1f * 10, d2f * 10, valid);
}

//https://journals.sagepub.com/doi/full/10.5772/63246#:~:text=3.-,Trilateration%20Algorithms,a%20sphere%20or%20a%20circle.
void calXYZ(float r1, float r2, float r3)
{
  float x = 0, y = 0, z = 0;

  if (abs(r1 - r2) < LENGTH && abs(r2 - r3) < LENGTH && abs(r3 - r1) < LENGTH) //triangle check
  {

    x = ( pow(r1, 2) - pow(r2, 2) + pow(X2, 2)) / (2 * X2);
    y = ( pow(r1, 2) - pow(r3, 2) + pow(X3, 2) + pow(Y3, 2) - (2 * X3 * x)) / (2 * Y3);
    z = sqrt( pow(r1, 2) - pow(x, 2) - pow(y, 2));

    PosX = x - X1;
    PosY = y - Y1;
    PosZ = z;
    valid = 1;
  }
  else
  {
    valid = 0;
  }
}

void SendJson(int x, int y, int z, int s0, int s1, int s2, int v)
{
  char json[64];  //sprinf float print not implemented in arduino
  sprintf(json, "{\"x\":%d, \"y\":%d, \"z\":%d, \"s0\":%d, \"s1\":%d, \"s2\":%d,  \"v\":%d}", x, y, z, s0, s1, s2, v);
  Serial.println(json);

}

void SendJson(int x, int y, int z, int v)
{
  char json[32];
  sprintf(json, "{\"x\":%d, \"y\":%d, \"z\":%d, \"v\":%d}", x, y, z, v);
  Serial.println(json);

}


//this function wait constant 30ms ever call keep in mind
void read_pulse(float *data0, float *data1, float *data2)
{
  float s0 = 0, s1 = 0, s2 = 0;
  triggerPin(); // Set all sensor to listening mode and transmit sound with master
  

  unsigned long timeout = 30000 + micros();
  CSignal sig0(echoPin0);
  CSignal sig1(echoPin1);
  CSignal sig2(echoPin2);

  while (timeout > micros() ) // Constant time wait
  {
    delayMicroseconds(1);

    sig0.Read();
    sig1.Read();
    sig2.Read();

    if (sig0.rising_time > 0 || sig1.rising_time > 0 || sig2.rising_time > 0)
      disable_timer(); //Stop transmiting sound with master 
  }

  s0 = sig0.ReadDistance();
  s1 = sig1.ReadDistance();
  s2 = sig2.ReadDistance();

  if (s0 != 0)
    s0 = KalmanFilter0.updateEstimate(s0);
  if (s1 != 0)
    s1 = KalmanFilter1.updateEstimate(s1);
  if (s2 != 0)
    s2 = KalmanFilter2.updateEstimate(s2);

  *data0 = s0;
  *data1 = s1;
  *data2 = s2;
}


//trigger all sensor to measurement state
//Start Pwm Timer 
void triggerPin() {
  digitalWrite(trigPin0, LOW);
  digitalWrite(trigPin1, LOW);
  digitalWrite(trigPin2, LOW);
  reset_timer();//Start pwm timer
  digitalWrite(trigPin0, HIGH);
  digitalWrite(trigPin1, HIGH);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin0, LOW);
  digitalWrite(trigPin1, LOW);
  digitalWrite(trigPin2, LOW);
  //delayMicroseconds(70);
}

//Standart HC-04 Read function
int readDistance(int trig, int echo)
{
  long duration;
  int distance;
  // Clears the trigPin
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  triggerPin();

  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echo, HIGH, 20000);
  // Calculating the distance
  distance = duration * 0.034 / 2;

  return distance;
}
