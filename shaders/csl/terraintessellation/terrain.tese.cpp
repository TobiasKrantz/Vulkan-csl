#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> modelview;
      vector<f32, 4>    light_position;
      vector<f32, 4>    frustum_planes[6];
      f32               displacement_factor;
      f32               tessellation_factor;
      vector<f32, 2>    viewport_dimensions;
      f32               tessellated_edge_size;
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
      [[csl::location(2)]] vector<f32, 3> view_vector;
      [[csl::location(3)]] vector<f32, 3> light_vector;
      [[csl::location(4)]] vector<f32, 3> eye_position;
      [[csl::location(5)]] vector<f32, 3> world_position;
};

[[csl::tess_eval(TessPrimitive::Quads, TessSpacing::Equal, TessWinding::Cw)]] EvalOutput tess_eval_main()
{
      const Resources      resources_ = resources<Resources>();
      const vector<f32, 3> coord      = tess_coord();

      const EvalInput v0 = stage_input<EvalInput>(0u);
      const EvalInput v1 = stage_input<EvalInput>(1u);
      const EvalInput v2 = stage_input<EvalInput>(2u);
      const EvalInput v3 = stage_input<EvalInput>(3u);

      EvalOutput output;

      const vector<f32, 2> uv1 = lerp(v0.uv, v1.uv, coord.x);
      const vector<f32, 2> uv2 = lerp(v3.uv, v2.uv, coord.x);
      output.uv                = lerp(uv1, uv2, coord.y);

      const vector<f32, 3> normal1 = lerp(v0.normal, v1.normal, coord.x);
      const vector<f32, 3> normal2 = lerp(v3.normal, v2.normal, coord.x);
      output.normal                = lerp(normal1, normal2, coord.y);

      const vector<f32, 4> position1 = lerp(v0.position, v1.position, coord.x);
      const vector<f32, 4> position2 = lerp(v3.position, v2.position, coord.x);
      const vector<f32, 4> patch     = lerp(position1, position2, coord.y);

      const f32            height    = resources_.displacement_map.sample_lod(output.uv, 0.0f).x * resources_.ubo->displacement_factor;
      const vector<f32, 4> displaced = vector<f32, 4>{ patch.x, patch.y - height, patch.z, patch.w };

      output.position       = resources_.ubo->projection * resources_.ubo->modelview * displaced;
      output.view_vector    = vector<f32, 3>{ 0.0f, 0.0f, 0.0f } - displaced.xyz;
      output.light_vector   = normalize(resources_.ubo->light_position.xyz + output.view_vector);
      output.world_position = displaced.xyz;
      output.eye_position   = (resources_.ubo->modelview * displaced).xyz;
      return output;
}
