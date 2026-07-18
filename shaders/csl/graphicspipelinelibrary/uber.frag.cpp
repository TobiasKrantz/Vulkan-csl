#include <csl/csl.h>

using namespace csl;

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 3> color;
      [[csl::location(2)]] vector<f32, 3> view_vector;
      [[csl::location(3)]] vector<f32, 3> light_vector;
      [[csl::location(4)]] vector<f32, 3> flat_normal;
};

struct FragmentOutput
{
      [[csl::location(0)]] vector<f32, 4> color;
};

[[csl::fragment]] FragmentOutput fragment_main()
{
      [[csl::constant(0)]] const i32 lighting_model = 0;

      FragmentInput  input = stage_input<FragmentInput>();
      FragmentOutput output;

      switch (lighting_model)
      {
      case 0:
      {
            const vector<f32, 3> ambient   = input.color * vector<f32, 3>{ 0.25f, 0.25f, 0.25f };
            const vector<f32, 3> normal    = normalize(input.normal);
            const vector<f32, 3> light     = normalize(input.light_vector);
            const vector<f32, 3> view      = normalize(input.view_vector);
            const vector<f32, 3> reflected = reflect(-light, normal);
            const vector<f32, 3> diffuse   = max(dot(normal, light), 0.0f) * input.color;
            const vector<f32, 3> specular  = pow(max(dot(reflected, view), 0.0f), 32.0f) * vector<f32, 3>{ 0.75f, 0.75f, 0.75f };
            const vector<f32, 3> lit       = ambient + diffuse * 1.75f + specular;
            output.color                   = vector<f32, 4>{ lit.x, lit.y, lit.z, 1.0f };
            break;
      }
      case 1:
      {
            const vector<f32, 3> normal    = normalize(input.normal);
            const vector<f32, 3> light     = normalize(input.light_vector);
            const f32            intensity = dot(normal, light);
            vector<f32, 3>       color;
            if (intensity > 0.98f)
                  color = input.color * 1.5f;
            else if (intensity > 0.9f)
                  color = input.color * 1.0f;
            else if (intensity > 0.5f)
                  color = input.color * 0.6f;
            else if (intensity > 0.25f)
                  color = input.color * 0.4f;
            else
                  color = input.color * 0.2f;
            output.color.xyz = color;
            break;
      }
      case 2:
      {
            output.color.xyz = input.color;
            break;
      }
      case 3:
      {
            const f32 grey   = dot(input.color, vector<f32, 3>{ 0.299f, 0.587f, 0.114f });
            output.color.xyz = vector<f32, 3>{ grey, grey, grey };
            break;
      }
      }

      output.color.xyz = output.color.xyz * 1.25f;
      return output;
}
