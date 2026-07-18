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
      const u32    index = vertex_index();
      output.uv          = vector<f32, 2>{ f32((index << 1u) & 2u), f32(index & 2u) };
      output.clip        = vector<f32, 4>{ output.uv.x * 2.0f - 1.0f, output.uv.y * 2.0f - 1.0f, 0.0f, 1.0f };
      return output;
}
