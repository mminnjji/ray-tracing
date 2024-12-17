#include <glm/glm.hpp>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include "raytraceData.h"

using namespace glm;
using namespace raytraceData;


void vset(point* point, GLfloat x, GLfloat y, GLfloat z) {
    point->x = x;
    point->y = y;
    point->z = z;
    point->w = 1.0;
}

GLfloat vlength3(const point& p1, const point& p2) {
    GLfloat x = (p1.x - p2.x) * (p1.x - p2.x);
    GLfloat y = (p1.y - p2.y) * (p1.y - p2.y);
    GLfloat z = (p1.z - p2.z) * (p1.z - p2.z);
    return x + y + z;
}

GLfloat vlength2(const point& p) {
    return p.x * p.x + p.y * p.y + p.z * p.z;
}

GLfloat vlength(const point& p) {
    return sqrt(vlength2(p));
}

point vplus(const point& p1, const point& p2) {
    return {p1.x + p2.x, p1.y + p2.y, p1.z + p2.z, 1.0};
}

point vplus_(const point& p, GLfloat x, GLfloat y, GLfloat z) {
    return {p.x + x, p.y + y, p.z + z, 1.0};
}

point vminus(const point& p1, const point& p2) {
    return {p1.x - p2.x, p1.y - p2.y, p1.z - p2.z, 1.0};
}

point vminus_(const point& p, GLfloat x, GLfloat y, GLfloat z) {
    return {p.x - x, p.y - y, p.z - z, 1.0};
}

point vmult(const point& p, GLfloat t) {
    return {p.x * t, p.y * t, p.z * t, 1.0};
}

point vmult_(const point& p1, const point& p2) {
    return {p1.x * p2.x, p1.y * p2.y, p1.z * p2.z, 1.0};
}

point vdivide(const point& p, GLfloat t) {
    return {p.x / t, p.y / t, p.z / t, 1.0};
}

GLfloat vdot(const point& p1, const point& p2) {
    return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
}

point vcross(const point& p1, const point& p2) {
    return {
        p1.y * p2.z - p1.z * p2.y,
        p1.z * p2.x - p1.x * p2.z,
        p1.x * p2.y - p1.y * p2.x,
        1.0
    };
}

point vunit(const point& p) {
    GLfloat len = vlength(p);
    if (len == 0.0) {
        printf("Error: Divider is 0\n");
        exit(0);
    }
    return {p.x / len, p.y / len, p.z / len, 1.0};
}

point vmin(const point& p1, const point& p2) {
    GLfloat x = p1.x < p2.x ? p1.x : p2.x;
    GLfloat y = p1.x < p2.x ? p1.x : p2.x;
    GLfloat z = p1.x < p2.x ? p1.x : p2.x;
    return { x, y, z, 1.0 };
}

point ctop(color color)
{
    return {color.r, color.g, color.b, 1.0};
}

color ptoc(point point)
{
    return {point.x, point.y, point.z};
}

point ray_at(ray* r, float t)
{
    vector ray_dir = *(r->end);
    point at = vplus(*(r->start), vmult(ray_dir, t));
    at.w = 1.0;
    return (at);
}
