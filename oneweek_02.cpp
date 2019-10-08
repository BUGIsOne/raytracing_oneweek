#include <iostream>
#include <float.h>
#include <fstream>
#include "drand48.h"
#include "sphere.h"
#include "hitable_list.h"
#include "camera.h"
#include "material.h"

using namespace std;

vec3 color(const ray& r, hitable *world, int depth) {
  hit_record rec;
  // if(world->hit(r, 0.0, FLT_MAX, rec))
  // reflected rays hit the object not exactly t = 0
  if(world->hit(r, 0.001, FLT_MAX, rec))
  {
    // // return 0.5*vec3(rec.normal.x()+1, rec.normal.y()+1, rec.normal.z()+1);
    // vec3 target = rec.p + rec.normal + random_in_unit_sphere();
    // // for better understand: rec.p is the start point of an array
    // return 0.5*color(ray(rec.p, target-rec.p), world);
    ray scattered;
    vec3 attenuation;
    if(depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered) ) {
      return attenuation*color(scattered, world, depth+1);
    }
    else{
      return vec3(0, 0, 0);
    }
  }
  else
  {
    // draw the backgroud
    vec3 unit_direction = unit_vector(r.direction());
    float t = 0.5*(unit_direction.y() + 1.0);
    return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
  }
}

int main(){
  freopen("out.txt","w",stdout);
  int nx = 200;
  int ny = 100;
  int ns = 100;
  cout<<"P3\n" << nx <<" " << ny << "\n255\n";
  hitable *list[5];
  //the circle sphere
  list[0] = new sphere(vec3(0, 0, -1), 0.5, new lambertian(vec3(0.1, 0.2, 0.5)));
  //backgroud
  list[1] = new sphere(vec3(0, -100.5, -1), 100, new lambertian(vec3(0.8, 0.8, 0.0)));
  list[2] = new sphere(vec3(1, 0, -1), 0.5, new metal(vec3(0.8, 0.6, 0.2)));
  list[3] = new sphere(vec3(-1, 0, -1), 0.5, new dielectric(1.5));
  // use a negative radius, the surface normal points inward, like a bubble
  list[4] = new sphere(vec3(-1, 0, -1), -0.45, new dielectric(1.5));
  hitable *world = new hitable_list(list, 5);
  camera cam;
  
  for(int j=ny-1; j>=0; j--)
  {
    for(int i=0; i<nx; i++)
    {
      vec3 col(0, 0, 0);
      for(int s=0; s<ns; s++)
      {
        // float u = float(i) / float(nx);
        // float v = float(j) / float(ny);
        // ray r(origin, lower_left_corner + u*horizontal + v*vertical);

        // random to get ray
        float u = float(i+drand48()) / float(nx);
        float v = float(j+drand48()) / float(ny);
        ray r = cam.get_ray(u, v);
 
        // edge pixels are part backgroud and part foreground
        col += color(r, world, 0);
        // vec3 col = color(r, world);
      }
      col /= float(ns);
      // ''' image's 0 to 1 values have some transform before being
      // stored as a byte, to look light. - - - - >"gamma corrected"
      // raising color to the power 1/2, (square-root) '''
      col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
      int ir = int(255.99*col[0]);
      int ig = int(255.99*col[1]);
      int ib = int(255.99*col[2]);
      cout<<ir<<" " << ig<< " " <<ib<<"\n";
    }
  }

  fclose(stdout);
  return 0;
}

