#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> modelview;
      vector<f32, 4>    light_position;
      f32               tess_alpha;
      f32               tess_strength;
      f32               tess_level;
};

struct Resources
{
      [[csl::uniform_buffer(0, 0)]] const Ubo* ubo;
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

[[csl::tess_control(3)]] void tess_control_main()
{
      const Resources resources_ = resources<Resources>();

      if (invocation_id() == 0)
      {
            set_tess_level_inner(0, resources_.ubo->tess_level);
            set_tess_level_outer(0, resources_.ubo->tess_level);
            set_tess_level_outer(1, resources_.ubo->tess_level);
            set_tess_level_outer(2, resources_.ubo->tess_level);
      }

      const u32          id    = u32(invocation_id());
      const ControlInput input = stage_input<ControlInput>(id);

      ControlOutput output;
      output.position = input.position;
      output.normal   = input.normal;
      output.uv       = input.uv;
      set_tess_vertex(id, output);
}
