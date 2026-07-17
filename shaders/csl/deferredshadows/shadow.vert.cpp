#include <csl/csl.h>

using namespace csl;

struct VertexInput
{
      [[csl::location(0)]] vector<f32, 4> position;
};

struct VertexOutput
{
      [[csl::position]] vector<f32, 4> position;
      [[csl::location(0)]] i32         instance_index;
};

[[csl::vertex]] VertexOutput vertex_main()
{
      VertexInput  input = stage_input<VertexInput>();
      VertexOutput output;

      output.instance_index = i32(instance_index());
      output.position       = input.position;
      return output;
}
