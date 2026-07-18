#include <csl/csl.h>

using namespace csl;

struct VertexInput
{
      [[csl::location(0)]] vector<f32, 3> position;
      [[csl::location(1)]] vector<f32, 3> normal;
      [[csl::location(2)]] vector<f32, 2> uv;
};

struct VertexOutput
{
      [[csl::position]] vector<f32, 4>    position;
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 2> uv;
};

[[csl::vertex]] VertexOutput vertex_main()
{
      VertexInput  input = stage_input<VertexInput>();
      VertexOutput output;

      output.position = vector<f32, 4>{ input.position.x, input.position.y, input.position.z, 1.0f };
      output.uv       = input.uv;
      output.normal   = input.normal;
      return output;
}
