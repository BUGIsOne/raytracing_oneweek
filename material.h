#ifndef MATERIALH
#define MATERIALH
#include "hitable.h"
#include "drand48.h"
#include "texture.h"

struct hit_record;
// pick a random point "in" the cube. x,y,z between -1 to +1
vec3 random_in_unit_sphere() {
  vec3 p;
  do{
    p = 2.0 * vec3(drand48(), drand48(), drand48()) - vec3(1,1,1);
  }while(p.squared_length() >= 1.0);
  return p;
}

vec3 reflect(const vec3& v, const vec3& n){
  return v - 2*dot(v,n)*n;
}

bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted){
  vec3 uv = unit_vector(v);
  float dt = dot(uv, n);
  float discriminant = 1.0 - ni_over_nt*ni_over_nt*(1-dt*dt);
  if(discriminant > 0)
  {
    refracted = ni_over_nt*(uv - n*dt) - n*sqrt(discriminant);
    return true;
  }
  else
    return false;
}

// for glass, a simple polynomial approximation by Schlick
// varies with angles
float schlick(float cosine, float ref_idx){
  float r0 = (1-ref_idx) / (1+ref_idx);
  r0 = r0*r0;
  return r0 + (1-r0)*pow((1 - cosine), 5);
}

class material{
  public:
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const=0;
    virtual vec3 emitted(float u, float v, const vec3& p) const {
      return vec3(0, 0, 0);
    }
};


class lambertian: public material {
  public:
    lambertian(texture *a): albedo(a) {}
    virtual bool scatter(const ray& r_in, const hit_record& rec, 
        vec3& attenuation, ray& scattered) const{
      vec3 target = rec.p + rec.normal + random_in_unit_sphere();
      // have scattered rays at the time of incident ray
      scattered = ray(rec.p, target-rec.p);
      attenuation = albedo->value(rec.u, rec.v, rec.p);
      return true;
    }

    texture *albedo;
};

class metal: public material {
  public:
    // metal(const vec3& a): albedo(a) {}
    metal(const vec3& a, float f): albedo(a) {
      if(f < 1)
        fuzz = f;
      else
        fuzz = 1;
    }
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuaion, ray& scattered) const{
      vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
      // the bigger the sphere, the fuzzier the reflections will be
      scattered = ray(rec.p, reflected+fuzz*random_in_unit_sphere());
      attenuaion = albedo;
      return (dot(scattered.direction(), rec.normal) > 0);
    }

    vec3 albedo;
    float fuzz;
};

class dielectric: public material {
  public:
    dielectric(float ri): ref_idx(ri) {}
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const{
      vec3 outward_normal;
      // still use to scattered
      vec3 reflected = reflect(r_in.direction(), rec.normal);
      float ni_over_nt;
      // kill the blue channel (?whaaat)
      attenuation = vec3(1.0, 1.0, 1.0);
      vec3 refracted;
      float reflect_prob;
      float cosine;

      if(dot(r_in.direction(), rec.normal) > 0)
      {
        outward_normal = -rec.normal;
        ni_over_nt = ref_idx;
        cosine = ref_idx * dot(r_in.direction(), rec.normal) / r_in.direction().length();
        // cosine = dot(r_in.direction(), rec.normal) / r_in.direction().length();
        // cosine = sqrt(1 - ref_idx*ref_idx*(1-cosine*cosine));
      }
      else
      {
        outward_normal = rec.normal;
        ni_over_nt = 1.0 / ref_idx;
        cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().length();
      }
      
      if(refract(r_in.direction(), outward_normal, ni_over_nt, refracted))
        reflect_prob = schlick(cosine, ref_idx);
      else
      {
        // // a performance bug
        // scattered = ray(rec.p, reflected);
        reflect_prob = 1.0;
      }

      // there is the bug, I typed it refracted
      if(drand48() < reflect_prob)
      {
        scattered = ray(rec.p, reflected);
      }
      else
        scattered = ray(rec.p, refracted);
      
      return true;
    }

    // refraction index
    float ref_idx;
};

class diffuse_light: public material {
public:
  diffuse_light(texture *a) : emit(a) {}
  virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
    return false;
  }
  virtual vec3 emitted(float u, float v, const vec3& p) const {
    return emit->value(u, v, p);
  }

  texture *emit;
};


// isotropic picks a uniform random direction
class isotropic: public material {
public:
  isotropic(texture *a): albedo(a) {}
  virtual bool scatter(const ray& r_in, const hit_record& rec,
      vec3& attenuation, ray& scattered) const {
        scattered = ray(rec.p, random_in_unit_sphere());
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
      }

  texture *albedo;
};


#endif


