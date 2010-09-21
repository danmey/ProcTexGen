#define _USE_MATH_DEFINES
#include "perlin.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <iostream>
using namespace std;

namespace op
{

    pixel set(pixel& dst, float C) { return dst = C; }
    pixel add(pixel& dst, const pixel pix1, const pixel pix2)
    { return (dst = pix1+pix2); }
    pixel inv(pixel& dst, const pixel src)
    { return (dst = 1-src); }
    pixel sub(pixel& dst, const pixel pix1, const pixel pix2)
    { return (dst = pix1-pix2);}
    pixel scl(pixel& dst, const pixel src, float S)
    { return (dst = src*S); }
    pixel mul(pixel& dst, const pixel pix1, const pixel pix2)
    { return (dst = pix1*pix2); }
    pixel clmp(pixel& dst, const pixel src, float C)
    { (dst = src > C ? C : src); return (dst = dst < 0 ? 0 : dst);  }
    pixel msk(pixel& dst, const pixel src, float V)
    { return (dst = src > V ? 0 : src); }
    pixel msk_gr(pixel& dst, const pixel pix1, const pixel pix2)
    { return (dst = pix1>pix2 ? pix2 : pix1); }
    pixel msk_lw(pixel& dst, const pixel pix1, const pixel pix2)
    {return (dst = pix1<pix2 ? pix2 : pix1); }
    pixel add_sat(pixel& dst, const pixel pix1, const pixel pix2, float C)
    {return clmp(dst, add(dst, pix1,pix2), C); }
    pixel add_scl(pixel& dst, const pixel pix1, const pixel pix2, float S)
    {return scl(dst, add(dst, pix1, pix2), S); }
    pixel mul_scl(pixel& dst, const pixel pix1, const pixel pix2, float S)
    {return scl(dst, mul(dst, pix1,pix2), S); }
    
    rgba pck(pixel r, pixel g, pixel b, pixel a)
    {
	rgba res;
	res.c[3] = 1*255.0f; res.c[2] = r*255.0f; res.c[1] = g*255.0f; res.c[0] = b*255.0f;
	return res;
    }
    
    pixel add_v(pixel& dst, pixel* vec, int n)
    {
	dst = *vec++; for(int i=1;i<n;i++) add(dst, dst, *vec++);
	return dst;
    }
  
  pixel mul_v(pixel& dst, pixel* vec, int n)
  {
    dst = *vec++; for(int i=1;i<n;i++) mul(dst, dst, *vec++);
    return dst;
  }
  
  pixel add_sat_v(pixel& dst, pixel* vec, int n)
  {
    dst = *vec++; for(int i=1;i<n;i++) add(dst, dst, *vec++);
    return clmp(dst, dst, 1);
  }

  pixel add_avg_v(pixel& dst, pixel* vec, int n)
  {
    dst = *vec++; for(int i=1;i<n;i++) add(dst, dst, *vec++);
    return scl(dst, dst, 1./(float)n);
  }
  
  pixel add_scl_v(pixel& dst, pixel* vec, int n, float S)
  {
    dst = *vec++; for(int i=1;i<n;i++) add(dst, dst, *vec++);
    return scl(dst, dst, S);
  }

  pixel mul_scl_v(pixel& dst, pixel* vec, int n, float S)
  {
    dst = *vec++; for(int i=1;i<n;i++) mul(dst, dst, *vec++);
    return scl(dst, dst, S);
  }


  template<class Func>
  void apply_on_layer(layer result, int size,  Func func, float C)
  {
    for(int i=0; i<size*size; i++)
      {
	*result = func(*result, C);
	result++;
      }
  }

  template<class Func>
  void apply_on_layer(layer result, layer src, int size,  Func func, float C)
  {
    for(int i=0; i<size*size; i++)
      {
	*result = func(*result, *src++, C);
	result++;
      }
  }
  
