#include "raytrace.h"
#include "shader.h"
#include "vec_util.h"
#include <math.h>

using namespace raytraceData;

raytrace::raytrace(int w, int h)
{
	/* low-level graphics setup */
	lowlevel.initCanvas(w, h);

	/* raytracer setup */
	initCamera(w, h);
	initScene();
}

raytrace::~raytrace()
{
	if (s1 != NULL) delete s1;
	if (viewpoint != NULL) delete viewpoint;
}

GLubyte* raytrace::display(void)
{	
	drawScene();  /* draws the picture in the canvas */

	return lowlevel.flushCanvas();  /* draw the canvas to the OpenGL window */	
}

void raytrace::initScene()
{
	s1 = makeSphere(-0.23, 0.0, -2.0, 0.2);
	s1->m = shader.makeMaterial(1.0, 0.1, 0.0, 0.1, 0.7, 0.5, 50);
	s2 = makeSphere(0.2, -0.1, -1.5, 0.1);
	s2->m = shader.makeMaterial(0.0, 0.75, 0.85, 0.1, 0.7, 0.5, 50);
	pl = makePlane(0.0, 0.2, -2.0, *(makePoint(0, -1, 0, 1)));
	pl->m = shader.makeMaterial(0.4118, 0.5020, 0.6078, 0.1, 0.7, 0.5, 50);
	cy = makeCylinder(0.1, 0.0, -3.0, 0.1, *(makePoint(0.0, 1, 0., 1)), 0.4);
	cy->m = shader.makeMaterial(0.6, 0.5, 0.0, 0.1, 0.7, 0.5, 50);
	// s1 = NULL;
	// s2 = NULL;
	// pl= NULL;
	// cy = NULL;

	tracer.s1 = s1;
	tracer.s2 = s2;
	tracer.pl = pl;
	tracer.cy = cy;
	shader.s1 = s1;
	shader.s2 = s2;
	shader.pl = pl;
	shader.cy = cy;
	shader.tracer.s1 = s1;
	shader.tracer.s2 = s2;
	shader.tracer.cy = cy;
	shader.tracer.pl = pl;
}

void raytrace::initCamera(int w, int h)
{
	viewpoint = makePoint(0.0, 0.0, 0.0, 1.0);
	pnear = -1.0;
	fovx = M_PI / 6;
	/* window dimensions */
	width = w;
	height = h;
}

void raytrace::drawScene(void)
{
	int i, j;
	GLfloat imageWidth;

	/* declare data structures on stack to avoid malloc & free */
	point worldPix;  /* current pixel in world coordinates */
	point direction;
	ray r;
	color c;

	/* z and w are constant for all pixels */
	worldPix.w = 1.0;
	worldPix.z = pnear;

	r.start = &worldPix;
	r.end = &direction;

	imageWidth = 2 * pnear * tan(fovx / 2);

	/* trace a ray for every pixel */
	for (i = 0; i < width; i++) {
		for (j = 0; j < height; j++) {

			/* find position of pixel in world coordinates */
			/* y position = (pixel height/middle) scaled to world coords */
			worldPix.y = (j - (height / 2)) * imageWidth / width;
			/* x position = (pixel width/middle) scaled to world coords */
			worldPix.x = (i - (width / 2)) * imageWidth / width;

			/* find direction */
			/* note: direction vector is NOT NORMALIZED */
			calculateDirection(viewpoint, &worldPix, &direction);

			/* Find color for pixel */
			rayColor(&r, &c);
			/* write the pixel! */
			lowlevel.drawPixel(i, j, c.r, c.g, c.b);
		}
	}
}

point* raytrace::makePoint(GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
	point* p;
	/* allocate memory */	
	p = new point();
	/* put stuff in it */
	p->x = x; p->y = y; p->z = z; p->w = w;
	return (p);
}

sphere* raytrace::makeSphere(GLfloat x, GLfloat y, GLfloat z, GLfloat r) {
	sphere* s;
	/* allocate memory */
	s = new sphere();

	/* put stuff in it */
	s->c = makePoint(x, y, z, 1.0);   /* center */
	s->r = r;   /* radius */
	s->m = NULL;   /* material */
	return(s);
}

cylinder* raytrace::makeCylinder(GLfloat x, GLfloat y, GLfloat z, GLfloat r, vector v, GLfloat h) {
	cylinder* cy;
	/* allocate memory */
	cy = new cylinder();

	/* put stuff in it */
	cy->center = makePoint(x, y, z, 1.0);   /* center */
	cy->radius = r;   /* radius */
	cy->height = h; /* height */
	cy->normal = v; /* normal */
	cy->m = NULL;   /* material */
	return(cy);
}

plane* raytrace::makePlane(GLfloat x, GLfloat y, GLfloat z, vector v) {
	plane* p;
	/* allocate memory */
	p = new plane();

	/* put stuff in it */
	p->center = makePoint(x, y, z, 1.0);   /* center */
	p->normal = v;   /* normal */
	p->m = NULL;   /* material */
	return(p);
}

light* raytrace::makeLight(point* light_origin, color light_color, GLfloat bright_ratio)
{
	light* l;

	/*allocate memory*/
	l = new light();

	/*put initial value*/
	l->b_r = bright_ratio;
	l->orig = light_origin;
	l->l_c = light_color;
	return(l);
}

/* returns the color seen by ray r in parameter c */
void raytrace::rayColor(ray* r, color* c) {
	point p;  /* first intersection point */
	vector n;
	material* m;
	light* l;

	/////light initializatin
	// light* makeLight(raytraceData::point* light_origin, raytraceData::color light_color, GLfloat bright_ratio);
	l = makeLight(makePoint(5, -10, 10, 1), {1, 1, 1}, 1.0);

	p.w = 0.0;  /* inialize to "no intersection" */
	int type = tracer.trace(r, &p, &n, &m, INFINITY);

	if (p.w != 0.0) {
		shader.shade(&p, &n, m, c, l, viewpoint, type);  /* do the lighting calculations */
	}
	else {             /* nothing was hit */
		c->r = 1.0;
		c->g = 1.0;
		c->b = 1.0;
	}
}

/* vector from point p to point q is returned in v */
void raytrace::calculateDirection(point* p, point* q, point* v) {
	float length;

	v->x = q->x - p->x;
	v->y = q->y - p->y;
	v->z = q->z - p->z;
	/* a direction is a point at infinity */
	v->w = 0.0;
	v = &(vunit(*v));
}
