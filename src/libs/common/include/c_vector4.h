#pragma once

#include "vector_rs.hpp"

struct CVECTOR4
{
  public:
    float x, y, z, w;

    CVECTOR4(){};

    CVECTOR4(float a)
    {
        x = a;
        y = a;
        z = a;
        w = 1.0f;
    };

    CVECTOR4(float _x, float _y, float _z, float _w)
    {
        x = _x;
        y = _y;
        z = _z;
        w = _w;
    };

    CVECTOR4(float a[4])
    {
        x = a[0];
        y = a[1];
        z = a[2];
        w = a[3];
    };

    CVECTOR4(const rust::vec::Vec4F &v)
    {
        x = v.X();
        y = v.Y();
        z = v.Z();
        w = v.W();
    };

    rust::vec::Vec4F ToVec4F() const
    {
        return rust::vec::Vec4F(x, y, z, w);
    }
};
