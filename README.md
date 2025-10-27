# Lab4_Vectors
Optimizing the Sobel filter using SIMD vector instructions to speed up code.

## Assignment
Your next step in optimizing the Sobel filter is using SIMD vector instructions to speed up our code even more. The ARM cores on the Raspberry PI  feature NEON vector/SIMD engines. Since the Raspberry Pi 4 uses Cortex-A72 CPUs based on the Armv8-A architecture, each core supports Neon for Armv8-AL. Each of these Neon engines supports up to 128 bits of data per operand, and can be configured to complete 16 8-bit operations, 8 16-bit operations, 4 32-bit operations, or 2 64-bit operations simultaneously. Use of SIMD can drastically speed up the operation of so-called "embarrassingly parallel" operations on low-precision data, such as image processing. The compiler, however, will not always automatically make use of these instructions, so we have to manually tell the compiler where and how to use these.

While there are many ways to ensure Neon vector instructions are used, the mechanisms that give the programmer the most control over how the NEON engine operates are inline assembly commands and compiler intrinsics. Inline assembly allows the programmer to specify exactly how operations should be vectorized, how data should be passed into the vector engine, etc. Intrinsics exist as C-level hints telling the compiler when and where to vectorize.

For this assignment, I am requiring you to optimize your multi-threaded Sobel implementation to use either inline assembly or Neon intrinsics to vectorize both the Sobel and grayscale functions.

If you have time, please install perfmon and libpfm and start experimenting with pulling performance data from hardware counters (things like cache misses, time to run, etc.).

Some sample documentation: https://github.com/thenifty/neon-guideLinks to an external site.
 

## Deliverables
C and H (or cpp equivalent) source files for your code. A Demo of your Sobel code working. A tutorial or instructable designed to walk a novice through either Neon instrinsics and inline assembly, and what extra compiler flags you need to include (if any) to allow vectorization to occur.