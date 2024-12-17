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
	if (s1 != NULL)
		delete s1;
	if (viewpoint != NULL)
		delete viewpoint;
}

GLubyte *raytrace::display(void)
{
	drawScene(); /* draws the picture in the canvas */

	return lowlevel.flushCanvas(); /* draw the canvas to the OpenGL window */
}

void raytrace::initScene()
{
	// 빨간색 구 (오)
	s1 = makeSphere(-0.3, -0.1, -3.0, 0.3); // 왼오(-(오) + (왼)) 위아래(-(위) +아래) 앞뒤 ( -(뒤) +(앞))
	s1->m = shader.makeMaterial(1.0, 0.1, 0.0, 0.1, 0.7, 0.5, 50, 0, 1.0, 0);
	// 초록색 구 (왼 - 반사사)
	s2 = makeSphere(0.4, -0.3, -3.2, 0.2);
	s2->m = shader.makeMaterial(0.1, 0.75, 0.1, 0.1, 0.7, 0.5, 50, 0.5, 1.0, 0);
	// 노란색 구 (오른쪽 앞 - 굴절)
	s3 = makeSphere(-0.1, 0.02, -2.5, 0.12);
	s3->m = shader.makeMaterial(0.9, 0.8, 0.0, 0.1, 0.7, 0.5, 50, 0, 1.33, 0.9);
	// 하늘색 평면 (아래)
	pl = makePlane(0.0, 0.2, -2.0, *(makePoint(0, -1, 0, 0)));
	pl->m = shader.makeMaterial(0.0, 0.6, 0.9, 0.1, 0.7, 0.5, 50, 0, 1.0, 0);
	// 주황색 실린더
	cy1 = makeCylinder(0.3, 0.0, -2.0, 0.08, *(makePoint(-1, 0.5, 0.5, 0)), 0.25);
	cy1->m = shader.makeMaterial(0.7, 0.5, 0.0, 0.1, 0.7, 0.5, 50, 0, 1.0, 0);
	// 분홍색 실린더 
	cy2 = makeCylinder(-0.4, 0.0, -2.5, 0.1, *(makePoint(-1, -1, 0, 0)), 0.2);
	cy2->m = shader.makeMaterial(1.0, 0.64, 0.70, 0.1, 0.7, 0.5, 50, 0, 1.0, 0);

	// 일부 객체를 지우고 싶을 때 널값으로 설정정
	// pl = NULL;
	// s1 = NULL;
	// s2 = NULL;
	// s3 = NULL;
	// cy1 = NULL;
	// cy2 = NULL;

	tracer.s1 = s1;
	tracer.s2 = s2;
	tracer.s3 = s3;
	tracer.pl = pl;
	tracer.cy1 = cy1;
	tracer.cy2 = cy2;
	shader.s1 = s1;
	shader.s2 = s2;
	shader.s3 = s3;
	shader.pl = pl;
	shader.cy1 = cy1;
	shader.cy2 = cy2;
	shader.tracer.s1 = s1;
	shader.tracer.s3 = s3;
	shader.tracer.s2 = s2;
	shader.tracer.cy1 = cy1;
	shader.tracer.cy2 = cy2;
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
	point worldPix; /* current pixel in world coordinates */
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
	for (i = 0; i < width; i++)
	{
		for (j = 0; j < height; j++)
		{

			/* find position of pixel in world coordinates */
			/* y position = (pixel height/middle) scaled to world coords */
			worldPix.y = (j - (height / 2)) * imageWidth / width;
			/* x position = (pixel width/middle) scaled to world coords */
			worldPix.x = (i - (width / 2)) * imageWidth / width;

			/* find direction */
			/* note: direction vector is NOT NORMALIZED */
			calculateDirection(viewpoint, &worldPix, &direction);

			/* Find color for pixel */
			rayColor(&r, &c, 50);
			/* write the pixel! */
			lowlevel.drawPixel(i, j, c.r, c.g, c.b);
		}
	}
}

point *raytrace::makePoint(GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	point *p;
	/* allocate memory */
	p = new point();
	/* put stuff in it */
	p->x = x;
	p->y = y;
	p->z = z;
	p->w = w;
	return (p);
}

sphere *raytrace::makeSphere(GLfloat x, GLfloat y, GLfloat z, GLfloat r)
{
	sphere *s;
	/* allocate memory */
	s = new sphere();

	/* put stuff in it */
	s->c = makePoint(x, y, z, 1.0); /* center */
	s->r = r;						/* radius */
	s->m = NULL;					/* material */
	return (s);
}

cylinder *raytrace::makeCylinder(GLfloat x, GLfloat y, GLfloat z, GLfloat r, vector v, GLfloat h)
{
	cylinder *cy;
	/* allocate memory */
	cy = new cylinder();

	/* put stuff in it */
	cy->center = makePoint(x, y, z, 1.0); /* center */
	cy->radius = r;						  /* radius */
	cy->height = h;						  /* height */
	cy->normal = v;						  /* normal */
	cy->m = NULL;						  /* material */
	return (cy);
}

plane *raytrace::makePlane(GLfloat x, GLfloat y, GLfloat z, vector v)
{
	plane *p;
	/* allocate memory */
	p = new plane();

	/* put stuff in it */
	p->center = makePoint(x, y, z, 1.0); /* center */
	p->normal = v;						 /* normal */
	p->m = NULL;						 /* material */
	return (p);
}

