#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> projection[2];
      matrix<f32, 4, 4> modelview[2];
      vector<f32, 4>    light_position;
};

struct Resources
{
      [[csl::uniform_buffer(0, 0)]] const Ubo* ubo;
};

struct VertexInput
{
      [[csl::position]] vector<f32, 4>    position;
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 3> color;
};

struct VertexOutput
{
      [[csl::position]] vector<f32, 4>    position;
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 3> color;
      [[csl::location(2)]] vector<f32, 3> view_vector;
      [[csl::location(3)]] vector<f32, 3> light_vector;
      [[csl::viewport_index]] i32         viewport;
      [[csl::primitive_id]] i32           primitive;
};

[[csl::geometry(InputPrimitive::Triangles, OutputPrimitive::TriangleStrip, 3, 2)]] void geometry_main()
{
      const Resources resources_ = resources<Resources>();

      const i32 invocation = invocation_id();
      for (i32 i = 0; i < 3; i++)
      {
            const VertexInput input = stage_input<VertexInput>(u32(i));

            VertexOutput output;
            output.normal = mat3(resources_.ubo->modelview[invocation]) * input.normal;
            output.color  = input.color;

            const vector<f32, 4> world_position = resources_.ubo->modelview[invocation] * input.position;
            const vector<f32, 3> light_position = (resources_.ubo->modelview[invocation] * resources_.ubo->light_position).xyz;

            output.light_vector = light_position - world_position.xyz;
            output.view_vector  = vector<f32, 3>{ 0.0f, 0.0f, 0.0f } - world_position.xyz;
            output.position     = resources_.ubo->projection[invocation] * world_position;
            output.viewport     = invocation;
            output.primitive    = primitive_id();
            emit_vertex(output);
      }
      end_primitive();
}
