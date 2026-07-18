#include <csl/csl.h>

using namespace csl;

struct VertexInput
{
      [[csl::location(0)]] vector<f32, 2> position;
      [[csl::location(1)]] vector<f32, 2> uv;
};

struct VertexOutput
{
      [[csl::position]] vector<f32, 4>    clip;
      [[csl::location(0)]] vector<f32, 2> uv;
};

[[csl::vertex]] VertexOutput vertex_main()
{
      VertexInput  input = stage_input<VertexInput>();
      VertexOutput output;
      output.uv   = input.uv;
      output.clip = vector<f32, 4>{ input.position.x, input.position.y, 0.0f, 1.0f };
      return output;
}
