#include <iostream>
#include "random.h"
#include <fstream>
#include "drand48.h"
#include "sphere.h"
#include "hitable_list.h"
#include "camera.h"
#include "material.h"
#include "float.h"

using namespace std;

vec3 color(const ray &r, hitable *world, int depth)
{
  hit_record rec;
  // if(world->hit(r, 0.0, FLT_MAX, rec))
  // reflected rays hit the object not exactly t = 0
  if (world->hit(r, 0.001, FLT_MAX, rec))
  {
    // // return 0.5*vec3(rec.normal.x()+1, rec.normal.y()+1, rec.normal.z()+1);
    // vec3 target = rec.p + rec.normal + random_in_unit_sphere();
    // // for better understand: rec.p is the start point of an array
    // return 0.5*color(ray(rec.p, target-rec.p), world);
    ray scattered;
    vec3 attenuation;
    if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered))
    {
      return attenuation * color(scattered, world, depth + 1);
    }
    else
    {
      return vec3(0, 0, 0);
    }
  }
  else
  {
    // draw the backgroud
    vec3 unit_direction = unit_vector(r.direction());
    float t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
  }
}

hitable *random_scene()
{
  int n = 50000;
  hitable **list = new hitable *[n + 1];
  list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(new constant_texture(vec3(0.5, 0.5, 0.5))));
  int i = 1;

  texture *checker = new checker_texture(new constant_texture(vec3(0.2, 0.3, 0.1)),
    new constant_texture(vec3(0.9, 0.9, 0.9)));
    list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(checker));

  for (int a = -10; a < 10; a++)
  {
    for (int b = -10; b < 10; b++)
    {
      float choose_mat = random_double();
      vec3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());
      if ((center - vec3(4, 0.2, 0)).length() > 0.9)
      {
        if (choose_mat < 0.8) // diffuse
        {
          list[i++] = new moving_sphere(center, center+vec3(0, 0.5*drand48(), 0), 0.0, 1.0, 0.2,
                        new lambertian(new constant_texture(vec3(random_double() * random_double(),
                                            random_double() * random_double(),
                                            random_double() * random_double()))));
        }
        else if (choose_mat < 0.95) // metal
        {
          list[i++] = new sphere(center, 0.2,
                                 new metal(vec3(0.5 * (1 + random_double()),
                                                0.5 * (1 + random_double()),
                                                0.5 * (1 + random_double())),
                                           0.5 * random_double()));
        }
        else // glass
        {
          list[i++] = new sphere(center, 0.2, new dielectric(1.5));
        }
      }
    }
  }

  list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
  list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(new constant_texture(vec3(0.4, 0.2, 0.1))));
  list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));

  return new hitable_list(list, i);
}

int main()
{
  freopen("out.txt", "w", stdout);
  int nx = 300;
  int ny = 200;
  int ns = 100;
  cout << "P3\n"<< nx << " " << ny << "\n255\n";
  hitable *world = random_scene();

  vec3 lookfrom(13, 2, 3);
  vec3 lookat(0, 0, 0);
  float dist_to_focus = 10.0;
  float aperture = 0.1;

  camera cam(lookfrom, lookat, vec3(0, 1, 0), 20, float(nx) / float(ny), aperture, dist_to_focus,
      0.0, 1.0);

  for (int j = ny - 1; j >= 0; j--)
  {
    for (int i = 0; i < nx; i++)
    {
      vec3 col(0, 0, 0);
      for (int s = 0; s < ns; s++)
      {
        float u = float(i + random_double()) / float(nx);
        float v = float(j + random_double()) / float(ny);
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
      int ir = int(255.99 * col[0]);
      int ig = int(255.99 * col[1]);
      int ib = int(255.99 * col[2]);
      cout << ir << " " << ig << " " << ib << "\n";
    }
  }

  fclose(stdout);
  return 0;
}
