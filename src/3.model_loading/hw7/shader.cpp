#include "shader.h"
#include "vec_util.h"

using namespace raytraceData;

shader::shader()
{
}

shader::~shader()
{
}

material* shader::makeMaterial(GLfloat r, GLfloat g, GLfloat b, GLfloat amb) {
	material* m;

	/* allocate memory */
	m = new material();

	/* put stuff in it */
	m->c.r = r;
	m->c.g = g;
	m->c.b = b;
	m->amb = amb;
	return(m);
}

/* LIGHTING CALCULATIONS */

vector	reflect(point v, vector n)
{
	return (vminus(v, vmult(n, vdot(v, n) * 2)));
}

/* shade */
/* color of point p with normal vector n and material m returned in c */
void shader::shade(point* p, vector* n, material* m, color* c, light* l, point* c_origin) {

	color		light_color = {0, 0, 0};
	vector		l_dir;
	color		diff;
	color		spec;
	ray			l_ray;
	vector		v_dir[2];

	l_dir = vminus(*(l->orig), *p);
	l_dir = vunit(l_dir);
	diff = ptoc(vmult(ctop(l->l_c), fmax(vdot(*n, l_dir), 0.0)));
	v_dir[0] = vunit(vmult(vminus(*p, *c_origin), -1));
	v_dir[1] = reflect(vmult(l_dir, -1), *n);
	spec = ptoc(vmult(vmult(ctop(l->l_c), 0.5), pow(fmax(vdot(v_dir[0], v_dir[1]), 0), 64)));

	light_color = ptoc(vplus(vplus(ctop(light_color), vmult(vplus(ctop(diff), ctop(spec)), l->b_r * 3)), {m->amb, m->amb, m->amb, 1}));
	
	color n_c = ptoc(vmult_(ctop(light_color), ctop(m->c)));
	c->r = n_c.r;
	c->g = n_c.g;
	c->b = n_c.b;

	/* clamp color values to 1.0 */
	if (c->r > 1.0) c->r = 1.0;
	if (c->g > 1.0) c->g = 1.0;
	if (c->b > 1.0) c->b = 1.0;
}