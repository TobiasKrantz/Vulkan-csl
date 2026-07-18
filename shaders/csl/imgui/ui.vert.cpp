#include <csl/csl.h>

using namespace csl;

struct PushConstants
{
      vector<f32, 2> scale;
      vector<f32, 2> translate;
};

struct VertexInput
{
      [[csl::location(0)]] vector<f32, 2> position;
      [[csl::location(1)]] vector<f32, 2> uv;
      [[csl::location(2)]] vector<f32, 4> color;
};

struct VertexOutput
{
      [[csl::position]] vector<f32, 4>    clip;
      [[csl::location(0)]] vector<f32, 2> uv;
      [[csl::location(1)]] vector<f32, 4> color;
};

[[csl::vertex]] VertexOutput vertex_main(PushConstants push)
{
      VertexInput  input = stage_input<VertexInput>();
      VertexOutput output;

      output.uv    = input.uv;
      output.color = input.color;

      const vector<f32, 2> screen = input.position * push.scale + push.translate;
      output.clip                 = vector<f32, 4>{ screen.x, screen.y, 0.0f, 1.0f };
      return output;
}
