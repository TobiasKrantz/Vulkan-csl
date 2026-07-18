#include <csl/csl.h>

using namespace csl;

struct ModelData
{
      vector<f32, 4> pos;
      vector<f32, 4> color;
};

struct Resources
{
      [[csl::descriptor_heap]] const ModelData* model_data;
};

struct MatrixReference
{
      matrix<f32, 4, 4> mvp;
      u32               sampler_index;
      u32               image_heap_index_offset;
};

struct PushConstants
{
      const MatrixReference* matrix_reference;
};

struct VertexInput
{
      [[csl::location(0)]] vector<f32, 3> position;
      [[csl::location(1)]] vector<f32, 3> normal;
      [[csl::location(2)]] vector<f32, 2> uv;
      [[csl::location(3)]] vector<f32, 3> color;
};

struct VertexOutput
{
      [[csl::position]] vector<f32, 4>    clip;
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 3> color;
      [[csl::location(2)]] vector<f32, 2> uv;
      [[csl::location(3)]] i32            instance_index;
};

[[csl::vertex]] VertexOutput vertex_main(PushConstants push)
{
      const Resources resources_ = resources<Resources>();
      VertexInput     input      = stage_input<VertexInput>();
      VertexOutput    output;

      output.normal = input.normal;
      output.uv     = input.uv;

      const MatrixReference* uniform_data = push.matrix_reference;

      const vector<f32, 3> local = input.position * 0.25f + resources_.model_data[nonuniform(instance_index())].pos.xyz;
      output.clip                = uniform_data->mvp * vector<f32, 4>{ local.x, local.y, local.z, 1.0f };
      output.color               = resources_.model_data[nonuniform(instance_index())].color.xyz;
      output.instance_index      = i32(instance_index());
      return output;
}
