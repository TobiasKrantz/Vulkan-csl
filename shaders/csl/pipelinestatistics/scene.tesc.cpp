#include <csl/csl.h>

using namespace csl;

struct ControlInput
{
      [[csl::position]] vector<f32, 4>    position;
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 3> color;
      [[csl::location(2)]] vector<f32, 3> view_vector;
      [[csl::location(3)]] vector<f32, 3> light_vector;
};

struct ControlOutput
{
      [[csl::position]] vector<f32, 4>    position;
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 3> color;
      [[csl::location(2)]] vector<f32, 3> view_vector;
      [[csl::location(3)]] vector<f32, 3> light_vector;
};

[[csl::tess_control(3)]] void tess_control_main()
{
      if (invocation_id() == 0)
      {
            set_tess_level_inner(0, 2.0f);
            set_tess_level_outer(0, 1.0f);
            set_tess_level_outer(1, 1.0f);
            set_tess_level_outer(2, 1.0f);
      }

      const u32          id    = u32(invocation_id());
      const ControlInput input = stage_input<ControlInput>(id);

      ControlOutput output;
      output.position     = input.position;
      output.normal       = input.normal;
      output.color        = input.color;
      output.view_vector  = input.view_vector;
      output.light_vector = input.light_vector;
      set_tess_vertex(id, output);
}
