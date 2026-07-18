#include <csl/csl.h>

using namespace csl;

struct PnPatch
{
      f32 b210;
      f32 b120;
      f32 b021;
      f32 b012;
      f32 b102;
      f32 b201;
      f32 b111;
      f32 n110;
      f32 n011;
      f32 n101;
};

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> model;
      f32               tess_alpha;
      f32               tess_level;
};

struct Resources
{
      [[csl::uniform_buffer(0, 0)]] const Ubo* ubo;
};

struct EvalInput
{
      [[csl::position]] vector<f32, 4>    position;
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(3)]] vector<f32, 2> uv;
      [[csl::location(6)]] PnPatch        patch;
};

struct EvalOutput
{
      [[csl::position]] vector<f32, 4>    position;
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 2> uv;
};

[[csl::tess_eval(TessPrimitive::Triangles, TessSpacing::FractionalOdd, TessWinding::Cw)]] EvalOutput tess_eval_main()
{
      const Resources resources_ = resources<Resources>();

      const EvalInput p0 = stage_input<EvalInput>(0u);
      const EvalInput p1 = stage_input<EvalInput>(1u);
      const EvalInput p2 = stage_input<EvalInput>(2u);

      const vector<f32, 3> uvw         = tess_coord();
      const vector<f32, 3> uvw_squared = uvw * uvw;
      const vector<f32, 3> uvw_cubed   = uvw_squared * uvw;

      const vector<f32, 3> b210 = vector<f32, 3>{ p0.patch.b210, p1.patch.b210, p2.patch.b210 };
      const vector<f32, 3> b120 = vector<f32, 3>{ p0.patch.b120, p1.patch.b120, p2.patch.b120 };
      const vector<f32, 3> b021 = vector<f32, 3>{ p0.patch.b021, p1.patch.b021, p2.patch.b021 };
      const vector<f32, 3> b012 = vector<f32, 3>{ p0.patch.b012, p1.patch.b012, p2.patch.b012 };
      const vector<f32, 3> b102 = vector<f32, 3>{ p0.patch.b102, p1.patch.b102, p2.patch.b102 };
      const vector<f32, 3> b201 = vector<f32, 3>{ p0.patch.b201, p1.patch.b201, p2.patch.b201 };
      const vector<f32, 3> b111 = vector<f32, 3>{ p0.patch.b111, p1.patch.b111, p2.patch.b111 };

      const vector<f32, 3> n110 = normalize(vector<f32, 3>{ p0.patch.n110, p1.patch.n110, p2.patch.n110 });
      const vector<f32, 3> n011 = normalize(vector<f32, 3>{ p0.patch.n011, p1.patch.n011, p2.patch.n011 });
      const vector<f32, 3> n101 = normalize(vector<f32, 3>{ p0.patch.n101, p1.patch.n101, p2.patch.n101 });

      EvalOutput output;
      output.uv = p0.uv * uvw.z + p1.uv * uvw.x + p2.uv * uvw.y;

      const vector<f32, 3> bar_normal = p0.normal * uvw.z + p1.normal * uvw.x + p2.normal * uvw.y;
      const vector<f32, 3> pn_normal  = p0.normal * uvw_squared.z + p1.normal * uvw_squared.x + p2.normal * uvw_squared.y + n110 * (uvw.z * uvw.x) + n011 * (uvw.x * uvw.y) + n101 * (uvw.z * uvw.y);
      output.normal                   = pn_normal * resources_.ubo->tess_alpha + bar_normal * (1.0f - resources_.ubo->tess_alpha);

      const vector<f32, 3> bar_position    = p0.position.xyz * uvw.z + p1.position.xyz * uvw.x + p2.position.xyz * uvw.y;
      const vector<f32, 3> uvw_squared_3   = uvw_squared * 3.0f;
      const vector<f32, 3> pn_position     = p0.position.xyz * uvw_cubed.z + p1.position.xyz * uvw_cubed.x + p2.position.xyz * uvw_cubed.y + b210 * (uvw_squared_3.z * uvw.x)
                                         + b120 * (uvw_squared_3.x * uvw.z) + b201 * (uvw_squared_3.z * uvw.y) + b021 * (uvw_squared_3.x * uvw.y) + b102 * (uvw_squared_3.y * uvw.z)
                                         + b012 * (uvw_squared_3.y * uvw.x) + b111 * (6.0f * uvw.x * uvw.y * uvw.z);
      const vector<f32, 3> final_position  = bar_position * (1.0f - resources_.ubo->tess_alpha) + pn_position * resources_.ubo->tess_alpha;

      output.position = resources_.ubo->projection * resources_.ubo->model * vector<f32, 4>{ final_position.x, final_position.y, final_position.z, 1.0f };
      return output;
}
