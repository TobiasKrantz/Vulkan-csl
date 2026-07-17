#include <csl/csl.h>

using namespace csl;

struct Params
{
      matrix<f32, 4, 4> dummy;
      i32               ssao;
      i32               ssao_only;
      i32               ssao_blur;
};

struct Resources
{
      [[csl::binding(0, 0)]] CombinedSampler<f32, ImageType::IMAGE_2D> position;
      [[csl::binding(0, 1)]] CombinedSampler<f32, ImageType::IMAGE_2D> normal;
      [[csl::binding(0, 2)]] CombinedSampler<f32, ImageType::IMAGE_2D> albedo;
      [[csl::binding(0, 3)]] CombinedSampler<f32, ImageType::IMAGE_2D> ssao;
      [[csl::binding(0, 4)]] CombinedSampler<f32, ImageType::IMAGE_2D> ssao_blur;
      [[csl::uniform_buffer(0, 5)]] const Params*                      params;
};

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 2> uv;
};

struct FragmentOutput
{
      [[csl::location(0)]] vector<f32, 4> color;
};

[[csl::fragment]] FragmentOutput fragment_main()
{
      const Resources resource = resources<Resources>();
      FragmentInput   input    = stage_input<FragmentInput>();
      FragmentOutput  output;
      output.color = vector<f32, 4>{ 0.0f, 0.0f, 0.0f, 1.0f };

      const vector<f32, 3> frag_position = resource.position.sample(input.uv).rgb;
      const vector<f32, 3> normal        = normalize(resource.normal.sample(input.uv).rgb * 2.0f - 1.0f);
      const vector<f32, 4> albedo        = resource.albedo.sample(input.uv);

      const f32 ssao = (resource.params->ssao_blur == 1) ? resource.ssao_blur.sample(input.uv).r : resource.ssao.sample(input.uv).r;

      const vector<f32, 3> light_position = vector<f32, 3>{ 0.0f, 0.0f, 0.0f };
      const vector<f32, 3> light          = normalize(light_position - frag_position);
      const f32            n_dot_l        = max(0.5f, dot(normal, light));

      if (resource.params->ssao_only == 1)
      {
            output.color.xyz = vector<f32, 3>{ ssao, ssao, ssao };
            return output;
      }

      const vector<f32, 3> base_color = albedo.rgb * n_dot_l;

      if (resource.params->ssao == 1)
      {
            output.color.xyz = vector<f32, 3>{ ssao, ssao, ssao };
            if (resource.params->ssao_only != 1)
                  output.color.xyz = output.color.xyz * base_color;
      }
      else
      {
            output.color.xyz = base_color;
      }
      return output;
}
