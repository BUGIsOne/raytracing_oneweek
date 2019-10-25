#include <iostream>
#include "random.h"
#include <fstream>
#include "drand48.h"
#include "sphere.h"
#include "hitable_list.h"
#include "camera.h"
#include "material.h"
#include "float.h"
#include "aarect.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

vec3 color(const ray &r, hitable *world, int depth)
{
  hit_record rec;

  if (world->hit(r, 0.001, FLT_MAX, rec))
  {
    ray scattered;
    vec3 attenuation;
    vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
    if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered))
    {
      // return attenuation * color(scattered, world, depth + 1);
      return emitted + attenuation * color(scattered, world, depth+1);
    }
    else
    {
      // return vec3(0, 0, 0);
      return emitted;
    }
  }
  else
  {
    // // draw the backgroud
    // vec3 unit_direction = unit_vector(r.direction());
    // float t = 0.5 * (unit_direction.y() + 1.0);
    // return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
    return vec3(0, 0, 0);
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

hitable *two_spheres(){
  texture *checker = new checker_texture(new constant_texture(vec3(0.2, 0.3, 0.1)),
        new constant_texture(vec3(0.9, 0.9, 0.9)));
        int n = 50;
        hitable **list = new hitable*[n+1];
        list[0] = new sphere(vec3(0, -10, 0), 10, new lambertian(checker));
        list[1] = new sphere(vec3(0, 10, 0), 10, new lambertian(checker));
        
        return new hitable_list(list, 2);
}

hitable *two_perlin_spheres(){
  texture *pertext = new noise_texture(3);
  hitable **list = new hitable*[2];
  list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(pertext));
  list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(pertext));
  return new hitable_list(list, 2);
}

hitable *earth() {
  // use texture
  int nx, ny, nn;
  unsigned char *tex_data = stbi_load("earth.jpg", &nx, &ny, &nn, 0);
  material *mat = new lambertian(new image_texture(tex_data, nx, ny));

  return new sphere(vec3(0, 0, 0), 60, mat);
}

hitable *simple_light() {
  texture *pertext = new noise_texture(4);
  hitable **list = new hitable*[4];
  list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(pertext));
  list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(pertext));
  list[2] = new sphere(vec3(0, 7, 0), 2, new diffuse_light(new constant_texture(vec3(4, 4, 4))));
  list[3] = new xy_rect(3, 5, 1, 3, -2, new diffuse_light(new constant_texture(vec3(4, 4, 4))));

  return new hitable_list(list, 4);
}

hitable *cornell_box() {
  hitable **list = new hitable*[6];
  int i=0;

  material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
  material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
  material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
  material *light = new diffuse_light(new constant_texture(vec3(15, 15, 15)));

  list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
  list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
  list[i++] = new xz_rect(213, 343, 227, 332, 554, light);
  list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
  list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
  list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));

  return new hitable_list(list, i);
}

int main()
{
  freopen("out.txt", "w", stdout);
  int nx = 600;
  int ny = 400;
  int ns = 500;
  cout << "P3\n"<< nx << " " << ny << "\n255\n";
  
  // hitable *world = random_scene();
  // hitable *world = two_spheres();
  // hitable *world = two_perlin_spheres();
  // hitable *world = earth();
  // hitable *world = simple_light();
  hitable *world = cornell_box();

  // vec3 lookfrom(13, 6, 10);
  // vec3 lookat(0, 0, 0);
  vec3 lookfrom(278, 278, -800);
  vec3 lookat(278, 278, 0);
  float dist_to_focus = 10.0;
  float aperture = 0.0;
  float vfov = 90;

  camera cam(lookfrom, lookat, vec3(0, 1, 0), vfov, float(nx) / float(ny), aperture, dist_to_focus,
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
