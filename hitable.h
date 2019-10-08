#ifndef HITABLEH
#define HITABLEH
#include "ray.h"

class material;

struct hit_record {
  float t;
  vec3 p; // array, from start point to end
  vec3 normal;
  material *mat_ptr; // to point a class by pointer;
};

class hitable {
  public:
    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const=0;
};

#endif