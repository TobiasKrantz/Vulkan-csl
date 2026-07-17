# csl shaders

SPIR-V for the samples, authored in csl. A csl shader is an ordinary C++
translation unit that includes `<csl/csl.h>`; stage entry points are plain
functions carrying a `[[csl::vertex]]`, `[[csl::fragment]]`, or
`[[csl::compute]]` attribute. Types, resources, and stage interfaces are
expressed with the csl authoring surface rather than GLSL/HLSL syntax.

One file holds one stage entry point. Source files mirror the glsl/hlsl naming
(`triangle.vert.cpp`, `triangle.frag.cpp`), and each compiles to a single raw
SPIR-V module (`triangle.vert.spv`, `triangle.frag.spv`).

## Entry point names

C++ reserves `main` at global scope, so entry functions use descriptive names
(`vertex_main`, `fragment_main`, `compute_main`). The SPIR-V `OpEntryPoint`
name is the function name; the csl reflection header carries it. A loader that
hardcodes `"main"` therefore needs the reflected name, unlike the glsl/hlsl
modules.

## Compiling

```
python3 compileshaders.py --csl <path/to/csl> --include <path/to/csl/include>
```

`--csl` defaults to `csl` on `PATH`; `--include` defaults to `$CSL_INCLUDE`.
`--sample <name>` restricts to one directory. Each file is compiled with
`csl --emit spirv`.

## Verification

Every module passes `spirv-val --target-env vulkan1.3`. The pair is
additionally loaded into a real Vulkan pipeline on a software ICD (lavapipe)
with the validation layer enabled, an independent check that a driver accepts
and compiles the SPIR-V.
