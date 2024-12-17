#ifndef POINT_FUNCTIONS_H
#define POINT_FUNCTIONS_H

#include <glm/glm.hpp>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include "raytraceData.h"

using namespace glm;
using namespace raytraceData;

void vset(point* point, GLfloat x, GLfloat y, GLfloat z);
GLfloat vlength3(const point& p1, const point& p2);
GLfloat vlength2(const point& p);
GLfloat vlength(const point& p);
point vplus(const point& p1, const point& p2);
point vplus_(const point& p, GLfloat x, GLfloat y, GLfloat z);
point vminus(const point& p1, const point& p2);
point vminus_(const point& p, GLfloat x, GLfloat y, GLfloat z);
point vmult(const point& p, GLfloat t);
point vmult_(const point& p1, const point& p2);
point vdivide(const point& p, GLfloat t);
GLfloat vdot(const point& p1, const point& p2);
point vcross(const point& p1, const point& p2);
point vunit(const point& p);
point vmin(const point& p1, const point& p2);
point ctop(color color);
color ptoc(point point);
point ray_at(ray* r, float t);

#endif // POINT_FUNCTIONS_H
