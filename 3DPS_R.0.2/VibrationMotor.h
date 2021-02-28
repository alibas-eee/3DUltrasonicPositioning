
#include "Arduino.h"
#include "Adafruit_TLC59711.h"
#include "3DPoint.h"
#include <SPI.h>

// How many boards do you have chained?
#define NUM_TLC59711 1

#define data   5
#define clock  4

Adafruit_TLC59711 tlc = Adafruit_TLC59711(NUM_TLC59711, clock, data);

#define R 44.58
#define SQ2 1.4142
#define PM 3-SQ2

class C_MOTOR {
  public:
    C_MOTOR();
    C_MOTOR(float x, float y);
    void CheckDistance(float x, float y, float r);
    float X;
    float Y;
    long output;
};

C_MOTOR::C_MOTOR() {
  this->X = 0;
  this->Y = 0;
  this->output = 0;
}

C_MOTOR::C_MOTOR(float x, float y) {
  this->X = x;
  this->Y = y;
  this->output = 0;
}

void C_MOTOR::CheckDistance(float x, float y, float r) {
  float dx = abs( this->X - x);
  float dy = abs( this->Y - y);
  float d = sqrt( pow(dx, 2) + pow(dy, 2));

  float _R = 0;
  if (r < 300) { // uzaklık 3M den az ise
    _R = (300 - r) / 3 ;
    if (d < (_R / 2) )
      this->output = 65000; // obje konumu motor konumuna yakın ise daha çok titreşim ver
    else if (d < _R )
      this->output = 32000; // obje konumu motor konumuna çok yakın değil ise az titreşim ver
    else
      this->output = 0; // obje konumu motor konumuna uzak ise motor çalışmasın
  } else {
    this->output = 0;
  }

}


static C_MOTOR motors[] = {
  C_MOTOR(R, 3 * R),
  C_MOTOR((PM * R), (PM * R)),
  C_MOTOR(3 * R, R),

  C_MOTOR(3 * R, -R),
  C_MOTOR((PM * R), (-PM * R)),
  C_MOTOR(R, -3 * R),

  C_MOTOR(-R, -3 * R),
  C_MOTOR((-PM * R), -(PM * R)),
  C_MOTOR(-3 * R, -R),

  C_MOTOR(-3 * R, R),
  C_MOTOR((-PM * R), (PM * R)),
  C_MOTOR(-R, 3 * R)
};

class C_VIBRATION_MOTOR {
  public:
    C_VIBRATION_MOTOR();
    void CalculateMotors(C3DPoint obj);
    void TestMotors();
    void PulseMod(float a, float b, float c);
    void PulseDir(int dir, int mag);
  private:
    void MotorWriteOutput() ;
    int counter;

};

C_VIBRATION_MOTOR::C_VIBRATION_MOTOR() {

  tlc.begin();
  tlc.write();
  counter = 0;
}

void C_VIBRATION_MOTOR::TestMotors() {
  for (int i = 0; i < 12; i++)
  {
    for (int j = 0; j < 12; j++)
    {
      if (i == j)
        motors[j].output = 65000;
      else
        motors[j].output = 0;
    }
    MotorWriteOutput();
    delay(1000);
  }

  PulseDir(0, 65000);
  delay(1000);
  PulseDir(1, 65000);
  delay(1000);
  PulseDir(2, 65000);
  delay(1000);
  PulseDir(2, 0);
}


void C_VIBRATION_MOTOR::PulseMod(float a, float b, float c)
{
  int m = 0;
  if (a < b && a < c && a > 0 && a < 300)
  {
    m =  (300 - a) * 216;
    PulseDir(0, m);
  }

  if (b < a && b < c && b > 0 && b < 300)
  {
    m =  (300 - b) * 216;
    PulseDir(1, m);
  }

  if (c < b && c < a)
  {
    m =  (300 - c) * 216;
    PulseDir(2, m);
  }
}

void C_VIBRATION_MOTOR::PulseDir(int dir, int mag) {

  for (int i = 0; i < 12; i++)
  {
    if ( (i / 4) == dir)
      motors[i].output = mag;
    else
      motors[i].output = 0;
  }
  MotorWriteOutput();
}

//objenin uzaklığını hesapla
//Hangi motorun çalışacağına karar ver
void C_VIBRATION_MOTOR::CalculateMotors(C3DPoint obj) {

  float r = sqrt( pow(obj.X, 2) + pow(obj.Y, 2) + pow(obj.Z, 2) );

  for (int i = 0; i < 12; i++)
  {
    motors[i].CheckDistance( obj.X, obj.Y, r );
  }

  MotorWriteOutput();
}

//pwm değerlerini motorlara yazar
void C_VIBRATION_MOTOR::MotorWriteOutput() {
  counter++;

  for (int id = 0; id < 12; id++)
  {
    tlc.setPWM(id, motors[id].output);
    if (counter > 2)
      tlc.setPWM(id, 0);
  }

  tlc.write();

  if (counter == 5)
    counter = 0;
}
