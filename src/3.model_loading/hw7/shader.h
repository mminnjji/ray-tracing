#pragma once

//#include <glm/glm.hpp>
#include <glad/glad.h>
#include <stdlib.h> 
#include "raytraceData.h"

class shader {
public: 
	shader();
	~shader();

	raytraceData::material* makeMaterial(GLfloat r, GLfloat g, GLfloat b, GLfloat amb);
	void shade(raytraceData::point* p, raytraceData::vector* n, raytraceData::material* m, raytraceData::color* c, raytraceData::light* l, raytraceData::point* c_origin);
};
