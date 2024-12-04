#include "tracer.h"

#include <math.h>
#include "vec_util.h"

using namespace raytraceData;

tracer::tracer():
	s1(NULL)
{
}

tracer::~tracer()
{
}

/* point on ray r parameterized by t is returned in p */
void tracer::findPointOnRay(ray* r, float t, point* p) {
	p->x = r->start->x + t * r->end->x;
	p->y = r->start->y + t * r->end->y;
	p->z = r->start->z + t * r->end->z;
	p->w = 1.0;
}

int	hit_cylinder_d(cylinder* cy, ray *ray, rec* rc)
{
	point	dc;
	vector		odc;
	vector ray_dir = *(ray->end);
	double		droot;

	dc = vplus(*(cy->center), vmult(cy->normal, cy->height * -0.5));
	odc = vminus(dc, *(ray->start));
	droot = vdot(vmult(cy->normal, -1), ray_dir);
	if (droot != 0)
		droot = vdot(odc, vmult(cy->normal, -1)) / droot;
	else
		return (0);
	if (droot >= rc->tmin && rc->tmax >= droot && \
	vlength3(dc, ray_at(ray, droot)) <= cy->radius * cy->radius)
	{
		rc->t = droot;
		rc->p = ray_at(ray, droot);
		rc->normal = vunit(vmult(cy->normal, -1));
		rc->m = cy->m;
	}
	else
		return (0);
	return (1);
}

int	hit_cylinder_u(cylinder* cy, ray *ray, rec* rc)
{
	point		uc;
	vector		ouc;
	vector ray_dir = *(ray->end);
	double		uroot;

	uc = vplus(*(cy->center), vmult(cy->normal, cy->height * 0.5));
	ouc = vminus(uc, *(ray->start));
	uroot = vdot(cy->normal, ray_dir);
	if (uroot != 0)
		uroot = vdot(ouc, cy->normal) / uroot;
	else
		return (0);
	if (uroot >= rc->tmin && rc->tmax >= uroot && \
	vlength3(uc, ray_at(ray, uroot)) <= cy->radius * cy->radius)
	{
		rc->t = uroot;
		rc->p = ray_at(ray, uroot);
		rc->normal = vunit(cy->normal);
		rc->m = cy->m;
	}
	else
		return (0);
	return (1);
}

int	hitCylinderUD(cylinder* cy, ray *ray, rec* rc)
{
	int	a;
	int	b;

	a = hit_cylinder_u(cy, ray, rc);
	if (a)
		rc->tmax = rc->t;
	b = hit_cylinder_d(cy, ray, rc);
	if (a || b)
		return (1);
	return (0);
}

/*rayCylinderIntersect*/
int tracer::rayCylinderIntersect(ray* r, cylinder* cy, rec* rc) {
	vector oc = vminus(*(cy->center), *(r->start));
	vector ray_dir = *(r->end);
	vector uv = vminus(vmult(cy->normal, vdot(cy->normal, ray_dir)), ray_dir);
	vector dv = vminus(oc, vmult(cy->normal, vdot(cy->normal, oc)));

	GLfloat uv2 = vdot(uv, uv);
	GLfloat dv2 = 2 * vdot(uv, dv);
	GLfloat uvdv = dv2 * dv2 - 4 * uv2 * (vdot(dv, dv) - cy->radius * cy->radius);
	if (uvdv < 0)
		return (0);
	GLfloat root = (-1 * dv2 - sqrt(uvdv)) / (2 * uv2);
	GLfloat rvf = vdot(cy->normal, vminus(vmult(ray_dir, root), oc));

	if (root < rc->tmin || rc->tmax < root || rvf >= cy->height / 2 || rvf <= cy->height / 2 * -1)
	{
		GLfloat isUD = hitCylinderUD(cy, r, rc);
		root = (-dv2 + sqrt(uvdv)) / (2 * uv2);
		if (isUD && root > isUD)
			return (1);
		rvf = vdot(cy->normal, vminus(vmult(ray_dir, root), oc));
		if (root < rc->tmin || rc->tmax < root \
		|| rvf >= cy->height / 2 || rvf <= cy->height / 2 * -1)
			return (0);
	}
	rc->t = root;
	rc->p = ray_at(r, root);
	rc->normal = vunit(vminus(rc->p, vplus(*(cy->center), vmult(cy->normal, rvf))));
	rc->m = cy->m;
	return (1);
}