  template<class Func>
  void apply_on_layer(layer result, layer src1, layer src2, int size,  Func func, float C)
  {
    for(int i=0; i<size*size; i++)
      {
	*result = func(*result, *src1++, *src2++, C);
	result++;
      }
  }

  template<class Func>
  void apply_on_layer(layer result, layer src, int size, Func func)
  {
    for(int i=0; i<size*size; i++)
      {
	*result = func(*result, *src++);
	result++;
      }
  }
  
  template<class Func>
  void apply_on_layer(layer result, layer src1, layer src2, int size, Func func)
  {
    for(int i=0; i<size*size; i++)
      {
	*result = func(*result, *src1++, *src2++);
	result++;
      }
  }

#define PI 3.1415
  double interp(double y1,double y2, double mu)
  {
        double mu2;
	mu2 = (1-cos(mu*PI))/2;
           return(y1*(1-mu2)+y2*mu2);
	   // return y1+(y2-y1)*mu;
  }

#define PARAM_START(_params, _n_params) int _num_param = n_params; int _cur_param=0; param* _parameters = _params;
#define PARAM_INT(_var, _default) int _var=_default; if (_cur_param <=_num_param) _var = _parameters[_cur_param++].i;
#define PARAM_FLOAT(_var, _default) float _var=_default; if (_cur_param <=_num_param) _var = _parameters[_cur_param++].f;
  
  void layer_norm(layer dst, int size)
  {
    pixel min = 1E8;
    pixel max = -1E8;
    pixel *p;
    p = dst;
    for ( int i=0; i<size*size; i++)
      {
	if ( *p > max )
	  max = *p;
	if ( *p < min )
	  min = *p;
	p++;
      }
    p = dst;
    for ( int i=0; i<size*size; i++)
      {
	*p = (*p - min)/(max-min);
	p++;
      }
  }
  
  inline float perlin_get_value(int x, int y, int period)
  {
    static bool inited = false;
    static float G[512][512][2];
    
    if ( !inited )
      {
	float temp[256][256][2];
	for ( int y=0; y < 256; y++)
	  for ( int x=0; x < 256; x++)
	    {
	      float r = rand() / (float)(RAND_MAX)*2*M_PI;
	      G[y][x][0] = sin(r);
      	      G[y][x][1] = cos(r);
	    }
	inited = true;
      }
    float g[4][2];
    int ix1 = (x/((int)period))*(period);
    int iy1 = (y/((int)period))*(period);
    int ix2 = ix1+(period);
    int iy2 = iy1+(period);
    g[0][0] = (x-ix1)/(float)period;
    g[0][1] = (y-iy1)/(float)period;
    g[1][0] = (x-ix2)/(float)period;
    g[1][1] = (y-iy1)/(float)period;
    g[2][0] = (x-ix2)/(float)period;
    g[2][1] = (y-iy2)/(float)period;
    g[3][0] = (x-ix1)/(float)period;
    g[3][1] = (y-iy2)/(float)period;
    float v1 = g[0][0] * G[iy1][ix1][0]               + g[0][1] * G[iy1][ix1][1];
    float v2 = g[1][0] * G[iy1][ix1+period][0]        + g[1][1] * G[iy1][ix1+period][1];
    float v3 = g[2][0] * G[iy1+period][ix1+period][0] + g[2][1] * G[iy1+ period][ix1+ period][1];
    float v4 = g[3][0] * G[iy1+period][ix1][0]        + g[3][1] * G[iy1+ period][ix1][1];
    float a = (x%period)/(float)period;
    float b = (y%period)/(float)period;
    float v12, v43;
    //	    a = 3*a*a - 2*a*a*a;
    //b = 3*b*b - 2*b*b*b;
    v12 = interp(v1, v2, a);
    v43 = interp(v4, v3, a);
    return interp(v12,v43, b);

  }
  
