#include <csl/csl.h>

using namespace csl;

struct CameraProperties
{
      matrix<f32, 4, 4> view_inverse;
      matrix<f32, 4, 4> projection_inverse;
      vector<f32, 4>    light_position;
};

struct RayPayload
{
      vector<f32, 3> color;
      f32            distance;
      vector<f32, 3> normal;
      f32            reflector;
};

struct Resources
{
      [[csl::binding(0, 0)]] AccelerationStructure          top_level;
      [[csl::binding(0, 1)]] StorageImage<f32, ImageType::IMAGE_2D> image;
      [[csl::uniform_buffer(0, 2)]] const CameraProperties* camera;
};

[[csl::ray_payload(0)]] RayPayload ray_payload;

[[csl::ray_generation]] void ray_generation_main()
{
      [[csl::constant(0)]] const i32 MAX_RECURSION = 0;

      const Resources resources_ = resources<Resources>();

      const vector<u32, 3> id           = launch_id();
      const vector<f32, 2> pixel_center = vector<f32, 2>{ f32(id.x), f32(id.y) } + vector<f32, 2>{ 0.5f, 0.5f };
      const vector<f32, 2> in_uv        = pixel_center / vector<f32, 2>{ f32(launch_size().x), f32(launch_size().y) };
      const vector<f32, 2> d            = in_uv * 2.0f - 1.0f;

      const vector<f32, 4> target        = resources_.camera->projection_inverse * vector<f32, 4>{ d.x, d.y, 1.0f, 1.0f };
      const vector<f32, 3> ray_direction = normalize(target.xyz / target.w);

      vector<f32, 4> origin    = resources_.camera->view_inverse * vector<f32, 4>{ 0.0f, 0.0f, 0.0f, 1.0f };
      vector<f32, 4> direction = resources_.camera->view_inverse * vector<f32, 4>{ ray_direction.x, ray_direction.y, ray_direction.z, 0.0f };

      const f32 t_min = 0.001f;
      const f32 t_max = 10000.0f;

      vector<f32, 3> color = vector<f32, 3>{ 0.0f, 0.0f, 0.0f };

      for (i32 i = 0; i < MAX_RECURSION; i++)
      {
            trace_ray(resources_.top_level, RayFlags::Opaque, 0xff, 0, 0, 0, origin.xyz, t_min, direction.xyz, t_max, &ray_payload);
            const vector<f32, 3> hit_color = ray_payload.color;

            if (ray_payload.distance < 0.0f)
            {
                  color = color + hit_color;
                  break;
            }
            else if (ray_payload.reflector == 1.0f)
            {
                  const vector<f32, 4> hit_position = origin + direction * ray_payload.distance;
                  origin.xyz                        = hit_position.xyz + ray_payload.normal * 0.001f;
                  direction.xyz                     = reflect(direction.xyz, ray_payload.normal);
            }
            else
            {
                  color = color + hit_color;
                  break;
            }
      }

      resources_.image.store(vector<i32, 2>{ i32(id.x), i32(id.y) }, vector<f32, 4>{ color.x, color.y, color.z, 0.0f });
}
