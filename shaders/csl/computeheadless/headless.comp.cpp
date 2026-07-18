#include <csl/csl.h>

using namespace csl;

struct Positions
{
      [[csl::binding(0, 0)]] u32* values;
};

static u32 fibonacci(u32 n)
{
      if (n <= 1)
            return n;

      u32 current  = 1;
      u32 previous = 1;
      for (u32 i = 2; i < n; ++i)
      {
            u32 temp = current;
            current += previous;
            previous = temp;
      }
      return current;
}

[[csl::compute(1, 1, 1)]] void compute_main()
{
      [[csl::constant(0)]] const u32 BUFFER_ELEMENTS = 32;

      const Positions positions = resources<Positions>();

      u32 index = dispatch_id().x;
      if (index >= BUFFER_ELEMENTS)
            return;

      positions.values[index] = fibonacci(positions.values[index]);
}
