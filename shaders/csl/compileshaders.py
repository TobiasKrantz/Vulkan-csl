# Compile all csl shaders to SPIR-V.
#
# csl shaders are ordinary C++ translation units that include <csl/csl.h>. Each
# file holds one stage-attributed entry point. The csl compiler emits one raw
# SPIR-V module per file with --emit spirv. Output name mirrors the glsl/hlsl
# convention: triangle.vert.cpp -> triangle.vert.spv.

import argparse
import os
import subprocess
import sys

parser = argparse.ArgumentParser(description='Compile all csl shaders')
parser.add_argument('--csl', type=str, help='path to the csl compiler executable')
parser.add_argument('--include', type=str, help='path to the csl include directory (containing csl/csl.h)')
parser.add_argument('--sample', type=str, help='compile shaders for a single sample only')
args = parser.parse_args()


def find_csl():
    if args.csl is not None and os.path.isfile(args.csl) and os.access(args.csl, os.X_OK):
        return args.csl
    for directory in os.environ['PATH'].split(os.pathsep):
        candidate = os.path.join(directory, 'csl')
        if os.path.isfile(candidate) and os.access(candidate, os.X_OK):
            return candidate
    sys.exit('Could not find the csl compiler; pass --csl <path>')


def find_include():
    if args.include is not None:
        return args.include
    if 'CSL_INCLUDE' in os.environ:
        return os.environ['CSL_INCLUDE']
    sys.exit('Could not find the csl include directory; pass --include <path>')


csl = find_csl()
include = find_include()
root = os.path.dirname(os.path.realpath(__file__)).replace('\\', '/')

for current, directories, files in os.walk(root):
    if args.sample is not None and os.path.basename(current) != args.sample:
        continue
    for name in files:
        if not name.endswith('.cpp'):
            continue
        source = os.path.join(current, name)
        output = source[:-len('.cpp')] + '.spv'
        command = [csl, '--emit', 'spirv', '-o', output, source, '--',
                   '-std=c++20', '-Wno-unknown-attributes', '-I' + include]
        if subprocess.call(command) != 0:
            sys.exit(1)
