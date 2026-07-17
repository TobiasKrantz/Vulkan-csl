#include <csl/csl.h>

using namespace csl;

#define PI 3.1415926535897932384626433832795f

struct PushConstants
{
      matrix<f32, 4, 4> mvp;
      f32               delta_phi;
      f32               delta_theta;
};

struct Resources
{
      [[csl::binding(0, 0)]] CombinedSampler<f32, ImageType::IMAGE_CUBE_MAP> environment;
};

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 3> position;
};

struct FragmentOutput
{
      [[csl::location(0)]] vector<f32, 4> color;
};

[[csl::fragment]] FragmentOutput fragment_main(PushConstants push)
{
      const Resources res   = resources<Resources>();
      FragmentInput   input = stage_input<FragmentInput>();
      FragmentOutput  output;

      const vector<f32, 3> n     = normalize(input.position);
      const vector<f32, 3> right = normalize(cross(vector<f32, 3>{ 0.0f, 1.0f, 0.0f }, n));
      const vector<f32, 3> up    = cross(n, right);

      const f32 two_pi  = PI * 2.0f;
      const f32 half_pi = PI * 0.5f;

      vector<f32, 3> color        = vector<f32, 3>{ 0.0f, 0.0f, 0.0f };
      u32            sample_count = 0u;
      for (f32 phi = 0.0f; phi < two_pi; phi += push.delta_phi)
      {
            for (f32 theta = 0.0f; theta < half_pi; theta += push.delta_theta)
            {
                  const vector<f32, 3> temp_vector   = cos(phi) * right + sin(phi) * up;
                  const vector<f32, 3> sample_vector = cos(theta) * n + sin(theta) * temp_vector;
                  color                              = color + res.environment.sample(sample_vector).xyz * cos(theta) * sin(theta);
                  sample_count++;
            }
      }

      const vector<f32, 3> irradiance = PI * color / f32(sample_count);
      output.color                    = vector<f32, 4>{ irradiance.x, irradiance.y, irradiance.z, 1.0f };
      return output;
}