  void layer_perlin(layer dst, int size, int period, float amp, int mode)
  {
	/*
	for ( int y=0; y < 256; y++)
	  for ( int x=0; x < 256; x++)
	    {
	      float xf = x/256.f;
     	      float yf = y/256.f;
	      	      
	      float v1[2] = { temp[y    ][x    ][0] * (1-xf)*(1-yf),     temp[y    ][x    ][1] * (1-xf)*(1-yf) };
	      float v2[2] = { temp[255-y][x    ][0] * (1-xf)*(  yf),     temp[255-y][x    ][1] * (1-xf)*(  yf) };
      	      float v3[2] = { temp[255-y][255-x][0] * (  xf)*(  yf),     temp[255-y][255-x][1] * (  xf)*(  yf) };
      	      float v4[2] = { temp[y    ][255-x][0] * (  xf)*(1-yf),     temp[y    ][255-x][1] * (  xf)*(1-yf) };
	      G[y][x][0] = v1[0]+v2[0]+v3[0]+v4[0];
      	      G[y][x][1] = v1[1]+v2[1]+v3[1]+v4[1];
	    }
	*/
    pixel *p = dst;
    
    for(int y=0; y<size; y++)
	for(int x=0; x<size; x++)
	  {
	    float xf = x/(float)size;
     	    float yf = y/(float)size;
	    if (mode == 0 )
	      *p++ += amp*perlin_get_value(x,y,period);
	    else {
	    	    *p++ += amp * (
	      perlin_get_value(x,y,            period) * (1-xf)*(1-yf)+
      	      perlin_get_value(size-x,      y, period) * (  xf)*(1-yf)+
      	      perlin_get_value(size-x, size-y, period) * (  xf)*(  yf)+
      	      perlin_get_value(x,      size-y, period) * (1-xf)*(  yf));
	    }
	      
	  }
  }

  inline float to_rad(float val) { return val * 2*M_PI; }
  void gen_transform(layer* output, int n_o_layers, layer* input, int n_i_layers, param* pa, int n_params, int layer_size)
  {
    PARAM_START(pa, n_params);
    PARAM_INT(ox, layer_size/2);
    PARAM_INT(oy, layer_size/2);
    PARAM_INT(tx, 0);
    PARAM_INT(ty, 0);
    PARAM_FLOAT(sx, 1);
    PARAM_FLOAT(sy, 1);
    PARAM_FLOAT(rot, 0);

    float ang = to_rad(rot);
    float ang_90 = to_rad(0.5-rot);
    float dy1 = sin(ang)*sx;
    float dx1 = cos(ang)*sx;    
    float dy2 = cos(ang_90)*sy;
    float dx2 = sin(ang_90)*sy;    

    float start_x = -ox + tx;
    float start_y = -oy + ty;
    for(int y=0; y<layer_size; y++) {
      float cur_x = start_x;
      float cur_y = start_y;
      for(int x=0; x<layer_size; x++) {
	int cut_x = ((int)cur_x + ox); if ( cut_x < 0 ) cut_x += layer_size;
      	int cut_y = ((int)cur_y + oy); if ( cut_y < 0 ) cut_y += layer_size;
	int pix_x = cut_x%layer_size;
	int pix_y = cut_y%layer_size;
	int offs = pix_x+pix_y*layer_size;
	output[0][x+y*layer_size] = input[0][offs];
	cur_x += dx1;
	cur_y += dy1;
      }
      start_x += dx2;
      start_y += dy2;
    }
  }

  //mode
  // 0: pixel > t && pixel < t2
  // 1: pixel < t || pixel > t2
  void gen_mask(layer* output, int n_o_layers, layer* input, int n_i_layers, param* pa, int n_params, int layer_size)
  {
    PARAM_START(pa, n_params);
    PARAM_INT(mode, 0);
    PARAM_FLOAT(val, 1);
    PARAM_FLOAT(t, 0.5);
    PARAM_FLOAT(t2, 1);
    for(int i=0;i<layer_size*layer_size;i++) {
      switch(mode) {
      case 0: if ( input[0][i] >= t && input[0][i] < t2 ) output[0][i] = val; else output[0][i] = 0; break;
      case 1: (output[0][i] = input[0][i] < t || input[0][i] > t2) ? val : 0; break;
      }
    }
  }

