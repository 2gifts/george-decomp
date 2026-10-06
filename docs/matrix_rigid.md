# Scalar rigid inverse

`func_002A1098` recovers one complete 268-byte routine using the established
64-byte matrix layout. It transposes the three basis vectors and writes the
negative ordered products of the translation with each input basis. The
homogeneous components become zero, zero, zero and one.

The original loads and stores overlap in time. Each transpose store retains
live input memory; the third component is captured before the preceding zero
store. Each translation row reloads its input basis and translation after the
previous output row. All operands of the final row are captured before the
homogeneous-one store. These details preserve shifted input/output aliases,
including an identical input and output pointer. An in-place call follows the
observed sequential behavior and need not produce the mathematical inverse of
an untouched input. The usual geometric interpretation assumes an orthonormal
basis; the source retains the original operations for other finite inputs too.

Root and an independent agent reviewed all 67 original instructions, the full
source/header, tracer and native harness. The whole original SHA256 is
`57d32e4be519e5d76c3d41cf749e2b7df1393f463fd9385e935fc67307d9e1b1`.
All 34 encoded direct callers and their complete containing bodies were
verified. The terminal store in the return delay belongs to the routine;
four zero alignment bytes at `002A11A4` are excluded. There are no engine calls
or local branches. Three genuine full links reproduce 256-byte candidates
with 151, 190 and 190 differing bytes and zero unresolved symbols. The function
remains reconstructed and earns no exact-byte credit.

The scoped tracer reuses the existing strict scalar decoder unchanged. It
regenerates 512 synthetic finite dyadic fixtures, including all four-byte
overlap shifts from -60 through +60 and disjoint output. Every fixture executes
all 67 original instructions; the total is 34,304. Only authored buffers are
exported. The input SHA256 is
`2bccbc41a3ea40bc85fd861dee4ab6c2aa5a1b7c9f574c9fd30c741f27d51422`;
the header's canonical LF SHA256 is
`b486b07a8ce3071323d54d0b7a0c39671cf16f2439d5b8027289db64aca1e77a`.

Regenerate fixtures and run the asset-free checks:

```powershell
.venv/Scripts/python.exe tools/trace_matrix_rigid.py --golden-header tests/native/matrix_rigid_golden.h
.venv/Scripts/python.exe tests/native/run_utilities.py --harness matrix_rigid
```

The native harness passes 32,769 exact buffer-word checks with the reviewed
finite inputs. Three guard tests reject other scopes, uninitialized memory,
reserved operands, nonfinite arithmetic and unexpected transfers or delay
slots. The host IEEE model does not certify EE exceptional values, FCR flags,
timing or full gameplay behavior. Caller storage must be initialized and large
enough for both matrix extents, as required by the original.
