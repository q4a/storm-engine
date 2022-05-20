#pragma once

#include "rust_rs.h"

namespace rust::vec
{
struct Vec3F
{
  private:
    VecWrapper3F *vec;

    Vec3F(VecWrapper3F *vec)
    {
        this->vec = vec;
    }

  public:
    Vec3F(float x, float y, float z)
    {
        vec = ffi_new_3f_vector(x, y, z);
    }

    ~Vec3F()
    {
        ffi_free_3f_vector(vec);
    }

    const float X() const
    {
        return ffi_vector3f_x(vec);
    }

    const float Y() const
    {
        return ffi_vector3f_y(vec);
    }

    const float Z() const
    {
        return ffi_vector3f_z(vec);
    }

    Vec3F &operator+=(const Vec3F &v)
    {
        ffi_vector3f_add_self(vec, v.vec);
        return *this;
    };

    Vec3F &operator-=(const Vec3F &v)
    {
        ffi_vector3f_sub_self(vec, v.vec);
        return *this;
    };

    Vec3F &operator*=(const Vec3F &v)
    {
        ffi_vector3f_mul_self(vec, v.vec);
        return *this;
    };

    Vec3F &operator/=(const Vec3F &v)
    {
        ffi_vector3f_div_self(vec, v.vec);
        return *this;
    };

    friend Vec3F operator+(const Vec3F &v1, const Vec3F &v2)
    {
        auto result = ffi_vector3f_add(v1.vec, v2.vec);
        return Vec3F(result);
    };

    friend Vec3F operator-(const Vec3F &v1, const Vec3F &v2)
    {
        auto result = ffi_vector3f_sub(v1.vec, v2.vec);
        return Vec3F(result);
    };

    friend Vec3F operator*(const Vec3F &v1, const Vec3F &v2)
    {
        auto result = ffi_vector3f_mul(v1.vec, v2.vec);
        return Vec3F(result);
    };

    friend Vec3F operator/(const Vec3F &v1, const Vec3F &v2)
    {
        auto result = ffi_vector3f_div(v1.vec, v2.vec);
        return Vec3F(result);
    };

    friend Vec3F operator*(const Vec3F &v1, float f)
    {
        auto result = ffi_vector3f_mul_f32(v1.vec, f);
        return Vec3F(result);
    };

    friend Vec3F operator/(const Vec3F &v1, float f)
    {
        auto result = ffi_vector3f_div_f32(v1.vec, f);
        return Vec3F(result);
    };

    const float Length() const
    {
        return ffi_vector3f_length(vec);
    }

    const float LengthSQ() const
    {
        return ffi_vector3f_length_sq(vec);
    }

    Vec3F normal() const
    {
        auto result = ffi_vector3f_normal(vec);
        return Vec3F(result);
    }

    Vec3F cross(const Vec3F &v2) const
    {
        auto result = ffi_vector3f_cross(vec, v2.vec);
        return Vec3F(result);
    }

    const float dot(const Vec3F &v2) const
    {
        return ffi_vector3f_dot(vec, v2.vec);
    }
};

struct Vec3D
{
  private:
    VecWrapper3D *vec;

    Vec3D(VecWrapper3D *vec)
    {
        this->vec = vec;
    }

  public:
    Vec3D(double x, double y, double z)
    {
        vec = ffi_new_3d_vector(x, y, z);
    }

    ~Vec3D()
    {
        ffi_free_3d_vector(vec);
    }

    const double X() const
    {
        return ffi_vector3d_x(vec);
    }

    const double Y() const
    {
        return ffi_vector3d_y(vec);
    }

    const double Z() const
    {
        return ffi_vector3d_z(vec);
    }

    Vec3D &operator+=(const Vec3D &v)
    {
        ffi_vector3d_add_self(vec, v.vec);
        return *this;
    };

    Vec3D &operator-=(const Vec3D &v)
    {
        ffi_vector3d_sub_self(vec, v.vec);
        return *this;
    };

    Vec3D &operator*=(const Vec3D &v)
    {
        ffi_vector3d_mul_self(vec, v.vec);
        return *this;
    };

    Vec3D &operator/=(const Vec3D &v)
    {
        ffi_vector3d_div_self(vec, v.vec);
        return *this;
    };

    friend Vec3D operator+(const Vec3D &v1, const Vec3D &v2)
    {
        auto result = ffi_vector3d_add(v1.vec, v2.vec);
        return Vec3D(result);
    };

    friend Vec3D operator-(const Vec3D &v1, const Vec3D &v2)
    {
        auto result = ffi_vector3d_sub(v1.vec, v2.vec);
        return Vec3D(result);
    };

    friend Vec3D operator*(const Vec3D &v1, const Vec3D &v2)
    {
        auto result = ffi_vector3d_mul(v1.vec, v2.vec);
        return Vec3D(result);
    };

    friend Vec3D operator/(const Vec3D &v1, const Vec3D &v2)
    {
        auto result = ffi_vector3d_div(v1.vec, v2.vec);
        return Vec3D(result);
    };

    friend Vec3D operator*(const Vec3D &v1, double d)
    {
        auto result = ffi_vector3d_mul_f64(v1.vec, d);
        return Vec3D(result);
    };

    friend Vec3D operator/(const Vec3D &v1, double d)
    {
        auto result = ffi_vector3d_div_f64(v1.vec, d);
        return Vec3D(result);
    };

    const double Length() const
    {
        return ffi_vector3d_length(vec);
    }

    const double LengthSQ() const
    {
        return ffi_vector3d_length_sq(vec);
    }

    Vec3D normal() const
    {
        auto result = ffi_vector3d_normal(vec);
        return Vec3D(result);
    }

    Vec3D cross(const Vec3D &v2) const
    {
        auto result = ffi_vector3d_cross(vec, v2.vec);
        return Vec3D(result);
    }

    const double dot(const Vec3D &v2) const
    {
        return ffi_vector3d_dot(vec, v2.vec);
    }
};

struct Vec4F
{
  private:
    VecWrapper4F *vec;

    Vec4F(VecWrapper4F *vec)
    {
        this->vec = vec;
    }

  public:
    Vec4F(float x, float y, float z, float w)
    {
        vec = ffi_new_4f_vector(x, y, z, y);
    }

    ~Vec4F()
    {
        ffi_free_4f_vector(vec);
    }

    const double X() const
    {
        return ffi_vector4f_x(vec);
    }

    const double Y() const
    {
        return ffi_vector4f_y(vec);
    }

    const double Z() const
    {
        return ffi_vector4f_z(vec);
    }

    const double W() const
    {
        return ffi_vector4f_z(vec);
    }
};
}