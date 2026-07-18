#include <csl/csl.h>

using namespace csl;

struct VertexOutput
{
      [[csl::position]] vector<f32, 4> clip;
};

[[csl::vertex]] VertexOutput vertex_main()
{
      VertexOutput output;

      const u32 index = vertex_index();
      const f32 x     = f32((index << 1) & 2u);
      const f32 y     = f32(index & 2u);

      const vector<f32, 2> uv = vector<f32, 2>{ x, y } * 2.0f - 1.0f;
      output.clip             = vector<f32, 4>{ uv.x, uv.y, 0.0f, 1.0f };
      return output;
}
