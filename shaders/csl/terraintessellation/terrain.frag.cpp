#include <csl/csl.h>

using namespace csl;

struct Resources
{
      [[csl::binding(0, 1)]] CombinedSampler<f32, ImageType::IMAGE_2D>       height;
      [[csl::binding(0, 2)]] CombinedSampler<f32, ImageType::IMAGE_2D_ARRAY> layers;
};

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 2> uv;
      [[csl::location(2)]] vector<f32, 3> view_vector;
      [[csl::location(3)]] vector<f32, 3> light_vector;
      [[csl::location(4)]] vector<f32, 3> eye_position;
      [[csl::location(5)]] vector<f32, 3> world_position;
};

struct FragmentOutput
{
      [[csl::location(0)]] vector<f32, 4> color;
};

static f32 fog(f32 density, f32 frag_z, f32 frag_w)
{
      const f32 LOG2 = -1.442695f;
      const f32 dist = frag_z / frag_w * 0.1f;
      const f32 d    = density * dist;
      return 1.0f - clamp(exp2(d * d * LOG2), 0.0f, 1.0f);
}

[[csl::fragment]] FragmentOutput fragment_main()
{
      const Resources resource = resources<Resources>();
      FragmentInput   input    = stage_input<FragmentInput>();
      FragmentOutput  output;

      vector<f32, 2> layers[6];
      layers[0] = vector<f32, 2>{ -10.0f, 10.0f };
      layers[1] = vector<f32, 2>{ 5.0f, 45.0f };
      layers[2] = vector<f32, 2>{ 45.0f, 80.0f };
      layers[3] = vector<f32, 2>{ 75.0f, 100.0f };
      layers[4] = vector<f32, 2>{ 95.0f, 140.0f };
      layers[5] = vector<f32, 2>{ 140.0f, 190.0f };

      vector<f32, 3> terrain_color = vector<f32, 3>{ 0.0f, 0.0f, 0.0f };

      const f32 height = resource.height.sample_lod(input.uv, 0.0f).x * 255.0f;

      for (i32 i = 0; i < 6; ++i)
      {
            const f32 range  = layers[i].y - layers[i].x;
            f32       weight = (range - abs(height - layers[i].y)) / range;
            weight           = max(0.0f, weight);

            const vector<f32, 2> scaled = input.uv * 16.0f;
            const vector<f32, 3> coord  = vector<f32, 3>{ scaled.x, scaled.y, f32(i) };
            terrain_color               = terrain_color + weight * resource.layers.sample(coord).xyz;
      }

      const vector<f32, 3> n       = normalize(input.normal);
      const vector<f32, 3> l       = normalize(input.light_vector);
      const vector<f32, 3> ambient = vector<f32, 3>{ 0.5f, 0.5f, 0.5f };
      const vector<f32, 3> diffuse = max(dot(n, l), 0.0f) * vector<f32, 3>{ 1.0f, 1.0f, 1.0f };

      const vector<f32, 3> shaded = (ambient + diffuse) * terrain_color;
      const vector<f32, 4> color  = vector<f32, 4>{ shaded.x, shaded.y, shaded.z, 1.0f };

      const vector<f32, 4> fog_color = vector<f32, 4>{ 0.47f, 0.5f, 0.67f, 0.0f };
      const vector<f32, 4> coord     = frag_coord();
      const f32            factor    = fog(0.25f, coord.z, coord.w);

      output.color = lerp(color, fog_color, vector<f32, 4>{ factor, factor, factor, factor });
      return output;
}
