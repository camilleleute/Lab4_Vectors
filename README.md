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




## MY NOTES:
- Vector registers are registers that hold entire arrays at once instead of just one element at once
- Allows you to hold multiple values and then do the same operation on all of them (similar to the concept of gpu warps)
- NEON IS JUST SIMD BUT FOR ARM
- The NEON engine uses 128 bit registers 
- Since sobel filter goes through and multiples every value with the same exact kernel, this can be parallelized 
- Can process 16 pixels at once in the vector
***When you have the same operations on arrays of data, you can pack that data into vector registers and execute one instruction that does the same thing to all of them***

CURRENT APPROACH:
- First to do the greyscale, go through each RGB pixel one by one and do the same calculation on it to make it grey
- THEN
- Divide the big matrix into chunks, and find the bounds of that chunk
- Give that chunk and those bounds to each thread
- Find the 8 neighbor pixels
- Multiply each by Gx kernel, then sum that to get one Gx value
- Multiply each by Gy kernel, then sum that to get one Gy value
- Do the absolute value calculation and then clamp it
- Output one sobel pixel
- Write that to the output matrix
- Repeat for all pixels
- When all threads are done, move to the next thread

PARALLEIZED APPROACH: 8 pixels can be processed at once
- FOR RGB -> Greyscale
- Go put 8 pixels into the vector, run the math, write 8 outputs to the output  grayscaled matrix
- FOR ACTUAL SOBEL
- Load 8 uint8 pixels into the vector array(64 bits total)
- Widen this to 8 uint16 values (expands to 128 bits)
- Do all the math 
- Narrow it back down to 8 uint8 (64 bits) and store it in the final pixel


CONCEPTUALLY:
GOING TO DO THIS AS ONE MAIN FUNCTION
- If i kept the modular structure of the previous code, I would have to pass the 9 vectors in by reference each time, giving each function a lot of overhead. instead i can just make one large sobel function that handles everything
- I also would do this so that I can just have one loop going through the matrix, and I don't have to keep a pointer for where the last set of 8 pixels got processed was in the matrix. 
- Find the neighbors in all directions of all 8 target pixels
    - this returns 9 different vectors
    - then go through and scalar multiply all those vectors
    - then go through and do that and sum all the Gx and Gy's and clamp
    - then slap that into the output matrix


ok so conceptually, first to find the neighbors, i need to find the neighbors of 8 pixels at once. SO i have one vector of all the pixels, one vector of all the upper left neighbor, one vector of all the upper neighbor, one vector of all the upper right neighbor, one vector of all the left neighbor, one of all the right neighbors, one of all teh bottom neighbors, one for bottom left and bottom right? giving me 9 total vectors? then i take those, and i multiply each vector with the scalar in the convolution kernel at that location? then I sum that all together or what do I do with that?