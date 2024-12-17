#pragma once

#include "raytraceData.h"

class tracer {
public:
	tracer();
	~tracer();

	void findPointOnRay(raytraceData::ray* r, float t, raytraceData::point* p);
	int raySphereIntersect(raytraceData::ray* r, raytraceData::sphere* s, raytraceData::rec* rc );
	int rayCylinderIntersect(raytraceData::ray* r, raytraceData::cylinder* cy, raytraceData::rec* rc );
	int rayCylinderUDIntersect(raytraceData::ray* r, raytraceData::cylinder* cy, raytraceData::rec* rc );
	int rayPlaneIntersect(raytraceData::ray* r, raytraceData::plane* p, raytraceData::rec* rc );
	int realHit(raytraceData::ray* r, raytraceData::sphere* s1, raytraceData::sphere* s2, raytraceData::cylinder* cy1, raytraceData::cylinder* cy2, raytraceData::plane* pl, raytraceData::rec* rc);
	int trace(raytraceData::ray* r, raytraceData::point* p, raytraceData::vector* n, raytraceData::material** m, float tmax);
	void findSphereNormal(raytraceData::sphere* s, raytraceData::point* p, raytraceData::vector* n);
	void findPlaneNormal(raytraceData::plane* p, raytraceData::vector* n);
	void findCylinderNormal(raytraceData::rec* r, raytraceData::vector* n);

	raytraceData::sphere* s1; /* the scene: two sphere one cylinder */
	raytraceData::sphere* s2;		   
	raytraceData::cylinder* cy1;
	raytraceData::cylinder* cy2;
	raytraceData::plane* pl;
};