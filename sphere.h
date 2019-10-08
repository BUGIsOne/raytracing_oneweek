// eliminated a bunch of redundant 2's
// i have no idea, what?
#ifndef SPHEREH
#define SPHEREH

#include "hitable.h"

class sphere: public hitable {
  public:
    sphere() {}
    sphere(vec3 cen, float r, material *m): center(cen), radius(r), mat_ptr(m) {};
    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const;

    material *mat_ptr;
    vec3 center;
    float radius;
};

bool sphere::hit(const ray& r, float t_min, float t_max, hit_record& rec) const{
  vec3 oc = r.origin() - center;
  float a = dot(r.direction(), r.direction());
  // no more 2*dot(oc, r.direction())
  float b = dot(oc, r.direction());
  float c = dot(oc, oc) - radius*radius;
  // no more 4*a*c, cause reduction fraction
  float discriminant = b*b - a*c;

  if(discriminant > 0)
  {
    // no more 2*a
    float temp = (-b - sqrt(b*b - a*c)) / a;
    if(temp < t_max && temp >t_min)
    {
      rec.t = temp;
      rec.p = r.point_at_parameter(rec.t);
      rec.normal = (rec.p - center) / radius;
      rec.mat_ptr = mat_ptr;
      return true;
    }
    temp = (-b + sqrt(b*b - a*c)) / a;
    if(temp < t_max && temp > t_min)
    {
      rec.t = temp;
      rec.p = r.point_at_parameter(rec.t);
      rec.normal = (rec.p - center) / radius;
      rec.mat_ptr = mat_ptr;
      return true;
    }
  }
  return false;
}

#endif
