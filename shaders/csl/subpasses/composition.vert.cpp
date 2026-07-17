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

      const u32 index = vertex_index();
      const u32 x     = (index << 1) & 2u;
      const u32 y     = index & 2u;

      output.uv   = vector<f32, 2>{ static_cast<f32>(x), static_cast<f32>(y) };
      output.clip = vector<f32, 4>{ output.uv.x * 2.0f - 1.0f, output.uv.y * 2.0f - 1.0f, 0.0f, 1.0f };
      return output;
}
