#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> mvp[3];
      vector<f32, 4>    instance_position[3];
};

struct Resources
{
      [[csl::uniform_buffer(0, 0)]] const Ubo* ubo;
};

struct VertexInput
{
      [[csl::position]] vector<f32, 4> position;
      [[csl::location(0)]] i32         instance_index;
};

struct VertexOutput
{
      [[csl::position]] vector<f32, 4> position;
      [[csl::layer]] i32               layer;
};

[[csl::geometry(InputPrimitive::Triangles, OutputPrimitive::TriangleStrip, 3, 3)]] void geometry_main()
{
      const Resources resources_ = resources<Resources>();

      const VertexInput    first              = stage_input<VertexInput>(0u);
      const vector<f32, 4> instanced_position = resources_.ubo->instance_position[first.instance_index];

      const i32 invocation = invocation_id();
      for (i32 i = 0; i < 3; i++)
      {
            const VertexInput input = stage_input<VertexInput>(u32(i));

            VertexOutput         output;
            const vector<f32, 4> local_position = input.position + instanced_position;
            output.position                     = resources_.ubo->mvp[invocation] * local_position;
            output.layer                        = invocation;
            emit_vertex(output);
      }
      end_primitive();
}
