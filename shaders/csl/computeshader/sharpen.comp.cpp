#include <csl/csl.h>

using namespace csl;

struct Images
{
      [[csl::binding(0, 0)]] StorageImage<f32, ImageType::IMAGE_2D, false, ImageFormat::Rgba8Unorm> input;
      [[csl::binding(0, 1)]] StorageImage<f32, ImageType::IMAGE_2D, false, ImageFormat::Rgba8Unorm> result;
};

struct Kernel
{
      f32 weight[9];
};

struct Channels
{
      Kernel red;
      Kernel green;
      Kernel blue;
};

static f32 convolve(Kernel kernel, Kernel data, f32 denominator, f32 offset)
{
      f32 sum = 0.0f;
      for (i32 i = 0; i < 9; ++i)
            sum += kernel.weight[i] * data.weight[i];
      return clamp(sum / denominator + offset, 0.0f, 1.0f);
}

[[csl::compute(16, 16, 1)]] void compute_main()
{
      const Images   images = resources<Images>();
      vector<u32, 3> id     = dispatch_id();

      Channels channels;
      i32      n = -1;
      for (i32 i = -1; i < 2; ++i)
            for (i32 j = -1; j < 2; ++j)
            {
                  n += 1;
                  vector<f32, 4> rgb      = images.input.load(vector<i32, 2>{ i32(id.x) + i, i32(id.y) + j });
                  channels.red.weight[n]   = rgb.x;
                  channels.green.weight[n] = rgb.y;
                  channels.blue.weight[n]  = rgb.z;
            }

      Kernel kernel;
      kernel.weight[0] = -1.0f;
      kernel.weight[1] = -1.0f;
      kernel.weight[2] = -1.0f;
      kernel.weight[3] = -1.0f;
      kernel.weight[4] = 9.0f;
      kernel.weight[5] = -1.0f;
      kernel.weight[6] = -1.0f;
      kernel.weight[7] = -1.0f;
      kernel.weight[8] = -1.0f;

      vector<f32, 4> color = vector<f32, 4>{ convolve(kernel, channels.red, 1.0f, 0.0f), convolve(kernel, channels.green, 1.0f, 0.0f), convolve(kernel, channels.blue, 1.0f, 0.0f), 1.0f };
      images.result.store(vector<i32, 2>{ i32(id.x), i32(id.y) }, color);
}
