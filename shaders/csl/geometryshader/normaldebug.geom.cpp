#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      matrix<f32, 4, 4> projection;
      matrix<f32, 4, 4> model;
};

struct Resources
{
      [[csl::uniform_buffer(0, 0)]] const Ubo* ubo;
};

struct VertexInput
{
      [[csl::position]] vector<f32, 4>    position;
      [[csl::location(0)]] vector<f32, 3> normal;
};

struct VertexOutput
{
      [[csl::position]] vector<f32, 4>    position;
      [[csl::location(0)]] vector<f32, 3> color;
};

[[csl::geometry(InputPrimitive::Triangles, OutputPrimitive::LineStrip, 6)]] void geometry_main()
{
      const Resources resources_ = resources<Resources>();

      const f32 normal_length = 0.02f;
      for (i32 i = 0; i < 3; i++)
      {
            const VertexInput    input  = stage_input<VertexInput>(u32(i));
            const vector<f32, 3> pos    = input.position.xyz;
            const vector<f32, 3> normal = input.normal;

            VertexOutput base;
            base.position = resources_.ubo->projection * (resources_.ubo->model * vector<f32, 4>{ pos.x, pos.y, pos.z, 1.0f });
            base.color    = vector<f32, 3>{ 1.0f, 0.0f, 0.0f };
            emit_vertex(base);

            const vector<f32, 3> tip = pos + normal * normal_length;
            VertexOutput         end;
            end.position = resources_.ubo->projection * (resources_.ubo->model * vector<f32, 4>{ tip.x, tip.y, tip.z, 1.0f });
            end.color    = vector<f32, 3>{ 0.0f, 0.0f, 1.0f };
            emit_vertex(end);

            end_primitive();
      }
}