  void gen_mix(layer* output, int n_o_layers, layer* input, int n_i_layers, param* pa, int n_params, int layer_size)
  { 
    PARAM_START(pa, n_params);
    PARAM_INT(mode, 0);
    for(int i=0;i<layer_size*layer_size;i++) {
      switch(mode) {
      case 0: output[0][i] = input[0][i] < input[1][i] ? input[0][i] : input[1][i];
      case 1: output[0][i] = input[0][i] >= input[1][i] ? input[0][i] : input[1][i];
      }
    }
  }

  
  void gen_brick(layer* output, int n_o_layers, layer* input, int n_i_layers, param* pa, int n_params, int layer_size)
  {
    PARAM_START(pa, n_params);
    PARAM_INT(w, layer_size/8);
    PARAM_INT(h, w/2);
    PARAM_FLOAT(c, 1);

    for(int y=0; y<layer_size; y++) 
      for(int x=0; x<layer_size; x++) {
	if ( ((y/h)%2 == 0 && x % w == 0) || ((y/h)%2 == 1 && x % w == w/2)
	     || (y%h) == 0
	     )
	  output[0][y*layer_size+x] = c;
	else
  	  output[0][y*layer_size+x] = 0;
      }
  }

  void gen_light(layer* output, int n_o_layers, layer* input, int n_i_layers, param* pa, int n_params, int layer_size)
  {
    PARAM_START(pa, n_params);
    PARAM_FLOAT(lx, 0);
    PARAM_FLOAT(ly, 0);

    float len = sqrt(lx*lx+ly*ly);
    if (len != 0) {
      lx /= len;
      ly /= len;
    }

    pixel* il = input[0];
    pixel* ol = output[0];
    for(int y=0; y<layer_size; y++)   
      for(int x=0; x<layer_size; x++) {
	float pix_up, pix_down, pix_left, pix_right;
	pix_down = il[((y+1)%layer_size)*layer_size + x];
	pix_up = il[((y-1) < 0 ? (layer_size-1) : (y-1))*layer_size + x];
	pix_right = il[((x+1)%layer_size) + y*layer_size];
	pix_left = il[((x-1) < 0 ? (layer_size-1) : (x-1)) + y*layer_size];
	float dx, dy;
	dx = pix_right-pix_left;
	dy = pix_up-pix_down;
	float len = sqrt(dx*dx+dy*dy);
	if (len != 0) {
	  dx /= len;
	  dy /= len;
	}
	float l = 0.5*(dx*lx+dy*ly) + 0.5;
	ol[y*layer_size+x] = l;
    }
  }

  enum { DIST_RADIAL };
  void gen_distort(layer* output, int n_o_layers, layer* input, int n_i_layers, param* pa, int n_params, int layer_size)
  {
    PARAM_START(pa, n_params);
    PARAM_INT(type, DIST_RADIAL);
    float ampl = 100.f;
    pixel* pout = output[0];
    pixel* pdst = input[0];
    pixel* pdir = input[1];
    pixel* ppow = input[2];
    for (int y=0; y<layer_size; y++)
      for (int x=0; x<layer_size; x++)
	{
	  float raddir = *pdir*2*M_PI;
	  int xd = (float)sin(raddir)* *ppow*ampl;
  	  int yd = (float)cos(raddir)* *ppow*ampl;
	  xd+=x;
	  yd+=y;
	  xd%=(layer_size);
	  yd%=(layer_size-1);
	  if ( xd < 0 ) xd = layer_size+xd;
  	  if ( yd < 0 ) yd = layer_size+yd;
	  *pout = pdst[yd*layer_size+xd];
	  pdir++; ppow++; pout++;
	}
  }

  

