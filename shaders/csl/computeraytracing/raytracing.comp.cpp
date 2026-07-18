#include <csl/csl.h>

using namespace csl;

constexpr f32 EPSILON            = 0.0001f;
constexpr f32 MAX_LENGTH         = 1000.0f;
constexpr f32 SHADOW             = 0.5f;
constexpr i32 RAY_BOUNCES        = 2;
constexpr f32 REFLECTION_STRENGTH = 0.4f;
constexpr f32 REFLECTION_FALLOFF  = 0.5f;

constexpr i32 OBJECT_TYPE_SPHERE = 0;
constexpr i32 OBJECT_TYPE_PLANE  = 1;

struct Camera
{
      vector<f32, 3> pos;
      vector<f32, 3> lookat;
      f32            fov;
};

struct Ubo
{
      vector<f32, 4>    light_pos_aspect;
      vector<f32, 4>    fog_color;
      Camera            camera;
      matrix<f32, 4, 4> rot_mat;
};

struct SceneObject
{
      vector<f32, 4> object_properties;
      vector<f32, 4> diffuse_specular;
      i32            id;
      i32            object_type;
};

struct Resources
{
      [[csl::binding(0, 0)]] StorageImage<f32, ImageType::IMAGE_2D, false, ImageFormat::Rgba8Unorm> result_image;
      [[csl::uniform_buffer(0, 1)]] const Ubo*                                                      ubo;
      [[csl::binding(0, 2)]] SceneObject*                                                           scene_objects;
};

static vector<f32, 3> reflect_ray(vector<f32, 3> ray_direction, vector<f32, 3> normal)
{
      return ray_direction + 2.0f * -dot(normal, ray_direction) * normal;
}

static f32 light_diffuse(vector<f32, 3> normal, vector<f32, 3> light_direction)
{
      return clamp(dot(normal, light_direction), 0.1f, 1.0f);
}

static f32 light_specular(vector<f32, 3> normal, vector<f32, 3> light_direction, f32 specular_factor, vector<f32, 3> camera_pos)
{
      vector<f32, 3> view_vector = normalize(camera_pos);
      vector<f32, 3> half_vector = normalize(light_direction + view_vector);
      return pow(clamp(dot(normal, half_vector), 0.0f, 1.0f), specular_factor);
}

static f32 sphere_intersect(vector<f32, 3> ray_origin, vector<f32, 3> ray_direction, SceneObject sphere)
{
      vector<f32, 3> oc = ray_origin - sphere.object_properties.xyz;
      f32            b  = 2.0f * dot(oc, ray_direction);
      f32            c  = dot(oc, oc) - sphere.object_properties.w * sphere.object_properties.w;
      f32            h  = b * b - 4.0f * c;
      if (h < 0.0f)
            return -1.0f;
      return (-b - sqrt(h)) / 2.0f;
}

static vector<f32, 3> sphere_normal(vector<f32, 3> pos, SceneObject sphere)
{
      return (pos - sphere.object_properties.xyz) / sphere.object_properties.w;
}

static f32 plane_intersect(vector<f32, 3> ray_origin, vector<f32, 3> ray_direction, SceneObject plane)
{
      f32 d = dot(ray_direction, plane.object_properties.xyz);
      if (d == 0.0f)
            return 0.0f;
      f32 t = -(plane.object_properties.w + dot(ray_origin, plane.object_properties.xyz)) / d;
      if (t < 0.0f)
            return 0.0f;
      return t;
}

static vector<f32, 3> fog(f32 t, vector<f32, 3> color, vector<f32, 4> fog_color)
{
      f32 amount = clamp(sqrt(t * t) / 20.0f, 0.0f, 1.0f);
      return lerp(color, fog_color.xyz, vector<f32, 3>{ amount, amount, amount });
}

