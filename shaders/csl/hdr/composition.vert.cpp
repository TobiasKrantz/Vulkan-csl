#include <csl/csl.h>

using namespace csl;

struct VertexOutput
{
      [[csl::position]] vector<f32, 4>    clip;
      [[csl::location(0)]] vector<f32, 2> uv;
};

[[csl::vertex]] VertexOutput vertex_main()
{
      VertexOutput output;

      const u32            index = vertex_index();
      const vector<f32, 2> uv    = vector<f32, 2>{ f32((index << 1) & 2u), f32(index & 2u) };
      output.uv                  = uv;

      const vector<f32, 2> ndc = uv * 2.0f - 1.0f;
      output.clip              = vector<f32, 4>{ ndc.x, ndc.y, 0.0f, 1.0f };
      return output;
}