  layer gen_create_layer(int size)
  {
    return new pixel[size*size];
  }

  void gen_dispose_layer(layer toDispose)
  {
    delete []toDispose;
  }

  void gen_copy(layer dst, layer src, int size)
  {
    memcpy(dst, src, size*size*sizeof(pixel));
  }
  
  void gen_perlin(layer* output, int n_o_layers, layer* input, int n_i_layers, param* pa, int n_params, int layer_size)
  {
    PARAM_START(pa, n_params);
    PARAM_INT(octaves, 6);
    PARAM_FLOAT(persistence, 0.7);
    PARAM_INT(mode, 1);
      //printf("P:%f %d/n", persistence, octaves);
    layer p = output[0];
    //layer buf = new pixel[size*size];
    //op::apply_on_layer(buf, size, set, 0.0f);
    memset(p, 0, layer_size*layer_size*sizeof(float));
    for (int o=0; o<octaves; o++)
      {
	layer_perlin(p, layer_size, 256/pow(2,o), pow(persistence, o), mode);
	//layer_perlin(p, size, 256, 1);
      }
    layer_norm(p, layer_size);
	//delete buf;
  }

  void gen_alpha(layer* output, int n_o_layers, layer* input, int n_i_layers, param* pa, int n_params, int layer_size)
  {
    PARAM_START(pa, n_params);
    PARAM_INT(stride, 3)
      for(int si=0; si<stride; si++) {
     	pixel* ol = output[si];
	pixel* il1 = input[si];
	pixel* il2 = input[si+stride];
	pixel* al = input[n_i_layers-1];
	for(int pi=0; pi<layer_size*layer_size;pi++) {
	  *ol = *il1* *al + *il2*(1- *al);
	  ol++; il1++; il2++; al++;
	}
      }
  }
    
  
  void gen_flat(layer* output, int n_o_layers, layer* input, int n_i_layers, param* pa, int n_params, int layer_size)
  {
    float c=1;
    PARAM_START(pa, n_params);
    PARAM_INT(x, 0);
    PARAM_INT(y, 0);
    PARAM_INT(w, layer_size);
    PARAM_INT(h, layer_size);
    PARAM_FLOAT(color, 1)
      //      cout << x << ", " << y << "(" << w << ", " << h << "): " << color << "\n";
      /*    if ( pa[0].i == 5)
      {
	c=pa++->f;
	}*/

    pixel *p = output[0];
    //printf("%x\n", p);
    p += layer_size*y+x;
    //printf("%d, %d \n", w , h);
    int w2 = w;
    int x2 = x;
    while(h-->0)
      {
	while(w-->0)
	  {
	    //	  printf("%d, %d\n", w , h);
	    op::set(*p++, color);
	  }
	w=w2;
	p+=layer_size-w2;
      }

  }

  void hsv2rgb(float h, float s, float v, float& R, float& G, float& B)
  {
    h = fmod(h, 1.0f); 
    int hi = (int)floor(360.f*h/60.0)%6;
    float f = h*360.f/60.0f - hi;
    float p = v*(1-s);
    float q = v*(1-(float)f*s);
    float t = v*(1-(1-(float)f)*s);
    
    switch(hi)
      {
      case 0 :
	R = v;
	G = t;
	B = p;
	break;
      case 1 :
	R = q;
	G = v;
	B = p;	
	break;
      case 2 :
	R = p;
	G = v;
	B = t;	
	break;
      case 3 :
	R = p;
	G = q;
	B = v;	
	break;
      case 4 :
	R = t;
	G = p;
	B = v;	
	break;
      case 5 :
	R = v;
	G = p;
	B = q;	
	break;
      }
  }

