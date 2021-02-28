//https://journals.sagepub.com/doi/full/10.5772/63246#:~:text=3.-,Trilateration%20Algorithms,a%20sphere%20or%20a%20circle.
#ifndef _POSITON_LOGIC_
#define _POSITON_LOGIC_

#include "Arduino.h"
#include <math.h>
#include "3DPoint.h"
                                      //   ___
#define  sqrt3     			1.73205       // \/ 3
#define  LENGTH 	  		20				    //CM
#define  LENGTH_INNER 	LENGTH/sqrt3	//CM
#define  MAX_RANGE 		  500 				  //CM
//#define RAD_TO_DEG		57.2957795


const float XO = 0;
const float YO = 0;
const float XA = -(LENGTH / 2);
const float YA = (LENGTH / 2) / sqrt3;
const float XB = (LENGTH / 2);
const float YB = (LENGTH / 2) / sqrt3;
const float XC = 0.0;
const float YC = -(LENGTH / sqrt3);


const float lAB = LENGTH;
const float lBC = LENGTH;
const float lCA = LENGTH;

const float lAO = LENGTH / sqrt3;
const float lBO = LENGTH / sqrt3;
const float lCO = LENGTH / sqrt3;



class CSensor {
  public:
    float X;
    float Y;
    float Measurement ;
    CSensor() {

    }
    CSensor(float x, float y) {
      X = x;
      Y = y;
      Measurement = 0;
    }
};


class CPositionLogic {
  public:
    CPositionLogic();
    C3DPoint Calculate(float MeasurementA, float MeasurementB, float MeasurementC, float MeasurementO);
  private:
    CSensor SensorO;
    CSensor SensorA;
    CSensor SensorB;
    CSensor SensorC;
    int totalValidPoint;
    C3DPoint Calculate();
    C3DPoint CalculateABC();
    C3DPoint AverageR(C3DPoint P0, C3DPoint P1, float ratio);
    bool TriangleCheckABC();
    bool TriangleCheckInner(CSensor S1, CSensor S2);
    C3DPoint CalculateInner(CSensor S1, CSensor S2);
};


CPositionLogic::CPositionLogic() {
  SensorO = CSensor(XO, YO);
  SensorA = CSensor(XA, YA);
  SensorB = CSensor(XB, YB);
  SensorC = CSensor(XC, YC);
}

C3DPoint CPositionLogic::Calculate(float MeasurementA, float MeasurementB, float MeasurementC, float MeasurementO) {
  SensorO.Measurement = MeasurementO;
  SensorA.Measurement = MeasurementA;
  SensorB.Measurement = MeasurementB;
  SensorC.Measurement = MeasurementC;

  return Calculate();
}

C3DPoint CPositionLogic::Calculate() {

  C3DPoint point;
  point.valid = true;

  C3DPoint pointABC = CalculateABC();
  C3DPoint pointOAB = CalculateInner(SensorA, SensorB);
  C3DPoint pointOBC = CalculateInner(SensorB, SensorC);
  C3DPoint pointOCA = CalculateInner(SensorC, SensorA);

  if (pointABC.valid) {
    if (totalValidPoint == 1) {
      return pointABC;
    }
    else if (totalValidPoint > 1) {
      float t = atan(pointABC.Y / pointABC.X) * RAD_TO_DEG;
      if (t > 30 && t < 150 && pointOAB.valid) { //AOB
        point =  AverageR(pointABC, pointOAB, 0.75);
      }
      if (t > 150 && t < 270 && pointOCA.valid) { //AOC
        point =  AverageR(pointABC, pointOCA, 0.75);
      }
      if (t > 270 && t < 30 && pointOBC.valid) { //COB
        point =  AverageR(pointABC, pointOBC, 0.75);
      } else {
        return pointABC;
      }
    }

  } else {
    if (totalValidPoint == 1)
    {
      if (pointOAB.valid) {
        point =  pointOAB;
      }
      else if (pointOCA.valid) {
        point =  pointOCA;
      }
      else if (pointOBC.valid) {
        point =  pointOBC;
      }
    }
    else if (totalValidPoint == 2)
    {
      if (pointOAB.valid && pointOCA.valid) {
        point =  AverageR(pointOCA, pointOAB, 0.5);
      }
      else if (pointOCA.valid && pointOBC.valid) {
        point =  AverageR(pointOCA, pointOBC, 0.5);
      }
      else if (pointOBC.valid && pointOAB.valid) {
        point =  AverageR(pointOBC, pointOAB, 0.5);
      }
    }
    else if (totalValidPoint == 3)
    {
      point.X = (pointOCA.X + pointOBC.X + pointOAB.X) / 3;
      point.Y = (pointOCA.Y + pointOBC.Y + pointOAB.Y) / 3;
      point.Z = (pointOCA.Z + pointOBC.Z + pointOAB.Z) / 3;
      point.valid = true;
    } else
    {
      point.valid = false;
    }

  }


  if (point.valid == false)
  {
    point.X = 0;
    point.Y = 0;
    point.Z = SensorO.Measurement;
  }


  return point;
}

