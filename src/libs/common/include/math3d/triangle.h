//============================================================================================
// Spirenkov Maxim, 2003
//============================================================================================
// StormTriangle
//============================================================================================

#pragma once

#pragma pack(push, 1)

#include "plane.h"
#include "vector.h"

// Class for representing a triangle in 3D space
class StormTriangle
{
  public:
    union {
        struct
        {
            // First vertex
            Vector p1;
            // Second vertex
            Vector p2;
            // Third vertex
            Vector p3;
        };

        struct
        {
            // Array of vertices
            Vector p[3];
        };
    };

    // -----------------------------------------------------------
    // Constructors
    // -----------------------------------------------------------
  public:
    // Empty constructor
    StormTriangle(){};
    // Copy constructor
    StormTriangle(const StormTriangle &t);
    StormTriangle(const Vector *v);

    // -----------------------------------------------------------
    // Operators
    // -----------------------------------------------------------
  public:
    // per component multiplication with assignment
    StormTriangle &operator*=(const Vector &v);

    // -----------------------------------------------------------
    // Utilities
    // -----------------------------------------------------------
  public:
    // Get normal
    Vector GetNormal() const;
    // Get midpoint
    Vector GetCenter() const;
    // Get triangle
    Plane GetPlane() const;
    // The plane passing through the face (p [start], p [NextIndex (start)])
    Plane OrtoPlane(int32_t start) const;
    // The plane passing through the face (p [0], p [1])
    Plane OrtoPlane01() const;
    // The plane passing through the face (p [1], p [2])
    Plane OrtoPlane12() const;
    // The plane passing through the face (p [2], p [0])
    Plane OrtoPlane20() const;
    // Get previous index of triangle vertex
    static int32_t PrevIndex(int32_t index);
    // Get the next vertex index of a triangle
    static int32_t NextIndex(int32_t index);

    // Find the nearest point in a triangle to a given one, lying in the plane of the triangle, true - inside the
    // triangle
    bool FindClosestPoint(const Vector &trgNormal, Vector &pointOnPlane) const;

    // Convert vertex coordinates
    StormTriangle &Transform(const Matrix &mtx);

    enum CoIntersectionResult
    {
        cir_none = 0,
        // Do not intersect
        cir_deg_cur,
        // The current triangle is degenerate
        cir_deg_t,
        // This triangle is degenerate
        cir_coplanar,
        // Do not intersect but lie in the same plane
        cir_intersection,
        // They lie in the same plane, intersect, but do not coincide
        cir_equal,
        // The triangles match
    };

    // Checking triangles for intersections in one plane
    CoIntersectionResult IsCoplanarIntersection(const StormTriangle &t, float intsEps = 0.0000001f) const;

  private:
    static int32_t z_sysClipTriangleEdgePlane(Plane plane, Vector src[8], Vector dst[8], int32_t count);
};

// ===========================================================
// Constructors
// ===========================================================

// Empty constructor
inline StormTriangle::StormTriangle(const StormTriangle &t)
{
    p1 = t.p1;
    p2 = t.p2;
    p3 = t.p3;
}

// Fill all components
inline StormTriangle::StormTriangle(const Vector *v)
{
    p[0] = v[0];
    p[1] = v[1];
    p[2] = v[2];
}

// ===========================================================
// Operators
// ===========================================================

// per component multiplication with assignment
inline StormTriangle &StormTriangle::operator*=(const Vector &v)
{
    p1 *= v;
    p2 *= v;
    p3 *= v;
    return *this;
}

/*!\relates StormTriangle
Multiply triangle by matrix
*/
inline StormTriangle operator*(const Matrix &mtx, const StormTriangle &t)
{
    StormTriangle trg;
    trg.p1 = mtx * t.p1;
    trg.p2 = mtx * t.p2;
    trg.p3 = mtx * t.p3;
    return trg;
}

/*!\relates StormTriangle
Multiply triangle by matrix
*/
inline StormTriangle operator*(const StormTriangle &t, const Matrix &mtx)
{
    StormTriangle trg;
    trg.p1 = mtx * t.p1;
    trg.p2 = mtx * t.p2;
    trg.p3 = mtx * t.p3;
    return trg;
}

// ===========================================================
// Utilities
// ===========================================================

// Get normal
inline Vector StormTriangle::GetNormal() const
{
    return !((p1 - p2) ^ (p1 - p3));
}

// Get midpoint
inline Vector StormTriangle::GetCenter() const
{
    return (p1 + p2 + p3) * 0.33333333333333f;
}

// Get triangle
inline Plane StormTriangle::GetPlane() const
{
    return Plane(GetNormal(), p1);
}

// Plane passing through the face (p [start% 2], p [(start + 1)% 2])
inline Plane StormTriangle::OrtoPlane(int32_t start) const
{
    Plane plane;
    // Vertices
    const auto &ps = p[start % 3];
    const auto &pe = p[NextIndex(start)];
    // Normal
    plane.n = !(((p3 - p1) ^ (p1 - p2)) ^ (ps - pe));
    // Distance
    plane.Move(ps);
    return plane;
}

// The plane passing through the face (p [0], p [1])
inline Plane StormTriangle::OrtoPlane01() const
{
    return OrtoPlane(0);
}

// The plane passing through the face (p [1], p [2])
inline Plane StormTriangle::OrtoPlane12() const
{
    return OrtoPlane(1);
}

// The plane passing through the face (p [2], p [0])
inline Plane StormTriangle::OrtoPlane20() const
{
    return OrtoPlane(2);
}

// Get previous index of triangle vertex
inline int32_t StormTriangle::PrevIndex(int32_t index)
{
    index--;
    if (index < 0)
        index = 2;
    if (index > 2)
        index = 2;
    return index;
}

