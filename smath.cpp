#include "smath.h"

#include <math.h>






const vec3 vec3::AXIS_X = vec3(1,0,0);
const vec3 vec3::AXIS_Y = vec3(0,1,0);
const vec3 vec3::AXIS_Z = vec3(0,0,1);
const vec3 vec3::AXIS_NEG_X = vec3(-1,0,0);
const vec3 vec3::AXIS_NEG_Y = vec3(0,-1,0);
const vec3 vec3::AXIS_NEG_Z = vec3(0,0,-1);

vec3&	vec3::mul(vec3 &res, const vec3 &val, float scale) {
	res.x = val.x * scale;
	res.y = val.y * scale;
	res.z = val.z * scale;
	return res;
}

vec3&	vec3::add(vec3 &res, const vec3 &lhs, const vec3 &rhs) {
	res.x = lhs.x+rhs.x;
	res.y = lhs.y+rhs.y;
	res.z = lhs.z+rhs.z;
	return res;
}

vec3&	vec3::sub(vec3 &res, const vec3 &lhs, const vec3 &rhs) {
	res.x = lhs.x-rhs.x;
	res.y = lhs.y-rhs.y;
	res.z = lhs.z-rhs.z;
	return res;
}

float	vec3::dot (const vec3 &lhs, const vec3 &rhs) {
    return lhs.x*rhs.x + lhs.y*rhs.y + lhs.z*rhs.z;
}

float	vec3::nrm(vec3 &res, const vec3 &val)
{
	float mag = sqrt(vec3::dot(val, val));
	vec3::mul(res, val, 1.f/mag);
	return mag;
}

vec3&	vec3::cross(vec3 &res, const vec3 &lhs, const vec3 &rhs) {
	res.x = lhs.y*rhs.z - lhs.z*rhs.y;
	res.y = lhs.z*rhs.x - lhs.x*rhs.z;
	res.z = lhs.x*rhs.y - lhs.y*rhs.x;
	return res;
}

vec3&	vec3::clamp(vec3 &res, const vec3 &val)
{
	res.x = (val.x > 1.0f) ? 1.0f : ((val.x < 0.0f) ? 0.0f : val.x);
	res.y = (val.y > 1.0f) ? 1.0f : ((val.y < 0.0f) ? 0.0f : val.y);
	res.z = (val.z > 1.0f) ? 1.0f : ((val.z < 0.0f) ? 0.0f : val.z);
	return res;
}

vec3& vec3::ori(vec3 &res, const vec3 &old, const vec3 &dir, const vec3 &up) {
	mat4 mtx;
	if ( fabs(vec3::dot(up, dir)-1) <0.0001) 
	{
		res = old;
		res.y = 0;
		vec3::nrm(res,res);
		return res;
	}
	mat4::lookat(mtx,vec3(0,0,0), dir, up);
	vec3::transform(res, old, mtx);
	return res;
}

vec3& vec3::transform(vec3 &res, const vec3 &val, const mat4 &mat)
{
	vec3 old = val;
	res.x = old.x*mat.m[0][0] + old.y*mat.m[0][1]+old.z*mat.m[0][2];
	res.y = old.x*mat.m[1][0] + old.y*mat.m[1][1]+old.z*mat.m[1][2];
	res.z = old.x*mat.m[2][0] + old.y*mat.m[2][1]+old.z*mat.m[2][2];
	return res;
}

bool vec3::tol(const vec3 &v1, const vec3 &v2, float tol = 0.0001f) {
	vec3 temp;
	vec3::sub(temp, v1,v2);
	return vec3::dot(temp, temp) < tol*tol;
}

vec3&	vec3::lerp(vec3 &res, const vec3 &v1, const vec3 &v2, float a )
{
	vec3 t1, t2;
	vec3::mul(t1, v1, 1-a);
	vec3::mul(t2, v2,   a);
	vec3::add(res,t1,t2);
	return res;
}

void mat4::lookat(mat4 &res, const vec3& eye, const vec3& at, const vec3& up)
{
	vec3 zaxis, xaxis, yaxis, temp;

	vec3::sub(zaxis, at, eye);
	vec3::nrm(zaxis, zaxis);

	vec3::cross(xaxis, up, zaxis);
	vec3::nrm(xaxis,xaxis);

	vec3::cross(yaxis, zaxis, xaxis);
	
	res.m[0][0] = xaxis.x; res.m[0][1] = yaxis.x; res.m[0][2] = zaxis.x;
	res.m[1][0] = xaxis.y; res.m[1][1] = yaxis.y; res.m[1][2] = zaxis.y;
	res.m[2][0] = xaxis.z; res.m[2][1] = yaxis.z; res.m[2][2] = zaxis.z;
	
	res.m[3][0] = -vec3::dot( eye, xaxis );
	res.m[3][1] = -vec3::dot( eye, yaxis );
	res.m[3][2] = -vec3::dot( eye, zaxis );
}