light *raytrace::makeLight(point *light_origin, color light_color, GLfloat bright_ratio)
{
	light *l;

	/*allocate memory*/
	l = new light();

	/*put initial value*/
	l->b_r = bright_ratio;
	l->orig = light_origin;
	l->l_c = light_color;
	return (l);
}

vector refract(vector I, vector N, GLfloat eta, GLfloat eta_prime)
{
	GLfloat ratio = eta / eta_prime; // 굴절률 비율
	GLfloat cos_theta_i = -vdot(I, N);
	GLfloat k = 1.0 - ratio * ratio * (1.0 - cos_theta_i * cos_theta_i);

	if (k < 0.0)
	{
		// 전반사 발생
		return {0.0, 0.0, 0.0};
	}
	else
	{
		vector refract_dir = vplus(vmult(I, ratio), vmult(N, ratio * cos_theta_i - sqrt(k)));
		return vunit(refract_dir); // 단위 벡터로 정규화
	}
}

/* returns the color seen by ray r in parameter c */
void raytrace::rayColor(ray *r, color *c, int max_depth)
{
	point p; /* first intersection point */
	vector n;
	material *m;
	light *l;

	/////light initializatin
	// light* makeLight(raytraceData::point* light_origin, raytraceData::color light_color, GLfloat bright_ratio);
	l = makeLight(makePoint(5, -10, 10, 1), {1, 1, 1}, 1.0);

	p.w = 0.0; /* inialize to "no intersection" */
	tracer.trace(r, &p, &n, &m, INFINITY);

	if (p.w != 0.0)
	{
		shader.shade(&p, &n, m, c, l, viewpoint, max_depth); /* do the lighting calculations */
		color rC;
		rC = {0, 0, 0};

		color refractC = {0, 0, 0}; // 반사광 초기화

		if (max_depth > 0)
		{
			// 굴절 광선 처리
			if (m->transparency > 0)
			{
				GLfloat eta = 1.0;						 // 공기 굴절률
				GLfloat eta_prime = m->refractive_index; // 물체의 굴절률
				GLfloat cos_theta_i = vdot(*(r->end), n);

				vector N_corrected = n;
				if (cos_theta_i > 0.0) // 내부에서 외부로 나가는 경우
				{
					eta = m->refractive_index;
					eta_prime = 1.0;
					N_corrected = vmult(n, -1.0); // 법선 반전
				}
				else
				{
					cos_theta_i = -cos_theta_i; // 음수를 양수로 변경
				}

				// 굴절 방향 계산
				vector refract_dir = refract(*(r->end), N_corrected, eta, eta_prime);

				// 전반사가 아닌 경우에만 처리
				if (!(refract_dir.x == 0 && refract_dir.y == 0 && refract_dir.z == 0))
				{
					ray refractR;
					GLfloat epsilon = 0.001;					  // 자기 교차 방지
					vector offset = vmult(N_corrected, -epsilon); // 굴절 시작점 이동
					point refract_start = vplus(p, offset);

					refractR.start = &refract_start;
					refractR.end = &refract_dir;

					// 재귀 호출
					rayColor(&refractR, &refractC, max_depth - 1);
				}
			}
			if (max_depth > 0 && m->reflectivity > 0.01)
			{
				// 반사 벡터 계산
				vector reflect_dir = vminus(*(r->end), vmult(n, 2.0f * vdot(*(r->end), n)));
				reflect_dir = vunit(reflect_dir); // 단위 벡터로 정규화

				// 반사 광선 시작점 보정
				GLfloat epsilon = 0.0001;
				vector offset = vmult(n, epsilon);
				point new_start = vplus(p, offset);

				// 새로운 광선 설정
				ray newR;
				newR.start = &new_start;
				newR.end = &reflect_dir;

				// 재귀 호출
				rayColor(&newR, &rC, max_depth - 1);
			}
			// 비율 합이 1을 초과하지 않도록 조정
			GLfloat total_ratio = m->transparency + m->reflectivity;

			GLfloat transparency_ratio = m->transparency;
			GLfloat reflectivity_ratio = m->reflectivity;

			if (total_ratio > 1.0)
			{
				transparency_ratio /= total_ratio; // 비율 재조정
				reflectivity_ratio /= total_ratio;
			}

			// 색상 혼합
			c->r = c->r * (1.0 - transparency_ratio - reflectivity_ratio) + refractC.r * transparency_ratio + rC.r * reflectivity_ratio;

			c->g = c->g * (1.0 - transparency_ratio - reflectivity_ratio) + refractC.g * transparency_ratio + rC.g * reflectivity_ratio;

			c->b = c->b * (1.0 - transparency_ratio - reflectivity_ratio) + refractC.b * transparency_ratio + rC.b * reflectivity_ratio;
		}
	}
	else
	{ /* nothing was hit */
		c->r = 1.0;
		c->g = 1.0;
		c->b = 1.0;
	}
}

/* vector from point p to point q is returned in v */
void raytrace::calculateDirection(point *p, point *q, point *v)
{
	float length;

	v->x = q->x - p->x;
	v->y = q->y - p->y;
	v->z = q->z - p->z;
	/* a direction is a point at infinity */
	v->w = 0.0;
	v = &(vunit(*v));
}
