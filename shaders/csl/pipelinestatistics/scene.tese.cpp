#include <csl/csl.h>

using namespace csl;

struct EvalInput
{
      [[csl::position]] vector<f32, 4>    position;
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 3> color;
      [[csl::location(2)]] vector<f32, 3> view_vector;
      [[csl::location(3)]] vector<f32, 3> light_vector;
};

struct EvalOutput
{
      [[csl::position]] vector<f32, 4>    position;
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 3> color;
      [[csl::location(2)]] vector<f32, 3> view_vector;
      [[csl::location(3)]] vector<f32, 3> light_vector;
};

[[csl::tess_eval(TessPrimitive::Triangles, TessSpacing::Equal, TessWinding::Ccw)]] EvalOutput tess_eval_main()
{
      const vector<f32, 3> coord = tess_coord();

      const EvalInput v0 = stage_input<EvalInput>(0u);
      const EvalInput v1 = stage_input<EvalInput>(1u);
      const EvalInput v2 = stage_input<EvalInput>(2u);

      EvalOutput output;
      output.position     = coord.x * v2.position + coord.y * v1.position + coord.z * v0.position;
      output.normal       = coord.x * v2.normal + coord.y * v1.normal + coord.z * v0.normal;
      output.view_vector  = coord.x * v2.view_vector + coord.y * v1.view_vector + coord.z * v0.view_vector;
      output.light_vector = coord.x * v2.light_vector + coord.y * v1.light_vector + coord.z * v0.light_vector;
      output.color        = v0.color;
      return output;
}
