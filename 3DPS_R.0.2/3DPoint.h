#ifndef _3D_POINT_
#define _3D_POINT_

class C3DPoint {
  public:
    float X;
    float Y;
    float Z;
    bool  valid;
    C3DPoint(float x, float y, float z) {
      X = x;
      Y = y;
      Z = z;
      valid = false;
    }

    C3DPoint() {
      X = 0;
      Y = 0;
      Z = 0;
      valid = false;
    }
};

#endif