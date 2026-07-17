#include <csl/csl.h>

using namespace csl;

struct Ubo
{
      f32 height_scale;
      f32 parallax_bias;
      f32 num_layers;
      i32 mapping_mode;
};

struct Resources
{
      [[csl::binding(0, 1)]] CombinedSampler<f32, ImageType::IMAGE_2D> color_map;
      [[csl::binding(0, 2)]] CombinedSampler<f32, ImageType::IMAGE_2D> normal_height_map;
      [[csl::uniform_buffer(0, 3)]] const Ubo*                         ubo;
};

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 2> uv;
      [[csl::location(1)]] vector<f32, 3> tangent_light_position;
      [[csl::location(2)]] vector<f32, 3> tangent_view_position;
      [[csl::location(3)]] vector<f32, 3> tangent_frag_position;
};

struct FragmentOutput
{
      [[csl::location(0)]] vector<f32, 4> color;
};

[[csl::fragment]] FragmentOutput fragment_main()
{
      const Resources res   = resources<Resources>();
      FragmentInput   input = stage_input<FragmentInput>();
      FragmentOutput  output;

      const i32 mapping_mode = res.ubo->mapping_mode;
      if (mapping_mode == 0)
      {
            output.color = res.color_map.sample(input.uv);
            return output;
      }

      const vector<f32, 3> v = normalize(input.tangent_view_position - input.tangent_frag_position);

      const f32 height_scale  = res.ubo->height_scale;
      const f32 parallax_bias = res.ubo->parallax_bias;
      const f32 num_layers    = res.ubo->num_layers;

      vector<f32, 2> uv = input.uv;

      if (mapping_mode == 2)
      {
            const f32            height = 1.0f - res.normal_height_map.sample_lod(input.uv, 0.0f).w;
            const vector<f32, 2> offset = v.xy * (height * (height_scale * 0.5f) + parallax_bias) / v.z;
            uv                          = input.uv - offset;
      }
      else if (mapping_mode == 3 || mapping_mode == 4)
      {
            const f32            layer_depth = 1.0f / num_layers;
            const vector<f32, 2> delta_uv    = v.xy * height_scale / (v.z * num_layers);

            f32            current_layer_depth = 0.0f;
            vector<f32, 2> current_uv          = input.uv;
            f32            height              = 1.0f - res.normal_height_map.sample_lod(current_uv, 0.0f).w;

            for (i32 i = 0; f32(i) < num_layers; ++i)
            {
                  current_layer_depth += layer_depth;
                  current_uv = current_uv - delta_uv;
                  height     = 1.0f - res.normal_height_map.sample_lod(current_uv, 0.0f).w;
                  if (height < current_layer_depth)
                        break;
            }
            uv = current_uv;

            if (mapping_mode == 4)
            {
                  const vector<f32, 2> previous_uv    = current_uv + delta_uv;
                  const f32            next_depth      = height - current_layer_depth;
                  const f32            previous_depth  = 1.0f - res.normal_height_map.sample_lod(previous_uv, 0.0f).w - current_layer_depth + layer_depth;
                  const f32            weight          = next_depth / (next_depth - previous_depth);
                  uv                                   = lerp(current_uv, previous_uv, vector<f32, 2>{ weight, weight });
            }
      }

      const vector<f32, 3> normal_height = res.normal_height_map.sample_lod(uv, 0.0f).xyz;
      const vector<f32, 3> surface_color = res.color_map.sample(uv).xyz;

      if (uv.x < 0.0f || uv.x > 1.0f || uv.y < 0.0f || uv.y > 1.0f)
            discard();

      const vector<f32, 3> n = normalize(normal_height * 2.0f - 1.0f);
      const vector<f32, 3> l = normalize(input.tangent_light_position - input.tangent_frag_position);
      const vector<f32, 3> h = normalize(l + v);

      const vector<f32, 3> ambient  = 0.2f * surface_color;
      const vector<f32, 3> diffuse  = max(dot(l, n), 0.0f) * surface_color;
      const vector<f32, 3> specular = vector<f32, 3>{ 0.15f, 0.15f, 0.15f } * pow(max(dot(n, h), 0.0f), 32.0f);

      const vector<f32, 3> lit = ambient + diffuse + specular;
      output.color             = vector<f32, 4>{ lit.x, lit.y, lit.z, 1.0f };
      return output;
}
