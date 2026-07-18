#include <csl/csl.h>

using namespace csl;

struct RayPayload
{
      vector<f32, 3> color;
      f32            distance;
      vector<f32, 3> normal;
      f32            reflector;
};

[[csl::ray_payload_in(0)]] RayPayload ray_payload;

[[csl::miss]] void miss_main()
{
      const vector<f32, 3> gradient_start = vector<f32, 3>{ 0.5f, 0.6f, 1.0f };
      const vector<f32, 3> gradient_end   = vector<f32, 3>{ 1.0f, 1.0f, 1.0f };
      const vector<f32, 3> unit_direction = normalize(world_ray_direction());
      const f32            t              = 0.5f * (unit_direction.y + 1.0f);

      ray_payload.color     = gradient_start * (1.0f - t) + gradient_end * t;
      ray_payload.distance  = -1.0f;
      ray_payload.normal    = vector<f32, 3>{ 0.0f, 0.0f, 0.0f };
      ray_payload.reflector = 0.0f;
}