[[csl::compute(16, 16, 1)]] void compute_main()
{
      const Resources resources_ = resources<Resources>();

      vector<u32, 2> extent      = resources_.result_image.size();
      vector<u32, 2> pixel       = dispatch_id().xy;
      vector<f32, 2> uv          = vector<f32, 2>{ f32(pixel.x), f32(pixel.y) } / vector<f32, 2>{ f32(extent.x), f32(extent.y) };

      vector<f32, 3> camera_pos    = resources_.ubo->camera.pos;
      vector<f32, 4> fog_color     = resources_.ubo->fog_color;
      vector<f32, 3> light_pos     = resources_.ubo->light_pos_aspect.xyz;
      f32            aspect_ratio  = resources_.ubo->light_pos_aspect.w;

      vector<f32, 3> ray_origin    = camera_pos;
      vector<f32, 2> screen        = (vector<f32, 2>{ -1.0f, -1.0f } + 2.0f * uv) * vector<f32, 2>{ aspect_ratio, 1.0f };
      vector<f32, 3> ray_direction = normalize(vector<f32, 3>{ screen.x, screen.y, -1.0f });

      i32            id                 = 0;
      vector<f32, 3> final_color        = vector<f32, 3>{ 0.0f, 0.0f, 0.0f };
      f32            reflection_strength = REFLECTION_STRENGTH;
      u32            count               = array_length(resources_.scene_objects);

      for (i32 pass = 0; pass <= RAY_BOUNCES; pass++)
      {
            vector<f32, 3> color = vector<f32, 3>{ 0.0f, 0.0f, 0.0f };
            f32            t     = MAX_LENGTH;

            i32 object_id = -1;
            for (u32 i = 0; i < count; i++)
            {
                  f32 hit = -1000.0f;
                  if (resources_.scene_objects[i].object_type == OBJECT_TYPE_SPHERE)
                        hit = sphere_intersect(ray_origin, ray_direction, resources_.scene_objects[i]);
                  if (resources_.scene_objects[i].object_type == OBJECT_TYPE_PLANE)
                        hit = plane_intersect(ray_origin, ray_direction, resources_.scene_objects[i]);
                  if ((hit > EPSILON) && (hit < t))
                  {
                        object_id = resources_.scene_objects[i].id;
                        t         = hit;
                  }
            }

            if (object_id != -1)
            {
                  vector<f32, 3> pos        = ray_origin + t * ray_direction;
                  vector<f32, 3> light_vec  = normalize(light_pos - pos);
                  vector<f32, 3> normal     = vector<f32, 3>{ 0.0f, 0.0f, 0.0f };

                  for (u32 i = 0; i < count; i++)
                  {
                        if (object_id == resources_.scene_objects[i].id)
                        {
                              if (resources_.scene_objects[i].object_type == OBJECT_TYPE_SPHERE)
                                    normal = sphere_normal(pos, resources_.scene_objects[i]);
                              if (resources_.scene_objects[i].object_type == OBJECT_TYPE_PLANE)
                                    normal = resources_.scene_objects[i].object_properties.xyz;
                              f32 diffuse  = light_diffuse(normal, light_vec);
                              f32 specular = light_specular(normal, light_vec, resources_.scene_objects[i].diffuse_specular.w, camera_pos);
                              color        = diffuse * resources_.scene_objects[i].diffuse_specular.xyz + specular;
                        }
                  }

                  if (id != -1)
                  {
                        id = object_id;

                        t = length(light_pos - pos);

                        f32 shadow_factor = 1.0f;
                        for (u32 i = 0; i < count; i++)
                        {
                              if (resources_.scene_objects[i].id == id)
                                    continue;
                              f32 shadow_t = MAX_LENGTH;
                              if (resources_.scene_objects[i].object_type == OBJECT_TYPE_SPHERE)
                                    shadow_t = sphere_intersect(pos, light_vec, resources_.scene_objects[i]);
                              if (resources_.scene_objects[i].object_type == OBJECT_TYPE_PLANE)
                                    shadow_t = plane_intersect(pos, light_vec, resources_.scene_objects[i]);
                              if ((shadow_t > EPSILON) && (shadow_t < t))
                              {
                                    t             = shadow_t;
                                    shadow_factor = SHADOW;
                                    break;
                              }
                        }
                        color *= shadow_factor;

                        color = fog(t, color, fog_color);

                        ray_direction = reflect_ray(ray_direction, normal);
                        ray_origin    = pos;
                  }
            }

            if (pass == 0)
            {
                  final_color = color;
            }
            else
            {
                  f32 blend   = 1.0f - reflection_strength;
                  final_color = blend * final_color + reflection_strength * lerp(color, final_color, vector<f32, 3>{ blend, blend, blend });
                  reflection_strength *= REFLECTION_FALLOFF;
            }
      }

      resources_.result_image.store(vector<i32, 2>{ i32(pixel.x), i32(pixel.y) }, vector<f32, 4>{ final_color.x, final_color.y, final_color.z, 0.0f });
}
