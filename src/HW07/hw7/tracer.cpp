#include "tracer.h"

#include <math.h>
#include "vec_util.h"

using namespace raytraceData;

tracer::tracer() : s1(NULL), s2(NULL), cy1(NULL), cy2(NULL), pl(NULL)
{
}

tracer::~tracer()
{
}

/* point on ray r parameterized by t is returned in p */
void tracer::findPointOnRay(ray *r, float t, point *p)
{
	p->x = r->start->x + t * r->end->x;
	p->y = r->start->y + t * r->end->y;
	p->z = r->start->z + t * r->end->z;
	p->w = 1.0;
}

int tracer::rayCylinderUDIntersect(ray *r, cylinder *cy, rec *rc)
{
	GLfloat uroot;
	GLfloat droot;
	// 윗면의 center
	vector uc = vplus(*(cy->center), vmult(cy->normal, cy->height * 0.5));
	// 아랫면의 center
	vector dc = vplus(*(cy->center), vmult(cy->normal, cy->height * -0.5));

	// ray ~ center 벡터가 법선벡터와 수직
	if (vdot(*(r->end), cy->normal))
	{
		uroot = (vdot(uc, cy->normal) - vdot(*(r->start), cy->normal)) / vdot(*(r->end), cy->normal);
		droot = (vdot(dc, cy->normal) - vdot(*(r->start), cy->normal)) / vdot(*(r->end), cy->normal);

		if (vlength(vminus(ray_at(r, uroot), uc)) > cy->radius && vlength(vminus(ray_at(r, droot), dc)) <= cy->radius) // d만
		{
			if (droot < rc->tmin || rc->tmax < droot)
			{
				return (0);
			}
			rc->t = droot;
			rc->normal = vmult(cy->normal, -1);
			rc->m = cy->m;
			rc->tmax = droot;
			return (1);
		}
		else if (vlength(vminus(ray_at(r, uroot), uc)) <= cy->radius && vlength(vminus(ray_at(r, droot), dc)) > cy->radius) // u만
		{
			if (uroot < rc->tmin || rc->tmax < uroot)
			{
				return (0);
			}
			rc->t = uroot;
			rc->normal = cy->normal;
			rc->m = cy->m;
			rc->tmax = uroot;
			return (1);
		}
		else if (vlength(vminus(ray_at(r, uroot), uc)) <= cy->radius && vlength(vminus(ray_at(r, droot), dc)) <= cy->radius)
		{
			GLfloat root = uroot < droot ? uroot : droot;
			if (root < rc->tmin || rc->tmax < root)
			{
				return (0);
			}
			rc->t = root;
			rc->normal = root == uroot ? cy->normal : vmult(cy->normal, -1);
			rc->m = cy->m;
			rc->tmax = root;
			return (1);
		}
		return (0);
	}
	else
	{
		return (0);
	}
}

/*rayCylinderIntersect*/
int tracer::rayCylinderIntersect(ray *r, cylinder *cy, rec *rc)
{
    GLfloat root;
    cy->normal = vunit(cy->normal);

    // 레이와 원기둥 축의 직교 성분(ad) 및 bd 계산
    vector ad = vminus(*(r->end), vmult(cy->normal, vdot(cy->normal, *(r->end))));
    vector bd = vplus(vminus(vminus(*(r->start), *(cy->center)), 
                  vmult(cy->normal, vdot(cy->normal, *(r->start)))), 
                  vmult(cy->normal, vdot(cy->normal, *(cy->center))));

    // 2차 방정식의 계수
    float a = vdot(ad, ad);
    float b = 2 * vdot(ad, bd);
    float c = vdot(bd, bd) - cy->radius * cy->radius;

    // 판별식 계산
    float pbs = b * b - 4 * a * c;
    if (pbs < 0)
        return (0); // 교차 없음

    // 작은 root부터 검사
    root = (-1 * b - sqrt(pbs)) / (2 * a);
    vector cp = vminus(ray_at(r, root), *(cy->center)); // 교차점 - 원기둥 중심
    vector cp_ = vmult(cy->normal, vdot(cp, cy->normal)); // 축 방향 성분

    // 높이 조건 확인
    if (vdot(cp_, cp_) > (cy->height * cy->height) / 4 || root < rc->tmin || root > rc->tmax)
    {
        // root가 높이 조건을 벗어나거나 t 범위에 없으면 두 번째 root 검사
        root = (-1 * b + sqrt(pbs)) / (2 * a);
        cp = vminus(ray_at(r, root), *(cy->center));
        cp_ = vmult(cy->normal, vdot(cp, cy->normal));

        if (vdot(cp_, cp_) > (cy->height * cy->height) / 4 || root < rc->tmin || root > rc->tmax)
            return (0); // 두 번째 root도 유효하지 않음
    }

    // 겉면인지 확인: 교차점의 노멀과 레이 방향 벡터의 내적이 음수인지 확인
    vector surface_normal = vunit(vminus(cp, cp_)); // 원기둥 표면 노멀
    if (vdot(surface_normal, *(r->end)) > 0)
        return (0); // 안쪽 면 교차는 무시

    // rc 업데이트
    rc->t = root;
    rc->normal = surface_normal;
    rc->m = cy->m;
    rc->tmax = root;
    return (1);
}


