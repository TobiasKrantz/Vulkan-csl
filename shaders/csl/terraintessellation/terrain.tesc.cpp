#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> modelview;
      vector<f32, 4>    light_position;
      vector<f32, 4>    frustum_planes[6];
      f32               displacement_factor;
      f32               tessellation_factor;
      vector<f32, 2>    viewport_dimensions;
      f32               tessellated_edge_size;
};

struct Resources
{
      [[csl::uniform_buffer(0, 0)]] const Ubo*                       ubo;
      [[csl::binding(0, 1)]] CombinedSampler<f32, ImageType::IMAGE_2D> height_map;
};

struct ControlInput
{
      [[csl::position]] vector<f32, 4>    position;
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 2> uv;
};

struct ControlOutput
{
      [[csl::position]] vector<f32, 4>    position;
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 2> uv;
};

static f32 screen_space_tess_factor(vector<f32, 4> p0, vector<f32, 4> p1, matrix<f32, 4, 4> modelview, matrix<f32, 4, 4> projection, vector<f32, 2> viewport, f32 edge_size, f32 factor)
{
      const vector<f32, 4> mid_point = 0.5f * (p0 + p1);
      const f32            radius    = distance(p0, p1) / 2.0f;

      const vector<f32, 4> view  = modelview * mid_point;
      vector<f32, 4>       clip0 = projection * (view - vector<f32, 4>{ radius, 0.0f, 0.0f, 0.0f });
      vector<f32, 4>       clip1 = projection * (view + vector<f32, 4>{ radius, 0.0f, 0.0f, 0.0f });

      clip0 = clip0 / clip0.w;
      clip1 = clip1 / clip1.w;

      clip0 = vector<f32, 4>{ clip0.x * viewport.x, clip0.y * viewport.y, clip0.z, clip0.w };
      clip1 = vector<f32, 4>{ clip1.x * viewport.x, clip1.y * viewport.y, clip1.z, clip1.w };

      return clamp(distance(clip0, clip1) / edge_size * factor, 1.0f, 64.0f);
}

[[csl::tess_control(4)]] void tess_control_main()
{
      const Resources resources_ = resources<Resources>();
      const u32       id         = u32(invocation_id());

      if (invocation_id() == 0)
      {
            const ControlInput   corner = stage_input<ControlInput>(0u);
            const f32            radius = 8.0f;
            const f32            height = resources_.height_map.sample_lod(corner.uv, 0.0f).x * resources_.ubo->displacement_factor;
            const vector<f32, 4> sphere = vector<f32, 4>{ corner.position.x, corner.position.y - height, corner.position.z, corner.position.w };

            bool visible = true;
            for (i32 i = 0; i < 6; i++)
                  if (dot(sphere, resources_.ubo->frustum_planes[i]) + radius < 0.0f)
                        visible = false;

            if (!visible)
            {
                  set_tess_level_inner(0, 0.0f);
                  set_tess_level_inner(1, 0.0f);
                  set_tess_level_outer(0, 0.0f);
                  set_tess_level_outer(1, 0.0f);
                  set_tess_level_outer(2, 0.0f);
                  set_tess_level_outer(3, 0.0f);
            }
            else if (resources_.ubo->tessellation_factor > 0.0f)
            {
                  const ControlInput   corner0 = stage_input<ControlInput>(0u);
                  const ControlInput   corner1 = stage_input<ControlInput>(1u);
                  const ControlInput   corner2 = stage_input<ControlInput>(2u);
                  const ControlInput   corner3 = stage_input<ControlInput>(3u);
                  const vector<f32, 4> p0      = corner0.position;
                  const vector<f32, 4> p1      = corner1.position;
                  const vector<f32, 4> p2      = corner2.position;
                  const vector<f32, 4> p3      = corner3.position;

                  const matrix<f32, 4, 4> modelview  = resources_.ubo->modelview;
                  const matrix<f32, 4, 4> projection = resources_.ubo->projection;
                  const vector<f32, 2>    viewport   = resources_.ubo->viewport_dimensions;
                  const f32               edge       = resources_.ubo->tessellated_edge_size;
                  const f32               factor     = resources_.ubo->tessellation_factor;

                  const f32 outer0 = screen_space_tess_factor(p3, p0, modelview, projection, viewport, edge, factor);
                  const f32 outer1 = screen_space_tess_factor(p0, p1, modelview, projection, viewport, edge, factor);
                  const f32 outer2 = screen_space_tess_factor(p1, p2, modelview, projection, viewport, edge, factor);
                  const f32 outer3 = screen_space_tess_factor(p2, p3, modelview, projection, viewport, edge, factor);

                  set_tess_level_outer(0, outer0);
                  set_tess_level_outer(1, outer1);
                  set_tess_level_outer(2, outer2);
                  set_tess_level_outer(3, outer3);
                  set_tess_level_inner(0, lerp(outer0, outer3, 0.5f));
                  set_tess_level_inner(1, lerp(outer2, outer1, 0.5f));
            }
            else
            {
                  set_tess_level_inner(0, 1.0f);
                  set_tess_level_inner(1, 1.0f);
                  set_tess_level_outer(0, 1.0f);
                  set_tess_level_outer(1, 1.0f);
                  set_tess_level_outer(2, 1.0f);
                  set_tess_level_outer(3, 1.0f);
            }
      }

      const ControlInput input = stage_input<ControlInput>(id);
      ControlOutput      output;
      output.position = input.position;
      output.normal   = input.normal;
      output.uv       = input.uv;
      set_tess_vertex(id, output);
}