  void gen_glow(layer* output, int n_o_layers, layer* input, int n_i_layers, param* pa, int n_params, int layer_size)
  {
    PARAM_START(pa, n_params);
    PARAM_FLOAT(x0, layer_size/2);
    PARAM_FLOAT(y0, layer_size/2);
    PARAM_FLOAT(atten, 5);
    PARAM_FLOAT(xr, 1)
    PARAM_FLOAT(yr, 1)
    for ( int y=0; y<layer_size; y++)
      for ( int x=0; x<layer_size; x++)
	{
	  float val = 1-sqrt((x-x0)*(x-x0)*xr + (y-y0)*(y-y0)*yr)/layer_size;
	  val = pow(val, atten);
	  output[0][y*layer_size+x] = val;
	}
  }
  
  void gen_apply_light(layer* output, int n_o_layers, layer* input, int n_i_layers, param* pa, int n_params, int layer_size)
  {
    for ( int y=0; y<layer_size; y++)
      for ( int x=0; x<layer_size; x++)
	{
	      
	}
  }
  
  void gen_apply_HSV_const(layer* output, int n_o_layers, layer* input, int n_i_layers, param* pa, int n_params, int layer_size)
  {
    PARAM_START(pa, n_params);
    PARAM_FLOAT(hp, 1.0f);
    PARAM_FLOAT(sp, 1.0f);
    PARAM_FLOAT(vp, 1.0f);
    
    pixel* h = input[0];
    pixel* s = input[1];
    pixel* v = input[2];
    pixel* r = output[0];
    pixel* g = output[1];
    pixel* b = output[2];
    
    for ( int i=0; i<layer_size*layer_size; i++)
      {
	hsv2rgb(h[i]*hp, s[i]*sp, v[i]*vp, r[i], g[i], b[i]);
      }
  }

  void gen_rgb(layer* output, int n_o_layers, layer* input, int n_i_layers, param* pa, int n_params, int layer_size)
  {
    PARAM_START(pa, n_params);
    PARAM_FLOAT(r, 1.0f);
    PARAM_FLOAT(g, 1.0f);
    PARAM_FLOAT(b, 1.0f);
    //    cout << "RGB params: " << r << ", " << g << ", " << b << "\n";

    pixel* ri = input[0];
    pixel* gi = input[1];
    pixel* bi = input[2];
    pixel* ro = output[0];
    pixel* go = output[1];
    pixel* bo = output[2];
    
    for ( int i=0; i<layer_size*layer_size; i++)
      {
        *ro++ = *++ri*r;
        *go++ = *++gi*g;
        *bo++ = *++bi*b;
      }
    
  }
  
  void gen_add(layer* output, int n_o_layers, layer* input, int n_i_layers, 
               param* pa, int n_params, int layer_size) {

    PARAM_START(pa, n_params);
    PARAM_INT(stride, 1);

    for(int li=0; li<n_i_layers/stride;li++) {
      for(int si=0; si<stride; si++) {
     	pixel* ol = output[si];
	pixel* il = input[si+li*stride];
	for(int pi=0; pi<layer_size*layer_size;pi++) {
	  op::add(*ol, *ol, *il);
	  ol++; il++;
	}
      }
    }
  }

  void gen_mul(layer* output, int n_o_layers, layer* input, int n_i_layers, 
               param* pa, int n_params, int layer_size) {
    PARAM_START(pa, n_params);
    PARAM_INT(stride, 1);
    for(int i=0; i<layer_size*layer_size; i++) {
      for(int si=0; si<stride; si++) {
	output[si][i] = 1.0;
      }
    }
    
    for(int li=0; li<n_i_layers/stride;li++) {
      for(int si=0; si<stride; si++) {
     	pixel* ol = output[si];
	pixel* il = input[si+li*stride];
	for(int pi=0; pi<layer_size*layer_size;pi++) {
	  op::mul(*ol, *ol, *il);
	  ol++; il++;
	}
      }
    }
  }

