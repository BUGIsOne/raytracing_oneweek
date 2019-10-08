#include <iostream>
#include "vec3.h"
#include "ray.h"
#include <sstream>
#include <fstream>
#include <io.h>

using namespace std;


// ''' 
// dot((A + t*B - C) ,(A + t*B - C)) = R*R
// t*t*dot(B, B) + 2*t*dot(B,A-C) + dot(A-C,A-C) - R*R = 0
// cause: t*t is double power
// so b*b - 4*a*c >= 0 , there will be answer(s) 
// '''

// draw the hit sphere into red
float hit_sphere(const vec3& center, float radius, const ray& r){
  vec3 oc = r.origin() - center;
  float a = dot(r.direction(), r.direction());
  float b = 2.0 * dot(oc, r.direction());
  float c = dot(oc, oc) - radius*radius;
  float discriminant = b*b - 4*a*c;
  // return (discriminant > 0);

  // a trick to visualize the normals
  if(discriminant < 0)
    return -1.0;
  else
    // the smaller value of the figuration
    return (-b - sqrt(discriminant) ) / (2.0*a);
}

vec3 color(const ray& r) {
  // if(hit_sphere(vec3(0, 0, -1), 0.5, r))
  // // draw the pixel to red
  //   return vec3(1, 0, 0);

  float t = hit_sphere(vec3(0, 0, -1), 0.5, r);
  if(t > 0.0)
  {
    vec3 N = unit_vector(r.point_at_parameter(t) - vec3(0, 0, -1))  ;
    return 0.5*vec3(N.x()+1, N.y()+1, N.z()+1);
  }

  // draw the background
  vec3 unit_direction = unit_vector(r.direction());
  t = 0.5*(unit_direction.y() + 1.0);
  // blended_value = (1-t)*start_value + t*end_value
  return (1.0-t)*vec3(1.0,1.0,1.0) + t*vec3(0.5,0.7,1.0);
}


int main()
{
  freopen("out.txt","w",stdout);
  int nx = 200;
  int ny = 100;
  cout << "P3\n"<<nx << " "<<ny<< "\n255\n";

  vec3 lower_left_corner(-2.0, -1.0, -1.0);
  vec3 horisontal(4.0, 0.0, 0.0);
  vec3 vertical(0.0, 2.0, 0.0);
  vec3 origin(0.0, 0.0, 0.0);

  for(int j=ny-1; j>=0; j--)
  {
    for(int i=0; i<nx; i++)
    {
      //vec3 col(float(i) / float(nx), float(j) / float(ny), 0.2);
      float u = float(i) / float(nx);
      float v = float(j) / float(ny);
      ray r(origin, lower_left_corner + u*horisontal + v*vertical);
      vec3 col = color(r);
      
      int ir = int(255.99*col[0]);
      int ig = int(255.99*col[1]);
      int ib = int(255.99*col[2]);

      cout<< ir << " " << ig << " " << ib << "\n";
    }
  }

  fclose(stdout);

  return 0;
}