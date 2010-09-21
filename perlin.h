#ifndef ___TEXGEN_H___
#define ___TEXGEN_H___

#include <stdlib.h>


typedef float pixel;
typedef pixel* layer;

struct rgba
{
  unsigned char c[4];
};


union param
{
  float f;
  int i;
};

namespace op
{
  pixel set(pixel& dst, float C);
  pixel add(pixel& dst, const pixel pix1, const pixel pix2);
  pixel inv(pixel& dst, const pixel src);
  pixel sub(pixel& dst, const pixel pix1, const pixel pix2);
  pixel scl(pixel& dst, const pixel src, float S);
  pixel mul(pixel& dst, const pixel pix1, const pixel pix2);
  pixel nrm(pixel dst, const pixel src);
  pixel clmp(pixel& dst, const pixel src, float C);
  pixel msk(pixel& dst, const pixel src, float V);
  pixel msk_gr(pixel& dst, const pixel pix1, const pixel pix2);
  pixel msk_lw(pixel& dst, const pixel pix1, const pixel pix2);
  pixel add_sat(pixel& dst, const pixel pix1, const pixel pix2, float C);
  pixel add_scl(pixel& dst, const pixel pix1, const pixel pix2, float S);
  pixel add_nrm(pixel& dst, const pixel pix1, const pixel pix2);
  pixel mul_scl(pixel& dst, const pixel pix1, const pixel pix2, float S);
  pixel mul_nrm(pixel& dst, const pixel pix1, const pixel pix2);
  pixel add_v(pixel& dst, pixel* vec, int n);
  pixel mul_v(pixel& dst, pixel* vec, int n);
  pixel add_sat_v(pixel& dst, pixel* vec, int n);
  pixel add_avg_v(pixel& dst, pixel* vec, int n);
  pixel add_scl_v(pixel& dst, pixel* vec, float S);
  pixel add_nrm_v(pixel& dst, pixel* vec);
  pixel mul_scl_v(pixel& dst, pixel* vec, float S);
  pixel mul_nrm_v(pixel& dst, pixel* vec);

  rgba pck(pixel r, pixel g, pixel b, pixel a);
  typedef void (*gen_func)(layer* output, int n_o_layers, layer* input, int n_i_layers, param* pa, int n_params, int layer_size);
  void gen_copy(layer dst, layer src);
  layer gen_create_layer(int size);
  void gen_dispose_layer(layer toDispose);

  void gen_constant(layer* output, int n_o_layers, layer* input, int n_i_layers, param* pa, int n_params, int layer_size);
  void gen_distort(layer* output, int n_o_layers, layer* input, int n_i_layers, param* pa, int n_params, int layer_size);
  void gen_perlin(layer* output, int n_o_layers, layer* input, int n_i_layers, param* pa, int n_params, int layer_size);
  void gen_flat(layer* output, int n_o_layers, layer* input, int n_i_layers, param* pa, int n_params, int layer_size);
  void gen_glow(layer* output, int n_o_layers, layer* input, int n_i_layers, param* pa, int n_params, int layer_size);
  void gen_pack(layer* output, int n_o_layers, layer* input, int n_i_layers, param* pa, int n_params, int layer_size);
  void gen_add(layer* output, int n_o_layers, layer* input, int n_i_layers, param* pa, int n_params, int layer_size);
  void gen_mul(layer* output, int n_o_layers, layer* input, int n_i_layers, param* pa, int n_params, int layer_size);
  void gen_clamp(layer* output, int n_o_layers, layer* input, int n_i_layers, param* pa, int n_params, int layer_size);
  void gen_normalize(layer* output, int n_o_layers, layer* input, int n_i_layers, param* pa, int n_params, int layer_size);
  void gen_apply_HSV_const(layer* output, int n_o_layers, layer* input, int n_i_layers, param* pa, int n_params, int layer_size);
  void gen_rgb(layer* output, int n_o_layers, layer* input, int n_i_layers, param* pa, int n_params, int layer_size);
  void gen_pixels(layer* output, int n_o_layers, layer* input, int n_i_layers, param* pa, int n_params, int layer_size);
  void gen_blur(layer* output, int n_o_layers, layer* input, int n_i_layers, param* pa, int n_params, int s);
  void gen_light(layer* output, int n_o_layers, layer* input, int n_i_layers, param* pa, int n_params, int layer_size);
  void gen_transform(layer* output, int n_o_layers, layer* input, int n_i_layers, param* pa, int n_params, int layer_size);
  void gen_mix(layer* output, int n_o_layers, layer* input, int n_i_layers, param* pa, int n_params, int layer_size);
  void gen_mask(layer* output, int n_o_layers, layer* input, int n_i_layers, param* pa, int n_params, int layer_size);
  void gen_alpha(layer* output, int n_o_layers, layer* input, int n_i_layers, param* pa, int n_params, int layer_size);  
  void pack_layers(rgba* buf, int size, layer r, layer g, layer b);
  void gen_brick(layer* output, int n_o_layers, layer* input, int n_i_layers, param* pa, int n_params, int layer_size);
}

#endif
