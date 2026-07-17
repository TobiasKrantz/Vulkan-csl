#include <csl/csl.h>

using namespace csl;

struct Textures
{
      [[csl::binding(0, 1)]] CombinedSampler<f32, ImageType::IMAGE_2D_ARRAY> mat_cap;
};

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 3> color;
      [[csl::location(1)]] vector<f32, 3> eye_position;
      [[csl::location(2)]] vector<f32, 3> normal;
      [[csl::location(3)]] i32            texture_index;
};

struct FragmentOutput
{
      [[csl::location(0)]] vector<f32, 4> color;
};

[[csl::fragment]] FragmentOutput fragment_main()
{
      const Textures textures = resources<Textures>();
      FragmentInput  input    = stage_input<FragmentInput>();
      FragmentOutput output;

      const vector<f32, 3> r  = reflect(input.eye_position, input.normal);
      const vector<f32, 3> r2 = vector<f32, 3>{ r.x, r.y, r.z + 1.0f };
      const f32            m  = 2.0f * length(r2);
      const vector<f32, 2> vn = r.xy / m + 0.5f;

      const vector<f32, 3> sampled = textures.mat_cap.sample(vector<f32, 3>{ vn.x, vn.y, f32(input.texture_index) }).xyz;
      const f32            shade   = clamp(input.color.x * 2.0f, 0.0f, 1.0f);

      const vector<f32, 3> rgb = sampled * shade;
      output.color             = vector<f32, 4>{ rgb.x, rgb.y, rgb.z, 1.0f };
      return output;
}
