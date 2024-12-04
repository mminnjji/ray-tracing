#pragma once

#include <glad/glad.h>

/* constants */
#define TRUE 1
#define FALSE 0

#define M_PI 3.1415926535897932384626433832795029

namespace raytraceData {

	/* data structures */
	typedef struct point {
		GLfloat x;
		GLfloat y;
		GLfloat z;
		GLfloat w;
	} point;

	/* a vector is just a point */
	typedef point vector;

	/* a line segment */
	typedef struct segment {
		point* start;
		point* end;
	} segment;

	/* a ray is just a segment with an endpoint at infinity */
	typedef segment ray;

	typedef struct color {
		GLfloat r;
		GLfloat g;
		GLfloat b;
		/* these should be between 0 and 1 */
	} color;

	typedef struct material {
		/* color */
		color c;
		/* ambient reflectivity */
		GLfloat amb;
		GLfloat diff;
		GLfloat spec;
		GLfloat shininess;
	} material;

	typedef struct light {
		/*light origin*/
		point *orig;
		/*light color*/
		color l_c;
		/*bright ratio*/
		GLfloat b_r;
	} light;

	typedef struct sphere {
		point* c;  /* center */
		GLfloat r;  /* radius */
		material* m;
	} sphere;

	typedef struct cylinder {
		point*		center;
		vector		normal;
		GLfloat		radius;
		GLfloat		height;
		material*	m;
	} cylinder;

	typedef struct plane {
		point	*center;
		vector	normal;
		material* m;
	} plane;

	typedef struct rec {
		material* m;
		vector	normal;
		point	p;
		float	t;
		float	tmax;
		float	tmin;
	} rec;

}