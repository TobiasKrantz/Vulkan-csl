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
      const f32 u     = f32((index << 1) & 2);
      const f32 v     = f32(index & 2);

      output.uv   = vector<f32, 2>{ u, v };
      output.clip = vector<f32, 4>{ u * 2.0f - 1.0f, v * 2.0f - 1.0f, 0.0f, 1.0f };
      return output;
}
