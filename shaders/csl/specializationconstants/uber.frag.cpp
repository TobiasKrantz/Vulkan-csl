#include <csl/csl.h>

using namespace csl;

struct Resources
{
      [[csl::binding(0, 1)]] CombinedSampler<f32, ImageType::IMAGE_2D> color_map;
      [[csl::binding(0, 2)]] CombinedSampler<f32, ImageType::IMAGE_2D> discard_map;
};

struct FragmentInput
{
      [[csl::location(0)]] vector<f32, 3> normal;
      [[csl::location(1)]] vector<f32, 3> color;
      [[csl::location(2)]] vector<f32, 2> uv;
      [[csl::location(3)]] vector<f32, 3> view_vector;
      [[csl::location(4)]] vector<f32, 3> light_vector;
};

struct FragmentOutput
{
      [[csl::location(0)]] vector<f32, 4> color;
};

[[csl::fragment]] FragmentOutput fragment_main()
{
      [[csl::constant(0)]] const i32 lighting_model    = 0;
      [[csl::constant(1)]] const f32 toon_desaturation = 0.0f;

      const Resources resource = resources<Resources>();
      FragmentInput   input    = stage_input<FragmentInput>();
      FragmentOutput  output;
      output.color = vector<f32, 4>{ 0.0f, 0.0f, 0.0f, 1.0f };

      switch (lighting_model)
      {
      case 0:
      {
            const vector<f32, 3> ambient  = input.color * vector<f32, 3>{ 0.25f, 0.25f, 0.25f };
            const vector<f32, 3> normal   = normalize(input.normal);
            const vector<f32, 3> light    = normalize(input.light_vector);
            const vector<f32, 3> view     = normalize(input.view_vector);
            const vector<f32, 3> mirror   = reflect(-light, normal);
            const vector<f32, 3> diffuse  = max(dot(normal, light), 0.0f) * input.color;
            const vector<f32, 3> specular = pow(max(dot(mirror, view), 0.0f), 32.0f) * vector<f32, 3>{ 0.75f, 0.75f, 0.75f };
            const vector<f32, 3> lit      = ambient + diffuse * 1.75f + specular;
            output.color                  = vector<f32, 4>{ lit.x, lit.y, lit.z, 1.0f };
            break;
      }
      case 1:
      {
            const vector<f32, 3> normal    = normalize(input.normal);
            const vector<f32, 3> light     = normalize(input.light_vector);
            const f32            intensity = dot(normal, light);
            vector<f32, 3>       shade;
            if (intensity > 0.98f)
                  shade = input.color * 1.5f;
            else if (intensity > 0.9f)
                  shade = input.color * 1.0f;
            else if (intensity > 0.5f)
                  shade = input.color * 0.6f;
            else if (intensity > 0.25f)
                  shade = input.color * 0.4f;
            else
                  shade = input.color * 0.2f;
            const f32 grey   = dot(vector<f32, 3>{ 0.2126f, 0.7152f, 0.0722f }, shade);
            shade            = lerp(shade, vector<f32, 3>{ grey, grey, grey }, vector<f32, 3>{ toon_desaturation, toon_desaturation, toon_desaturation });
            output.color.xyz = shade;
            break;
      }
      case 2:
      {
            const vector<f32, 4> sampled  = resource.color_map.sample(input.uv);
            const vector<f32, 4> texel    = vector<f32, 4>{ sampled.r, sampled.r, sampled.r, sampled.a };
            const vector<f32, 3> ambient  = texel.rgb * vector<f32, 3>{ 0.25f, 0.25f, 0.25f } * input.color;
            const vector<f32, 3> normal   = normalize(input.normal);
            const vector<f32, 3> light    = normalize(input.light_vector);
            const vector<f32, 3> view     = normalize(input.view_vector);
            const vector<f32, 3> mirror   = reflect(-light, normal);
            const vector<f32, 3> diffuse  = max(dot(normal, light), 0.0f) * texel.rgb;
            const f32            specular = pow(max(dot(mirror, view), 0.0f), 32.0f) * texel.a;
            const vector<f32, 3> lit      = ambient + diffuse + vector<f32, 3>{ specular, specular, specular };
            output.color                  = vector<f32, 4>{ lit.x, lit.y, lit.z, 1.0f };
            break;
      }
      }
      return output;
}
