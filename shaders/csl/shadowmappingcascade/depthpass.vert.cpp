#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> cascade_view_projection[4];
};

struct Scene
{
      [[csl::uniform_buffer(0, 3)]] const Ubo* ubo;
};

struct PushConstants
{
      vector<f32, 4> position;
      u32            cascade_index;
};

struct VertexInput
{
      [[csl::location(0)]] vector<f32, 3> position;
      [[csl::location(1)]] vector<f32, 2> uv;
};

struct VertexOutput
{
      [[csl::position]] vector<f32, 4>    clip;
      [[csl::location(0)]] vector<f32, 2> uv;
};

[[csl::vertex]] VertexOutput vertex_main(PushConstants push)
{
      const Scene  scene = resources<Scene>();
      VertexInput  input = stage_input<VertexInput>();
      VertexOutput output;

      output.uv = input.uv;

      const vector<f32, 3> position = input.position + push.position.xyz;
      output.clip                   = scene.ubo->cascade_view_projection[push.cascade_index] * vector<f32, 4>{ position.x, position.y, position.z, 1.0f };
      return output;
}
