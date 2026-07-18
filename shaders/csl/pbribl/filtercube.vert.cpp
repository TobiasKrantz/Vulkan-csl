#include <csl/csl.h>

using namespace csl;

struct PushConstants
{
      matrix<f32, 4, 4> mvp;
};

struct VertexInput
{
      [[csl::location(0)]] vector<f32, 3> position;
};

struct VertexOutput
{
      [[csl::position]] vector<f32, 4>    clip;
      [[csl::location(0)]] vector<f32, 3> uvw;
};

[[csl::vertex]] VertexOutput vertex_main(PushConstants push)
{
      VertexInput  input = stage_input<VertexInput>();
      VertexOutput output;

      output.uvw  = input.position;
      output.clip = push.mvp * vector<f32, 4>{ input.position.x, input.position.y, input.position.z, 1.0f };
      return output;
}