int tracer::rayPlaneIntersect(ray* r, plane* p, rec* rc)
{
	vector	oc;
	float	root;
	vector ray_dir = *(r->end);

	oc = vminus(*(p->center), *(r->start));
	root = vdot(p->normal, ray_dir);
	if (root != 0)
		root = vdot(oc, p->normal) / root;
	else
		return (0);
	if (root < rc->tmin || rc->tmax < root)
		return (0);
	rc->t = root;
	rc->normal = vunit(p->normal);
	rc->m = p->m;
	rc->tmax = root;
	return (1);
}


/* raySphereIntersect */
/* returns TRUE if ray r hits sphere s, with parameter value in t */
int tracer::raySphereIntersect(ray* r, sphere* s, rec* rc) {
	point p;   /* start of transformed ray */
	float a, b, c;  /* coefficients of quadratic equation */
	float D;    /* discriminant */
	point* v;
	float root;

	/* transform ray so that sphere center is at origin */
	/* don't use matrix, just translate! */
	p.x = r->start->x - s->c->x;
	p.y = r->start->y - s->c->y;
	p.z = r->start->z - s->c->z;
	v = r->end; /* point to direction vector */


	a = v->x * v->x + v->y * v->y + v->z * v->z;
	b = 2 * (v->x * p.x + v->y * p.y + v->z * p.z);
	c = p.x * p.x + p.y * p.y + p.z * p.z - s->r * s->r;

	D = b * b - 4 * a * c;

	if (D < 0) {  /* no intersection */
		return (FALSE);
	}
	else {
		D = static_cast<float>(sqrt(D));
		/* First check the root with the lower value of t: */
		/* this one, since D is positive */
		root = (-b - D) / (2 * a);
		/* ignore roots which are less than zero (behind viewpoint) */
		if (root < 0) {
			root = (-b + D) / (2 * a);
		}
		if (root < 0 || root < rc->tmin || rc->tmax < root) {return(FALSE);}
		else{
			rc->t = root;
			rc->p = ray_at(r, root);
			rc->normal = vunit(vminus(rc->p, *(s->c)));
			rc->m = s->m;
			rc->tmax = root;
			return(1);
		}
	}
}

int tracer::realHit(ray* r, sphere* s1, sphere* s2, cylinder* cy, plane* pl, rec* rc)
{
	float tmax;
	float tmin;
	int h1 = FALSE;
	int h2 = FALSE;
	int h3 = FALSE;
	int h4 = FALSE;
	float tmp = 0;

	h1 = raySphereIntersect(r, s1, rc);
	h2 = raySphereIntersect(r, s2, rc);
	// h3 = rayCylinderIntersect(r, cy, rc);
	h4 = rayPlaneIntersect(r, pl, rc);

	if (h1 && !h2)
		return (1);
	else if (h2 && !h1)
		return (2);
	else if (h4)
		return (3);
	// if ((h1 && h2 && h1 <= h2) || (h1 && !h2))
	// 	return (1);
	// else if ((h1 && h2 && h2 <= h1) || (h2 && !h1))
	// 	return (2);
	// else if (h4 <= h2 && h4 <= h1)
	// 	return (3);
	else
		return (0);
}

void tracer::findSphereNormal(sphere* s, point* p, vector* n) {
	n->x = (p->x - s->c->x) / s->r;
	n->y = (p->y - s->c->y) / s->r;
	n->z = (p->z - s->c->z) / s->r;
	n->w = 0.0;
}

void tracer::findPlaneNormal(plane* p, vector* n) {
	n->x = p->normal.x;
	n->y = p->normal.y;
	n->z = p->normal.z;
	n->w = 0.0;
	n = &(vunit(*n));
}

/* trace */
/* If something is hit, returns the finite intersection point p,
   the normal vector n to the surface at that point, and the surface
   material m. If no hit, returns an infinite point (p->w = 0.0) */
void tracer::trace(ray* r, point* p, vector* n, material** m) {
	rec* record;
	int hit = FALSE;

	record = new rec();

	record->tmin = 0.000000000001;
	record->tmax = INFINITY;

	hit = realHit(r, s1, s2, cy, pl, record);
	if (hit == 1) {
		*m = record->m;
		findPointOnRay(r, record->t, p);
		findSphereNormal(s1, p, n);
		//n = &(record->normal);
	}
	else if (hit == 2) {
		*m = record->m;
		findPointOnRay(r, record->t, p);
		findSphereNormal(s2, p, n);
		//n = &(record->normal);
	}
	else if (hit == 3) {
		*m = record->m;
		findPointOnRay(r, record->t, p);
		findPlaneNormal(pl, n);
		//n = &(record->normal);
	}
	else {
		/* indicates nothing was hit */
		p->w = 0.0;
	}
}