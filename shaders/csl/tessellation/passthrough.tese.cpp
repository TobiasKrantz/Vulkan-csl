#include <csl/csl.h>

using namespace csl;

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
      [[csl::location(1)]] vector<f32, 2> uv;
};

struct EvalOutput
{
      [[csl::position]] vector<f32, 4>    position;
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 2> uv;
};

[[csl::tess_eval(TessPrimitive::Triangles, TessSpacing::FractionalOdd, TessWinding::Cw)]] EvalOutput tess_eval_main()
{
      const Resources      resources_ = resources<Resources>();
      const vector<f32, 3> coord      = tess_coord();

      const EvalInput v0 = stage_input<EvalInput>(0u);
      const EvalInput v1 = stage_input<EvalInput>(1u);
      const EvalInput v2 = stage_input<EvalInput>(2u);

      EvalOutput output;
      const vector<f32, 4> position = coord.x * v0.position + coord.y * v1.position + coord.z * v2.position;
      output.position               = resources_.ubo->projection * resources_.ubo->model * position;
      output.normal                 = coord.x * v0.normal + coord.y * v1.normal + coord.z * v2.normal;
      output.uv                     = coord.x * v0.uv + coord.y * v1.uv + coord.z * v2.uv;
      return output;
}