int tracer::rayPlaneIntersect(ray *r, plane *p, rec *rc )
{
	vector oc;
	float root;
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
int tracer::raySphereIntersect(ray *r, sphere *s, rec *rc )
{
	point p;	   /* start of transformed ray */
	float a, b, c; /* coefficients of quadratic equation */
	float D;	   /* discriminant */
	point *v;
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

	if (D < 0)
	{ /* no intersection */
		return (FALSE);
	}
	else
	{
		D = static_cast<float>(sqrt(D));
		/* First check the root with the lower value of t: */
		/* this one, since D is positive */
		root = (-b - D) / (2 * a);
		/* ignore roots which are less than zero (behind viewpoint) */
		if (root < 0)
		{
			root = (-b + D) / (2 * a);
		}
		if (root < 0 || root < rc->tmin || rc->tmax < root)
		{
			return (FALSE);
		}
		else
		{
			rc->t = root;
			rc->p = ray_at(r, root);
			rc->normal = vunit(vminus(rc->p, *(s->c)));
			rc->m = s->m;
			rc->tmax = root;
			 
			return (1);
		}
	}
}

int tracer::realHit(ray *r, sphere *s1, sphere *s2, cylinder *cy1, cylinder *cy2, plane *pl, rec *rc)
{
	int h1 = FALSE;
	int h2 = FALSE;
	int h3 = FALSE;
	int h3_ = FALSE;
	int h4 = FALSE;
	int h5 = FALSE;
	int h5_ = FALSE;

	if (s1 != NULL)
		h1 = raySphereIntersect(r, s1, rc);
	if (s2 != NULL)
		h2 = raySphereIntersect(r, s2, rc);
	if (cy1 != NULL)
		h3 = rayCylinderIntersect(r, cy1, rc);
		h3_ = rayCylinderUDIntersect(r, cy1, rc);
	if (cy2 != NULL)
		h5 = rayCylinderIntersect(r, cy2, rc);
		h5_ = rayCylinderUDIntersect(r, cy2, rc);
	if (pl != NULL)
		h4 = rayPlaneIntersect(r, pl, rc);

	if (h1 && !h2 && !h3 && !h3_ && !h5 && !h5_ && !h4)
		return (1);
	else if (h2 && !h3 && !h3_ && !h5 && !h5_ && !h4)
		return (2);
	else if ((h3 && !h3_ && !h5 && !h5_ && !h4) || (h3_ && !h5 && !h5_ && !h4))
		return (3);
	else if((h5 && !h5_ && !h4) || (h5_ && !h4))
		return (5);
	else if (h4)
		return (4);
	else 
		return (0);
}

void tracer::findSphereNormal(sphere *s, point *p, vector *n)
{
	n->x = (p->x - s->c->x) / s->r;
	n->y = (p->y - s->c->y) / s->r;
	n->z = (p->z - s->c->z) / s->r;
	n->w = 0.0;
}

void tracer::findPlaneNormal(plane *p, vector *n)
{
	n->x = p->normal.x;
	n->y = p->normal.y;
	n->z = p->normal.z;
	n->w = 0.0;
	n = &(vunit(*n));
}

void tracer::findCylinderNormal(rec *r, vector *n)
{
	n->x = r->normal.x;
	n->y = r->normal.y;
	n->z = r->normal.z;
	n->w = 0.0;
	n = &(vunit(*n));
}

/* trace */
/* If something is hit, returns the finite intersection point p,
   the normal vector n to the surface at that point, and the surface
   material m. If no hit, returns an infinite point (p->w = 0.0) */
int tracer::trace(ray *r, point *p, vector *n, material **m, float tmax)
{
	rec *record;
	int hit = FALSE;

	record = new rec();

	record->tmin = 0;
	record->tmax = tmax;

	hit = realHit(r, s1, s2, cy1, cy2, pl, record);
	if (hit == 1)
	{
		*m = record->m;
		findPointOnRay(r, record->t, p);
		findSphereNormal(s1, p, n);
	}
	else if (hit == 2)
	{
		*m = record->m;
		findPointOnRay(r, record->t, p);
		findSphereNormal(s2, p, n);
	}
	else if (hit == 3)
	{
		*m = record->m;
		findPointOnRay(r, record->t, p);
		findCylinderNormal(record, n);
	}
	else if (hit == 5)
	{
		*m = record->m;
		findPointOnRay(r, record->t, p);
		findCylinderNormal(record, n);
	}
	else if (hit == 4)
	{
		*m = record->m;
		findPointOnRay(r, record->t, p);
		findPlaneNormal(pl, n);
	}
	else
	{
		/* indicates nothing was hit */
		p->w = 0.0;
		return (0);
	}
	return (1);
}