#include "shader.h"
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

/* shade */
/* color of point p with normal vector n and material m returned in c */
void shader::shade(point* p, vector* n, material* m, color* c, light* l, point* c_origin) {
    // 결과 조명을 저장할 컬러
    color light_color = {0, 0, 0};

    // 광선 방향 계산 (Light Direction)
    vector l_dir = {
        l->orig->x - p->x,
        l->orig->y - p->y,
        l->orig->z - p->z,
        0
    };

    // 방향 벡터 정규화
    GLfloat l_length = sqrt(l_dir.x * l_dir.x + l_dir.y * l_dir.y + l_dir.z * l_dir.z);
    l_dir.x /= l_length;
    l_dir.y /= l_length;
    l_dir.z /= l_length;

    // **Ambient 계산**
    light_color.r += l->l_c.r * m->amb * l->b_r;
    light_color.g += l->l_c.g * m->amb * l->b_r;
    light_color.b += l->l_c.b * m->amb * l->b_r;

    // **Diffuse 계산**
    GLfloat dot_ln = max(0.0f, l_dir.x * n->x + l_dir.y * n->y + l_dir.z * n->z);
    light_color.r += l->l_c.r * m->c.r * m->diff * dot_ln * l->b_r;
    light_color.g += l->l_c.g * m->c.g * m->diff * dot_ln * l->b_r;
    light_color.b += l->l_c.b * m->c.b * m->diff * dot_ln * l->b_r;

    // **Specular 계산**
    vector view_dir = {
        c_origin->x - p->x,
        c_origin->y - p->y,
        c_origin->z - p->z,
        0
    };

    // 뷰 벡터 정규화
    GLfloat view_length = sqrt(view_dir.x * view_dir.x + view_dir.y * view_dir.y + view_dir.z * view_dir.z);
    view_dir.x /= view_length;
    view_dir.y /= view_length;
    view_dir.z /= view_length;

    // 반사 벡터 계산
    vector reflect_dir = {
        2.0f * dot_ln * n->x - l_dir.x,
        2.0f * dot_ln * n->y - l_dir.y,
        2.0f * dot_ln * n->z - l_dir.z,
        0
    };

    // 정규화
    GLfloat reflect_length = sqrt(reflect_dir.x * reflect_dir.x + reflect_dir.y * reflect_dir.y + reflect_dir.z * reflect_dir.z);
    reflect_dir.x /= reflect_length;
    reflect_dir.y /= reflect_length;
    reflect_dir.z /= reflect_length;

    GLfloat dot_vr = max(0.0f, view_dir.x * reflect_dir.x + view_dir.y * reflect_dir.y + view_dir.z * reflect_dir.z);
    GLfloat spec_intensity = pow(dot_vr, m->shininess);

    light_color.r += l->l_c.r * m->c.r * m->spec * spec_intensity * l->b_r;
    light_color.g += l->l_c.g * m->c.g * m->spec * spec_intensity * l->b_r;
    light_color.b += l->l_c.b * m->c.b * m->spec * spec_intensity * l->b_r;

    // 최종 조명 값 반환
    c->r = min(light_color.r, 1.0f); // 색상 값 클램핑
    c->g = min(light_color.g, 1.0f);
    c->b = min(light_color.b, 1.0f);
}
