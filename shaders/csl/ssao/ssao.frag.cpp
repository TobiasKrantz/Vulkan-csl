#include <csl/csl.h>

using namespace csl;

constexpr u32 SSAO_KERNEL_ARRAY = 64;

struct SsaoKernel
{
      vector<f32, 4> samples[SSAO_KERNEL_ARRAY];
};

struct Ubo
{
      matrix<f32, 4, 4> projection;
};

struct Resources
{
      [[csl::binding(0, 0)]] CombinedSampler<f32, ImageType::IMAGE_2D> position_depth;
      [[csl::binding(0, 1)]] CombinedSampler<f32, ImageType::IMAGE_2D> normal;
      [[csl::binding(0, 2)]] CombinedSampler<f32, ImageType::IMAGE_2D> noise;
      [[csl::uniform_buffer(0, 3)]] const SsaoKernel*                  kernel;
      [[csl::uniform_buffer(0, 4)]] const Ubo*                         ubo;
};

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 2> uv;
};

struct FragmentOutput
{
      [[csl::location(0)]] f32 occlusion;
};

[[csl::fragment]] FragmentOutput fragment_main()
{
      [[csl::constant(0)]] const i32 SSAO_KERNEL_SIZE = 64;
      [[csl::constant(1)]] const f32 SSAO_RADIUS      = 0.5f;

      const Resources resource = resources<Resources>();
      FragmentInput   input    = stage_input<FragmentInput>();
      FragmentOutput  output;

      const vector<f32, 3> frag_position = resource.position_depth.sample(input.uv).rgb;
      const vector<f32, 3> normal        = normalize(resource.normal.sample(input.uv).rgb * 2.0f - 1.0f);

      const vector<u32, 2> texture_dimensions = resource.position_depth.size(0);
      const vector<u32, 2> noise_dimensions   = resource.noise.size(0);
      const vector<f32, 2> noise_scale        = vector<f32, 2>{ f32(texture_dimensions.x) / f32(noise_dimensions.x), f32(texture_dimensions.y) / f32(noise_dimensions.y) };
      const vector<f32, 2> noise_uv           = noise_scale * input.uv;
      const vector<f32, 3> random_vector      = resource.noise.sample(noise_uv).xyz * 2.0f - 1.0f;

      const vector<f32, 3>    tangent   = normalize(random_vector - normal * dot(random_vector, normal));
      const vector<f32, 3>    bitangent = cross(tangent, normal);
      const matrix<f32, 3, 3> tbn       = mat3(tangent, bitangent, normal);

      f32           occlusion = 0.0f;
      constexpr f32 bias      = 0.025f;
      for (i32 i = 0; i < SSAO_KERNEL_SIZE; ++i)
      {
            vector<f32, 3> sample_position = tbn * resource.kernel->samples[i].xyz;
            sample_position                = frag_position + sample_position * SSAO_RADIUS;

            vector<f32, 4> offset = resource.ubo->projection * vector<f32, 4>{ sample_position.x, sample_position.y, sample_position.z, 1.0f };
            vector<f32, 3> screen = offset.xyz / offset.w;
            screen                = screen * 0.5f + 0.5f;

            const f32 sample_depth = -resource.position_depth.sample(screen.xy).w;

            const f32 range_check = smooth_step(0.0f, 1.0f, SSAO_RADIUS / abs(frag_position.z - sample_depth));
            occlusion += (sample_depth >= sample_position.z + bias ? 1.0f : 0.0f) * range_check;
      }

      output.occlusion = 1.0f - (occlusion / f32(SSAO_KERNEL_SIZE));
      return output;
}
