#ifndef SMALL_MATH_H
#define SMALL_MATH_H


#include <string.h>


struct mat4;
struct vec3
{
	float x,y,z;

	vec3()							{ this->x = 0; this->y = 0; this->z = 0; }
	vec3(float x, float y, float z) { this->x = x; this->y = y; this->z = z; }
	
	const static vec3 AXIS_X;
	const static vec3 AXIS_Y;
	const static vec3 AXIS_Z;
	const static vec3 AXIS_NEG_X;
	const static vec3 AXIS_NEG_Y;
	const static vec3 AXIS_NEG_Z;

	static vec3&		mul(vec3 &res, const vec3 &val, float scale);
	static vec3&		add(vec3 &res, const vec3 &lhs, const vec3 &rhs);
	static vec3&		sub(vec3 &res, const vec3 &lhs, const vec3 &rhs);

	static float	dot(const vec3 &lhs, const vec3 &rhs);
	static float	nrm(vec3 &res, const vec3 &val);
	static vec3&		cross(vec3 &res, const vec3 &lhs, const vec3 &rhs);

	static vec3&		clamp(vec3 &res, const vec3 &val);

	static vec3&		ori(vec3 &res, const vec3 &old, const vec3 &dir, const vec3 &up);
	static vec3&		transform(vec3 &res, const vec3 &val, const mat4 &mat);

	static bool		tol(const vec3 &v1, const vec3 &v2, float tol );
	static vec3&		lerp(vec3 &res, const vec3 &v1, const vec3 &v2, float a );

};

struct mat4
{
	float m[4][4];
	mat4() { memset(&m, 0, sizeof(m)), m[0][0]=1; m[1][1]=1; m[2][2]=1; m[3][3]=1; }

	static void lookat(mat4 &res, const vec3& eye, const vec3& lookat, const vec3 &up);

};


#endif