  void gen_clamp(layer* output, int n_o_layers, layer* input, int n_i_layers, 
                 param* pa, int n_params, int layer_size){
    pixel* ol = output[0];      
    pixel* il = input[0];
    for(int pi=0; pi<layer_size*layer_size;pi++) {
      op::clmp(*ol, *il, 1);
      ol++; il++;
    }
  }

  void gen_normalize(layer* output, int n_o_layers, layer* input, int n_i_layers, 
                     param* pa, int n_params, int layer_size){
    pixel* ol = output[0];      
    pixel* il = input[0];
    gen_copy(ol, il, layer_size);
    layer_norm(ol, layer_size);
  }

  void gen_pixels(layer* output, int n_o_layers, layer* input, int n_i_layers, 
                     param* pa, int n_params, int layer_size){
    PARAM_START(pa, n_params);
    PARAM_INT(count, 1000);
    for(int i=0; i<count; i++) output[0][rand()%(layer_size*layer_size)] = 1.0; 
  }

  void blur_line(pixel* ol, pixel* il, int s, int r, bool horizontal)
  {
    pixel total=0;
    
    int line_mul = horizontal ? 0 : s;
    int line_add = horizontal ? 1 : 0;

#define A(i) ((i)*line_mul+(i)*line_add)
#define PIX(b, i) ((b)[(i)*line_mul+(i)*line_add])
#define P(b) *((b+=(line_mul+line_add))-(line_mul+line_add))
    
    //    for (int y=0; y<s; y++) {
      total = 0;
      float av = (float)r*2+1;
      for(int i=0; i<r; i++) { total += PIX(il, i); }
      P(ol) += total/(float)(r+1); 

      for(int x=1; x<s; x++) {
	if (x>=1 && x<=r) {
	  total += PIX(il, x+r-1);
	  P(ol) += total/(float)(x+r+1);
	  } else if ( x>r && x<=(s-r)) {
	  total += PIX(il,x+r-1) - PIX(il, x-r-1);
	  //total += PIX(il,x+r-1);
	  P(ol) += total/(float)(av);
	} else if ( x >(s-r) && x<s) {
	  int delta = (x-(s-r));
	  total += -PIX(il, s-delta);
	  P(ol) += total/(float)(r+(s-x)+1);
	}
      }
      //}
  }
  
  void gen_blur(layer* output, int n_o_layers, layer* input, int n_i_layers, 
                     param* pa, int n_params, int s){
    PARAM_START(pa, n_params);
    PARAM_INT(r, 32);
    PARAM_INT(n, 1);
    PARAM_INT(m, 0)
      
      //    cout << "blala\n";
      //cout << "S: " << s << "R: " << r << "\n";
    if ( s < r ) return;

    pixel* spare = gen_create_layer(s);
    pixel* il = input[0];
    for ( int i=0; i<n; i++) {
      pixel* ol = spare;
      if ( i != 0 )
	il = output[0];

      if (m == 0 || m == 1) {
	for (int y=0; y<s; y++) {
	  blur_line(ol, il, s, r, true);
	  il+=s;
	  ol+=s;
	}
      }
      
      if ( m == 1 ) { gen_copy(output[0], spare, s); continue; }

      if ( m == 2 ) {
	pixel* temp = spare;
	spare = output[0];
	output[0] = temp;
      } else {
	ol = output[0];
	il = spare;
      }

      if (m == 0 || m == 2) {
      for (int y=0; y<s; y++) {
	blur_line(ol, il, s, r, false);
	il+=1;
	ol+=1;
      }
      }
    }

    //layer_norm(output[0], s);
    
  }
  
  void pack_layers(rgba* buf, int size, layer r, layer g, layer b)
  {
    for(int i=0; i<size*size; i++)
      *buf++ = pck(*r++, *g++, *b++, 0);
  }

    void gen_pack(layer* output, int n_o_layers, layer* input, int n_i_layers, param* pa, int n_params, int layer_size)
    {

    }
    
};

