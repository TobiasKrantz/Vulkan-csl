#include <csl/csl.h>

using namespace csl;

struct CameraProperties
{
      matrix<f32, 4, 4> view_inverse;
      matrix<f32, 4, 4> projection_inverse;
      u32               frame;
};

struct Resources
{
      [[csl::binding(0, 0)]] AccelerationStructure                  top_level;
      [[csl::binding(0, 1)]] StorageImage<f32, ImageType::IMAGE_2D> image;
      [[csl::uniform_buffer(0, 2)]] const CameraProperties*         camera;
};

[[csl::ray_payload(0)]] vector<f32, 3> hit_value;
[[csl::ray_payload(3)]] u32            payload_seed;

static u32 tea(u32 value0, u32 value1)
{
      u32 sum = 0u;
      u32 v0  = value0;
      u32 v1  = value1;
      for (u32 n = 0u; n < 16u; n++)
      {
            sum += 0x9E3779B9u;
            v0 += ((v1 << 4u) + 0xA341316Cu) ^ (v1 + sum) ^ ((v1 >> 5u) + 0xC8013EA4u);
            v1 += ((v0 << 4u) + 0xAD90777Du) ^ (v0 + sum) ^ ((v0 >> 5u) + 0x7E95761Eu);
      }
      return v0;
}

static u32 lcg(u32& previous)
{
      previous = 1664525u * previous + 1013904223u;
      return previous & 0x00FFFFFFu;
}

static f32 rnd(u32& previous)
{
      return f32(lcg(previous)) / f32(0x01000000u);
}

[[csl::ray_generation]] void ray_generation_main()
{
      const Resources resources_ = resources<Resources>();

      const vector<u32, 3> id   = launch_id();
      const vector<u32, 3> size = launch_size();

      u32       seed = tea(id.y * size.x + id.x, resources_.camera->frame);
      const f32 r1   = rnd(seed);
      const f32 r2   = rnd(seed);

      const vector<f32, 2> jitter       = resources_.camera->frame == 0u ? vector<f32, 2>{ 0.5f, 0.5f } : vector<f32, 2>{ r1, r2 };
      const vector<f32, 2> pixel_center = vector<f32, 2>{ f32(id.x), f32(id.y) } + jitter;
      const vector<f32, 2> in_uv        = pixel_center / vector<f32, 2>{ f32(size.x), f32(size.y) };
      const vector<f32, 2> d            = in_uv * 2.0f - 1.0f;

      const vector<f32, 4> origin        = resources_.camera->view_inverse * vector<f32, 4>{ 0.0f, 0.0f, 0.0f, 1.0f };
      const vector<f32, 4> target        = resources_.camera->projection_inverse * vector<f32, 4>{ d.x, d.y, 1.0f, 1.0f };
      const vector<f32, 3> ray_direction = normalize(target.xyz);
      const vector<f32, 4> direction     = resources_.camera->view_inverse * vector<f32, 4>{ ray_direction.x, ray_direction.y, ray_direction.z, 0.0f };

      const f32 t_min = 0.001f;
      const f32 t_max = 10000.0f;

      hit_value                = vector<f32, 3>{ 0.0f, 0.0f, 0.0f };
      vector<f32, 3> hit_total = vector<f32, 3>{ 0.0f, 0.0f, 0.0f };

      const i32 samples = 4;
      for (i32 sample = 0; sample < samples; sample++)
      {
            payload_seed = tea(id.y * size.x + id.x, resources_.camera->frame);
            trace_ray(resources_.top_level, RayFlags::None, 0xff, 0, 0, 0, origin.xyz, t_min, direction.xyz, t_max, &hit_value);
            hit_total += hit_value;
      }

      const vector<f32, 3> color = hit_total / f32(samples);
      const vector<i32, 2>  pixel = vector<i32, 2>{ i32(id.x), i32(id.y) };

      if (resources_.camera->frame > 0u)
      {
            const f32            weight    = 1.0f / f32(resources_.camera->frame + 1u);
            const vector<f32, 3> old_color = resources_.image.load(pixel).xyz;
            const vector<f32, 3> blended   = lerp(old_color, color, weight);
            resources_.image.store(pixel, vector<f32, 4>{ blended.x, blended.y, blended.z, 1.0f });
      }
      else
            resources_.image.store(pixel, vector<f32, 4>{ color.x, color.y, color.z, 1.0f });
}
