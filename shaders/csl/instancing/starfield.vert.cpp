#include <csl/csl.h>

using namespace csl;

struct VertexOutput
{
      [[csl::position]] vector<f32, 4>    clip;
      [[csl::location(0)]] vector<f32, 3> uvw;
};

[[csl::vertex]] VertexOutput vertex_main()
{
      VertexOutput output;

      const u32 index = vertex_index();
      const f32 x     = f32((index << 1) & 2u);
      const f32 y     = f32(index & 2u);

      output.uvw              = vector<f32, 3>{ x, y, y };
      const vector<f32, 2> st = output.uvw.xy * 2.0f - 1.0f;
      output.clip             = vector<f32, 4>{ st.x, st.y, 0.0f, 1.0f };
      return output;
}
