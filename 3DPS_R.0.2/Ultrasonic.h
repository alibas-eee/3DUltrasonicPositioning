
#include "Arduino.h"
#include "Signal.h"
#include "SimpleKalmanFilter.h"
#include "PositionLogic.h"

//Sensor pin depinition
const int trigPin0 = A5;
const int echoPin0 = A4;

const int trigPin1 = A3;
const int echoPin1 = A2;

const int trigPin2 = A1;
const int echoPin2 = A0;

const int trigPinM = 2;
const int echoPinM = 3;


class CUltrasonic{

public:	
	CUltrasonic();
	void ReadDistance();
	void TriggerSensors();
	float Distance0;
	float Distance1;
	float Distance2;
	float DistanceC;
  C3DPoint pos_3d;
private:	
	SimpleKalmanFilter *KalmanFilter0;
	SimpleKalmanFilter *KalmanFilter1;
	SimpleKalmanFilter *KalmanFilter2;
	SimpleKalmanFilter *KalmanFilterM;
  CPositionLogic     *PositionLogic;
};


CUltrasonic::CUltrasonic(){
	KalmanFilter0 = new SimpleKalmanFilter (8, 1, 0.5);
	KalmanFilter1 = new SimpleKalmanFilter (8, 1, 0.5);
	KalmanFilter2 = new SimpleKalmanFilter (8, 1, 0.5);
	KalmanFilterM = new SimpleKalmanFilter (8, 1, 0.5);	
 
  PositionLogic = new CPositionLogic();  
  
  
  pinMode(trigPin0, OUTPUT); 
  pinMode(echoPin0, INPUT); 

  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT); 

  pinMode(trigPin2, OUTPUT); 
  pinMode(echoPin2, INPUT); 
  
  pinMode(trigPinM, OUTPUT); 
  pinMode(echoPinM, INPUT);	
}

//this function wait constant 30ms ever call keep in mind
void CUltrasonic::ReadDistance()
{
  float s0 = 0, s1 = 0, s2 = 0, sM = 0;
  TriggerSensors(); // Set all sensor to listening mode and transmit sound with master
  

  unsigned long timeout = 50000 + micros();
  CSignal sig0(echoPin0);
  CSignal sig1(echoPin1);
  CSignal sig2(echoPin2);
  CSignal sigM(echoPinM);

  while (timeout > micros() ) // Constant time wait
  {
    delayMicroseconds(1);

    sig0.Read();
    sig1.Read();
    sig2.Read();
	  sigM.Read();	
  }

  sig0.SyncRisingTimeWith(sigM);
  sig1.SyncRisingTimeWith(sigM);
  sig2.SyncRisingTimeWith(sigM);

  s0 = sig0.ReadDistance();
  s1 = sig1.ReadDistance();
  s2 = sig2.ReadDistance();
  sM = sigM.ReadDistance();

  if (s0 != 0)
    s0 = KalmanFilter0->updateEstimate(s0);
  if (s1 != 0)
    s1 = KalmanFilter1->updateEstimate(s1);
  if (s2 != 0)
    s2 = KalmanFilter2->updateEstimate(s2);
  if (sM != 0)
    sM = KalmanFilterM->updateEstimate(sM);

  Distance0 = s0;
  Distance1 = s1;
  Distance2 = s2;
  DistanceC = sM;

  pos_3d = PositionLogic->Calculate(Distance0, Distance1, Distance2, DistanceC);
}

//trigger all sensor to measurement state
//Start Pwm Timer 
void CUltrasonic::TriggerSensors() {
  digitalWrite(trigPin0, LOW);
  digitalWrite(trigPin1, LOW);
  digitalWrite(trigPin2, LOW);
  digitalWrite(trigPinM, LOW);
 // reset_timer();//Start pwm timer
  delayMicroseconds(5);
  digitalWrite(trigPin0, HIGH);
  digitalWrite(trigPin1, HIGH);
  digitalWrite(trigPin2, HIGH);
  digitalWrite(trigPinM, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin0, LOW);
  digitalWrite(trigPin1, LOW);
  digitalWrite(trigPin2, LOW);
  digitalWrite(trigPinM, LOW);
  //delayMicroseconds(70);
}
