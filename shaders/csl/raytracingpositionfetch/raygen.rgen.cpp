#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> view_inverse;
      matrix<f32, 4, 4> projection_inverse;
      vector<f32, 4>    light_position;
};

struct Resources
{
      [[csl::binding(0, 0)]] AccelerationStructure          top_level;
      [[csl::binding(0, 1)]] StorageImage<f32, ImageType::IMAGE_2D> image;
      [[csl::uniform_buffer(0, 2)]] const Ubo*              ubo;
};

[[csl::ray_payload(0)]] vector<f32, 3> hit_value;

[[csl::ray_generation]] void ray_generation_main()
{
      const Resources resources_ = resources<Resources>();

      const vector<u32, 3> id           = launch_id();
      const vector<f32, 2> pixel_center = vector<f32, 2>{ f32(id.x), f32(id.y) } + vector<f32, 2>{ 0.5f, 0.5f };
      const vector<f32, 2> in_uv        = pixel_center / vector<f32, 2>{ f32(launch_size().x), f32(launch_size().y) };
      const vector<f32, 2> d            = in_uv * 2.0f - 1.0f;

      const vector<f32, 4> origin        = resources_.ubo->view_inverse * vector<f32, 4>{ 0.0f, 0.0f, 0.0f, 1.0f };
      const vector<f32, 4> target        = resources_.ubo->projection_inverse * vector<f32, 4>{ d.x, d.y, 1.0f, 1.0f };
      const vector<f32, 3> ray_direction = normalize(target.xyz);
      const vector<f32, 4> direction     = resources_.ubo->view_inverse * vector<f32, 4>{ ray_direction.x, ray_direction.y, ray_direction.z, 0.0f };

      const f32 t_min = 0.001f;
      const f32 t_max = 10000.0f;

      hit_value = vector<f32, 3>{ 0.0f, 0.0f, 0.0f };

      trace_ray(resources_.top_level, RayFlags::Opaque, 0xff, 0, 0, 0, origin.xyz, t_min, direction.xyz, t_max, &hit_value);

      resources_.image.store(vector<i32, 2>{ i32(id.x), i32(id.y) }, vector<f32, 4>{ hit_value.x, hit_value.y, hit_value.z, 0.0f });
}
