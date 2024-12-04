#pragma once

#include "raytraceData.h"

class tracer {
public:
	tracer();
	~tracer();

	void findPointOnRay(raytraceData::ray* r, float t, raytraceData::point* p);
	int raySphereIntersect(raytraceData::ray* r, raytraceData::sphere* s, raytraceData::rec* rc);
	int rayCylinderIntersect(raytraceData::ray* r, raytraceData::cylinder* cy, raytraceData::rec* rc);
	int rayPlaneIntersect(raytraceData::ray* r, raytraceData::plane* p, raytraceData::rec* rc);
	int realHit(raytraceData::ray* r, raytraceData::sphere* s1, raytraceData::sphere* s2, raytraceData::cylinder* cy, raytraceData::rec* rc);
	void trace(raytraceData::ray* r, raytraceData::point* p, raytraceData::vector* n, raytraceData::material** m);
	raytraceData::sphere* s1; /* the scene: two sphere one cylinder */
	raytraceData::sphere* s2;		   
	raytraceData::cylinder* cy;
	raytraceData::plane* p;
};