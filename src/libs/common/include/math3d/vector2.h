//============================================================================================
// Aleksey Komarov, 2022
//============================================================================================
// Vector2
//============================================================================================

#pragma once

#pragma pack(push, 1)

// Class for representing a vector in 2D space
struct Vector2
{
  public:
    union {
        struct
        {
            // The X component
            float x;
            // The Y component
            float y;
        };

        // Array representation
        float v[2];
    };

    // -----------------------------------------------------------
    // Constructors
    // -----------------------------------------------------------
  public:
    // Empty constructor
    Vector2();
    // Fill with number
    Vector2(float f);
    // Fill with number
    Vector2(double d);
    // Fill all components
    Vector2(float fx, float fy);
    // Fill all components
    Vector2(const float f[2]);
    // Fill all components
    Vector2(const double d[2]);
    // Copy constructor
    Vector2(const Vector2 &v);

    // -----------------------------------------------------------
    // Operators
    // -----------------------------------------------------------
  public:
    // Cast
    operator float *();
    // Cast
    operator const float *() const;

    // Find the square of the length of a vector
    float operator~() const;

    // Get negative vector
    Vector2 operator-() const;

    // Assign
    Vector2 &operator=(float f);
    // Assign
    Vector2 &operator=(double d);
    // Assign
    Vector2 &operator=(const Vector2 &v);

    // -----------------------------------------------------------
    // Transformation
    // -----------------------------------------------------------
  public:
    // Normalize the vector, and return it to its former length
    float Normalize();
};

// ===========================================================
// Constructors
// ===========================================================

// Empty constructor
inline Vector2::Vector2()
{
}

// Fill with number
inline Vector2::Vector2(float f)
{
    x = f;
    y = f;
}

// Fill with number
inline Vector2::Vector2(double d)
{
    x = static_cast<float>(d);
    y = static_cast<float>(d);
}

// Fill all components
inline Vector2::Vector2(float fx, float fy)
{
    x = fx;
    y = fy;
}

// Fill all components
inline Vector2::Vector2(const float f[2])
{
    x = f[0];
    y = f[1];
}

// Fill all components
inline Vector2::Vector2(const double d[2])
{
    x = static_cast<float>(d[0]);
    y = static_cast<float>(d[1]);
}

// Copy constructor
inline Vector2::Vector2(const Vector2 &v)
{
    x = v.x;
    y = v.y;
}

// ===========================================================
// Operators
// ===========================================================

// Cast
inline Vector2::operator float *()
{
    return (float *)&x;
}

// Cast
inline Vector2::operator const float *() const
{
    return (const float *)&x;
}

// Find the square of the length of a vector
inline float Vector2::operator~() const
{
    return x * x + y * y;
}

// Get negative vector
inline Vector2 Vector2::operator-() const
{
    auto v(*this);
    v.x = -v.x;
    v.y = -v.y;
    return v;
}

// Assign
inline Vector2 &Vector2::operator=(float f)
{
    x = f;
    y = f;
    return *this;
}

// Assign
inline Vector2 &Vector2::operator=(double d)
{
    x = static_cast<float>(d);
    y = static_cast<float>(d);
    return *this;
}

// Assign
inline Vector2 &Vector2::operator=(const Vector2 &v)
{
    x = v.x;
    y = v.y;
    return *this;
}

// ===========================================================
// Transformation
// ===========================================================

// Normalize the vector, and return it to its former length
inline float Vector2::Normalize()
{
    const auto len =
        sqrt(static_cast<double>(x) * static_cast<double>(x) + static_cast<double>(y) * static_cast<double>(y));
    if (len > 0.0)
    {
        const auto k = 1.0 / len;
        x = static_cast<float>(x * k);
        y = static_cast<float>(y * k);
    }
    return static_cast<float>(len);
}

#pragma pack(pop)
