#include "shader.h"
#include "tracer.h"
#include "vec_util.h"

using namespace raytraceData;

shader::shader()
{
}

shader::~shader()
{
}

material* shader::makeMaterial(GLfloat r, GLfloat g, GLfloat b, GLfloat amb, GLfloat diff, GLfloat spec, GLfloat shininess) {
	material* m;

	/* allocate memory */
	m = new material();

	/* put stuff in it */
	m->c.r = r;
	m->c.g = g;
	m->c.b = b;
	m->amb = amb;
	m->diff = diff;
	m->spec = spec;
	m->shininess = shininess;
	return(m);
}

/* LIGHTING CALCULATIONS */

vector	reflect(point v, vector n)
{
	return (vminus(v, vmult(n, vdot(v, n) * 2)));
}

// is Shadow confirm 
int shader::isShadow(light l, point p, int type)
{
    point po;
    vector n;
    material *m;
    ray r;

    vector l_dir = vminus(*(l.orig), p);

    r.start = &(vplus(p, vmult(l_dir, EPSILON)));
    r.end = &l_dir;
    r.end->w = 0.0;

    float light_len = vlength(l_dir) + vlength(vmult(l_dir, EPSILON));

    int res = tracer.trace(&r, &po, &n, &m, light_len);
    if (res && res != type)
        return (1);
    return (0);
}

/* shade */
/* color of point p with normal vector n and material m returned in c */
void shader::shade(point* p, vector* n, material* m, color* c, light* l, point* c_origin, int type) {
    // 결과 조명을 저장할 컬러
    color light_color = {0, 0, 0};
    int isVisible = 1;

    // 그림자 여부 확인
    if (isShadow(*l, *p, type)) {
        isVisible = 0;
    }

    // 광선 방향 계산 (Light Direction)
    vector l_dir = {
        l->orig->x - p->x,
        l->orig->y - p->y,
        l->orig->z - p->z,
        0
    };

    l_dir = vunit(l_dir);
    // ambient 계산
    light_color = ptoc(vmult(ctop(l->l_c), m->amb * l->b_r));

    if (isVisible) {
        // diffuse 계산
        GLfloat dot_ln = max(0.0f, l_dir.x * n->x + l_dir.y * n->y + l_dir.z * n->z);
        light_color = ptoc(vplus(ctop(light_color), vmult(vmult_(ctop(l->l_c), ctop(m->c)), m->diff * dot_ln * l->b_r)));

        // specular 계산
        vector view_dir = {
            c_origin->x - p->x,
            c_origin->y - p->y,
            c_origin->z - p->z,
            0
        };

        view_dir = vunit(view_dir);

        // 반사 벡터 계산
        vector reflect_dir = {
            2.0f * dot_ln * n->x - l_dir.x,
            2.0f * dot_ln * n->y - l_dir.y,
            2.0f * dot_ln * n->z - l_dir.z,
            0
        };

        reflect_dir = vunit(reflect_dir);

        GLfloat dot_vr = max(0.0f, view_dir.x * reflect_dir.x + view_dir.y * reflect_dir.y + view_dir.z * reflect_dir.z);
        GLfloat spec_intensity = pow(dot_vr, m->shininess);
        light_color = ptoc(vplus(ctop(light_color), vmult(vmult_(ctop(l->l_c), ctop(m->c)), m->spec * spec_intensity * l->b_r)));
    }

    // 최종 조명 값 반환 -> 색상 값 클램핑
    c->r = min(light_color.r, 1.0f);
    c->g = min(light_color.g, 1.0f);
    c->b = min(light_color.b, 1.0f);
}
