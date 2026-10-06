`limits.h` is an unchanged copy of `ee/gcc/glimits.h` from the public GNU EE
toolchain at commit `b595ded606227e93b8c4a447446c1d2ac093827d`:

https://github.com/SSXModding/ps2-ee-toolchain/blob/b595ded606227e93b8c4a447446c1d2ac093827d/ee/gcc/glimits.h

It supplies the compiler's integer limits header for runtime recipes that
explicitly include this directory. GNU GCC's Makefile normally generates the
installed `limits.h` from this file, but the optional local compiler package
does not yet install that generated header. The GNU source license is retained
in `LICENSES/GPL-2.0.txt`. No game source recipe includes this directory by
default, and the header does not modify the compiler backend.
