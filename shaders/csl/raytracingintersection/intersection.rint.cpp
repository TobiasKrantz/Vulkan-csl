#include <csl/csl.h>

using namespace csl;

struct Sphere
{
      vector<f32, 3> center;
      f32            radius;
      vector<f32, 4> color;
};

struct Resources
{
      [[csl::binding(0, 3)]] Sphere* spheres;
};

static f32 sphere_intersect(Sphere sphere, vector<f32, 3> ray_origin, vector<f32, 3> ray_direction)
{
      const vector<f32, 3> oc = ray_origin - sphere.center;
      const f32            b  = dot(oc, ray_direction);
      const f32            c  = dot(oc, oc) - sphere.radius * sphere.radius;
      f32                  h  = b * b - c;
      if (h < 0.0f)
            return -1.0f;
      h = sqrt(h);
      return -b - h;
}

[[csl::intersection]] void intersection_main()
{
      const Resources resources_ = resources<Resources>();

      const Sphere sphere = resources_.spheres[primitive_id()];
      const f32    hit    = sphere_intersect(sphere, world_ray_origin(), world_ray_direction());
      if (hit > 0.0f)
            report_intersection(hit, 0);
}