// Get the next vertex index of a triangle
inline int32_t StormTriangle::NextIndex(int32_t index)
{
    index++;
    if (index < 0)
        index = 0;
    if (index > 2)
        index = 0;
    return index;
}

// Find the nearest point in a triangle to a given one, lying in the plane of the triangle, true - inside the triangle
inline bool StormTriangle::FindClosestPoint(const Vector &trgNormal, Vector &pointOnPlane) const
{
    const Vector *cPoint = nullptr;
    for (int32_t i = 0; i < 3; i++)
    {
        // Edge
        const auto &ps = p[i];
        const auto &pe = p[i + 1 < 3 ? i + 1 : 0];
        auto edge = pe - ps;
        // If a triangle has an invalid edge size, don't test it any more
        const auto edgeLen = edge.Normalize();
        if (edgeLen < 1e-37f)
            return false;
        // Orthogonal plane
        Plane orto(edge ^ trgNormal, ps);
        // Determine the position of the point
        const auto distToEdge = orto.Dist(pointOnPlane);
        if (distToEdge > 0.0f)
        {
            // Determine the length of the projection (pointOnPlane - ps) on the edge
            const auto prjLength = edge | (pointOnPlane - ps);
            if (prjLength < 0.0f)
            {
                cPoint = &ps;
            }
            else if (prjLength > edgeLen)
            {
                cPoint = &pe;
            }
            else
            {
                pointOnPlane = ps + (pe - ps) * (prjLength / edgeLen);
                return false;
            }
        }
    }
    if (cPoint)
    {
        pointOnPlane = *cPoint;
        return false;
    }
    return true;
}

// Convert vertex coordinates
inline StormTriangle &StormTriangle::Transform(const Matrix &mtx)
{
    p1 = mtx.MulVertex(p1);
    p2 = mtx.MulVertex(p2);
    p3 = mtx.MulVertex(p3);
    return *this;
}

// Checking triangles for intersections in one plane
inline StormTriangle::CoIntersectionResult StormTriangle::IsCoplanarIntersection(const StormTriangle &t, float intsEps) const
{
    // Checking the dimensions of the triangles
    if (~(p1 - p2) < intsEps * intsEps || ~(p2 - p3) < intsEps * intsEps || ~(p3 - p1) < intsEps * intsEps)
        return cir_deg_cur;
    if (~(t.p1 - t.p2) < intsEps * intsEps || ~(t.p2 - t.p3) < intsEps * intsEps || ~(t.p3 - t.p1) < intsEps * intsEps)
        return cir_deg_t;
    // get the normals
    auto n = (p1 - p2) ^ (p1 - p3);
    if (n.Normalize() < 0.0000001f)
        return cir_deg_cur;
    auto nt = (t.p1 - t.p2) ^ (t.p1 - t.p3);
    if (nt.Normalize() < 0.0000001f)
        return cir_deg_t;
    // Checking coplanarity
    const auto cs = n | nt;
    static const auto cosMin = cosf(0.5f * 3.141592654f / 180.0f);
    if (cs < cosMin)
        return cir_none;
    // Distance between planes
    const auto d = n | p1;
    const auto dt = n | t.p1;
    if (fabs(d - dt) > intsEps)
        return cir_none;
    // Check for a match
    if (~(p1 - t.p1) + ~(p2 - t.p2) + ~(p3 - t.p3) < intsEps * intsEps)
        return cir_equal;
    if (~(p2 - t.p1) + ~(p3 - t.p2) + ~(p1 - t.p3) < intsEps * intsEps)
        return cir_equal;
    if (~(p3 - t.p1) + ~(p1 - t.p2) + ~(p2 - t.p3) < intsEps * intsEps)
        return cir_equal;
    // Check for intersection by clipping
    static Vector poly1[8], poly2[8];
    poly1[0] = t.p1;
    poly1[1] = t.p2;
    poly1[2] = t.p3;
    int32_t count = 3;
    count = z_sysClipTriangleEdgePlane(Plane(!(n ^ (p1 - p2)), p1), poly1, poly2, count);
    if (!count)
        return cir_coplanar;
    count = z_sysClipTriangleEdgePlane(Plane(!(n ^ (p2 - p3)), p2), poly2, poly1, count);
    if (!count)
        return cir_coplanar;
    count = z_sysClipTriangleEdgePlane(Plane(!(n ^ (p3 - p1)), p3), poly1, poly2, count);
    if (!count)
        return cir_coplanar;
    for (int32_t s = 0; s < count; s++)
    {
        const auto e = s + 1 < count ? s + 1 : 0;
        const auto dist = ~(poly2[e] - poly2[s]);
        if (dist < intsEps * intsEps)
            return cir_coplanar;
    }
    return cir_intersection;
}

inline int32_t StormTriangle::z_sysClipTriangleEdgePlane(Plane plane, Vector src[8], Vector dst[8], int32_t count)
{
    float ds = plane * src[0], de;
    int32_t c = 0;
    for (int32_t s = 0; s < count; s++, ds = de)
    {
        // If in the area, add a vertex
        if (ds <= 0.0f)
            dst[c++] = src[s];
        // Index of next
        const auto e = s + 1 < count ? s + 1 : 0;
        // Distance to plane
        de = plane * src[e];
        // If on the one side, then continue
        if (ds * de >= 0.0f)
            continue;
        // There is an intersection
        dst[c++] = src[s] + (src[e] - src[s]) * (ds / (ds - de));
    }
    if (c < 3)
        c = 0;
    return c;
}

#pragma pack(pop)