C3DPoint CPositionLogic::AverageR(C3DPoint P0, C3DPoint P1, float ratio)
{
  C3DPoint pA;
  pA.X = P0.X * ratio + P1.X * (1 - ratio);
  pA.Y = P0.Y * ratio + P1.Y * (1 - ratio);
  pA.Z = P0.Z * ratio + P1.Z * (1 - ratio);
  pA.valid = true;
  return pA;
}

//check if position values fit in triangle rules
bool CPositionLogic::TriangleCheckABC() {

  if (abs(SensorA.Measurement - SensorB.Measurement) < LENGTH && abs(SensorA.Measurement - SensorC.Measurement) < LENGTH && abs(SensorC.Measurement - SensorB.Measurement) < LENGTH) {
    return true;
  }
  else {
    return false;
  }
}

//check if position values fit in triangle rules inner triangle
bool CPositionLogic::TriangleCheckInner(CSensor S1, CSensor S2) {
  if (abs(SensorO.Measurement - S1.Measurement) < LENGTH_INNER && abs(SensorO.Measurement - S2.Measurement) < LENGTH_INNER && abs(S1.Measurement - S2.Measurement) < LENGTH) {
    return true;
  }
  else {
    return false;
  }
}

//calculate 3d point from given 3 ultrasonic distance
C3DPoint CPositionLogic::CalculateABC() {
  C3DPoint point;

  if (TriangleCheckABC()) {
    point.X = ( pow(SensorA.Measurement, 2) - pow(SensorB.Measurement, 2) + pow(SensorB.X, 2)) / (2 * SensorB.X);
    point.Y = ( pow(SensorA.Measurement, 2) - pow(SensorC.Measurement, 2) + pow(SensorC.X, 2) + pow(SensorC.Y, 2) - (2 * SensorC.X * point.X)) / (2 * SensorC.Y);
    point.Z = sqrt( pow(SensorA.Measurement, 2) - pow(point.X, 2) - pow(point.Y, 2));

    point.X = point.X - SensorA.X;
    point.Y = point.Y - SensorA.Y;
    point.valid = true;
    totalValidPoint = 1;
  }
  else {
    point.valid = false;
    totalValidPoint = 0;
  }

  return point;
}



//calculate 3d point from given 3 ultrasonic distance
C3DPoint CPositionLogic::CalculateInner(CSensor S1, CSensor S2) {
  C3DPoint point;

  if (TriangleCheckABC()) {
    point.X = ( pow(SensorO.Measurement, 2) - pow(S1.Measurement, 2) + pow(S1.X, 2)) / (2 * S1.X);
    point.Y = ( pow(SensorO.Measurement, 2) - pow(S2.Measurement, 2) + pow(S2.X, 2) + pow(S2.Y, 2) - (2 * S2.X * point.X)) / (2 * S2.Y);
    point.Z = sqrt( pow(SensorO.Measurement, 2) - pow(point.X, 2) - pow(point.Y, 2));

    point.valid = true;
    totalValidPoint++;
  }
  else {
    point.valid = false;
  }

  return point;
}

#endif
