#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> modelview;
      vector<f32, 4>    light_position;
      f32               tess_alpha;
      f32               tess_strength;
      f32               tess_level;
};

struct Resources
{
      [[csl::uniform_buffer(0, 0)]] const Ubo*                       ubo;
      [[csl::binding(0, 1)]] CombinedSampler<f32, ImageType::IMAGE_2D> displacement_map;
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
      [[csl::location(2)]] vector<f32, 3> eye_position;
      [[csl::location(3)]] vector<f32, 3> light_vector;
};

[[csl::tess_eval(TessPrimitive::Triangles, TessSpacing::Equal, TessWinding::Cw)]] EvalOutput tess_eval_main()
{
      const Resources      resources_ = resources<Resources>();
      const vector<f32, 3> coord      = tess_coord();

      const EvalInput v0 = stage_input<EvalInput>(0u);
      const EvalInput v1 = stage_input<EvalInput>(1u);
      const EvalInput v2 = stage_input<EvalInput>(2u);

      EvalOutput           output;
      const vector<f32, 4> interpolated = coord.x * v0.position + coord.y * v1.position + coord.z * v2.position;
      output.uv                         = coord.x * v0.uv + coord.y * v1.uv + coord.z * v2.uv;
      output.normal                     = coord.x * v0.normal + coord.y * v1.normal + coord.z * v2.normal;

      const f32            displacement = max(resources_.displacement_map.sample_lod(output.uv, 0.0f).w, 0.0f) * resources_.ubo->tess_strength;
      const vector<f32, 3> displaced    = interpolated.xyz + normalize(output.normal) * displacement;
      const vector<f32, 4> position     = vector<f32, 4>{ displaced.x, displaced.y, displaced.z, interpolated.w };

      output.eye_position  = displaced;
      output.light_vector  = normalize(resources_.ubo->light_position.xyz - output.eye_position);
      output.position      = resources_.ubo->projection * resources_.ubo->modelview * position;
      return output;
}
