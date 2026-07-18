#include <csl/csl.h>

using namespace csl;

#define AMBIENT 0.1f

struct Resources
{
      [[csl::binding(0, 1)]] CombinedSampler<f32, ImageType::IMAGE_2D> shadow_map;
};

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 3> color;
      [[csl::location(2)]] vector<f32, 3> view_vector;
      [[csl::location(3)]] vector<f32, 3> light_vector;
      [[csl::location(4)]] vector<f32, 4> shadow_coordinate;
};

struct FragmentOutput
{
      [[csl::location(0)]] vector<f32, 4> color;
};

[[csl::fragment]] FragmentOutput fragment_main()
{
      [[csl::constant(0)]] const i32 enable_pcf = 0;

      const Resources resource = resources<Resources>();
      FragmentInput   input    = stage_input<FragmentInput>();
      FragmentOutput  output;

      const vector<f32, 4> projected  = input.shadow_coordinate / input.shadow_coordinate.w;
      const vector<u32, 2> dimensions = resource.shadow_map.size(0);
      const f32            scale      = 1.5f;
      const i32            range      = (enable_pcf == 1) ? 1 : 0;
      const f32            dx         = (enable_pcf == 1) ? scale / f32(dimensions.x) : 0.0f;
      const f32            dy         = (enable_pcf == 1) ? scale / f32(dimensions.y) : 0.0f;

      f32 shadow_factor = 0.0f;
      i32 count         = 0;
      for (i32 x = -range; x <= range; x++)
      {
            for (i32 y = -range; y <= range; y++)
            {
                  const vector<f32, 2> offset   = vector<f32, 2>{ dx * f32(x), dy * f32(y) };
                  const f32            dist      = resource.shadow_map.sample(projected.xy + offset).x;
                  const bool           in_range  = projected.z > -1.0f && projected.z < 1.0f;
                  const bool           shadowed  = in_range && projected.w > 0.0f && dist < projected.z;
                  shadow_factor += shadowed ? AMBIENT : 1.0f;
                  count++;
            }
      }
      const f32 shadow = shadow_factor / f32(count);

      const vector<f32, 3> normal  = normalize(input.normal);
      const vector<f32, 3> light   = normalize(input.light_vector);
      const vector<f32, 3> diffuse = max(dot(normal, light), AMBIENT) * input.color;

      const vector<f32, 3> lit = diffuse * shadow;
      output.color             = vector<f32, 4>{ lit.x, lit.y, lit.z, 1.0f };
      return output;
}
