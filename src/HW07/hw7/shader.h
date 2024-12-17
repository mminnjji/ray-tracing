#pragma once

//#include <glm/glm.hpp>
#include <glad/glad.h>
#include <stdlib.h> 
#include "raytraceData.h"
#include "tracer.h"

# define EPSILON 1e-6

class shader {
public: 
	shader();
	~shader();

	raytraceData::material* makeMaterial(GLfloat r, GLfloat g, GLfloat b, GLfloat amb, GLfloat diff, GLfloat spec, GLfloat shininess, GLfloat reflectivity, GLfloat refractive_index, GLfloat transparency);
	void shade(raytraceData::point* p, raytraceData::vector* n, raytraceData::material* m, raytraceData::color* c, raytraceData::light* l, raytraceData::point* c_origin, int max_depth);
	int isShadow(raytraceData::light l, raytraceData::point p);
	void traceRay(raytraceData::point* origin, raytraceData::vector* direction, raytraceData::light* l, int depth, raytraceData::color* output_color); 
	raytraceData::sphere* s1; /* the scene: two sphere one cylinder */
	raytraceData::sphere* s2;		   
	raytraceData::cylinder* cy1;
	raytraceData::cylinder* cy2;
	raytraceData::plane* pl;

	tracer tracer;
};
