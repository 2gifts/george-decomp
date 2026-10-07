/* Complete genuine primary source executes here; wrappers are observer-only.
   Engine allocation/free are controlled events, not an allocator model. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

static void *libio_bigint_allocate(size_t);
static void libio_bigint_release(void *);
#define malloc libio_bigint_allocate
#define free libio_bigint_release
#include <floatconv.c>
#undef malloc
#undef free
#include "libio_bigint_golden.h"

typedef char pointer_size[(sizeof(void *) == 4) ? 1 : -1];
typedef char limb_size[(sizeof(unsigned32) == 4) ? 1 : -1];
typedef char header_size[(BIGINT_HEADER_SIZE == 20) ? 1 : -1];
typedef char sign_offset[(offsetof(Bigint, sign) == 14) ? 1 : -1];
typedef char on_stack_offset[(offsetof(Bigint, on_stack) == 12) ? 1 : -1];

static unsigned32 *arena;
static unsigned allocations, event_n, events[2][3], mutation;
static Bigint *last_allocation;
static unsigned long checks;
static unsigned fixture;

static unsigned initial_word(unsigned index, unsigned seed)
{
  return 0x6a31c29du ^ (index * 0x10203u) ^ (seed * 0x443u);
}

static unsigned offset(const void *pointer)
{
  unsigned p = (unsigned)pointer;
  unsigned start = (unsigned)arena;
  if (p < start || p >= start + LIBIO_BIGINT_WORDS * 4u) {
    fprintf(stderr, "fixture%u pointer outside observer arena\n", fixture);
    exit(1);
  }
  return p - start;
}

static void check_u32(unsigned actual, unsigned expected, const char *label, unsigned index)
{
  ++checks;
  if (actual != expected) {
    fprintf(stderr, "fixture%u %s[%u] actual%08x expected%08x\n", fixture, label, index, actual, expected);
    exit(1);
  }
}

static void *libio_bigint_allocate(size_t bytes)
{
  Bigint *result;
  if ((bytes != 52 && bytes != 84 && bytes != 148) || allocations >= 2 || event_n >= 2) {
    fprintf(stderr, "fixture%u allocation outside observer domain\n", fixture);
    exit(1);
  }
  result = (Bigint *)((unsigned char *)arena + 0x1000u + allocations++ * 0x200u);
  last_allocation = result;
  events[event_n][0] = 1;
  events[event_n][1] = (unsigned)bytes;
  events[event_n++][2] = offset(result);
  return result;
}

static void libio_bigint_release(void *pointer)
{
  Bigint *old = (Bigint *)pointer;
  if (event_n >= 2) {
    fprintf(stderr, "fixture%u free event outside observer domain\n", fixture);
    exit(1);
  }
  events[event_n][0] = 2;
  events[event_n][1] = offset(old);
  events[event_n++][2] = (unsigned short)old->on_stack;
  if (mutation) {
    if (!last_allocation) {
      fprintf(stderr, "fixture%u invalid mutation domain\n", fixture);
      exit(1);
    }
    last_allocation->wds = 3;
  }
}

static void initialize(Bigint *b, unsigned k, unsigned stack, unsigned sign,
                       unsigned n, const unsigned *limbs)
{
  unsigned i;
  b->k = (int)k;
  b->maxwds = (int)(1u << k);
  b->on_stack = (short)stack;
  b->sign = (short)sign;
  b->wds = (int)n;
  for (i = 0; i < n; ++i)
    b->x[i] = limbs[i];
}

int main(void)
{
  unsigned fixture_count = sizeof(libio_bigint_fixtures) / sizeof(libio_bigint_fixtures[0]);
  arena = (unsigned32 *)malloc(LIBIO_BIGINT_WORDS * sizeof(unsigned32));
  if (!arena) return 2;
  for (fixture = 0; fixture < fixture_count; ++fixture) {
    const LibioBigintFixture *f = &libio_bigint_fixtures[fixture];
    Bigint *a = (Bigint *)((unsigned char *)arena + 0x100u);
    Bigint *b = (Bigint *)((unsigned char *)arena + 0x400u);
    Bigint *c = (Bigint *)((unsigned char *)arena + 0x700u);
    int *e = (int *)((unsigned char *)arena + 0xc00u);
    int *bits = (int *)((unsigned char *)arena + 0xc04u);
    unsigned expected[LIBIO_BIGINT_WORDS];
    unsigned i, j;
    unsigned long long result;
    union { double d; unsigned32 words[2]; } value;
#ifdef LIBIO_BIGINT_ONLY_FIXTURE
    if (fixture != LIBIO_BIGINT_ONLY_FIXTURE) continue;
#endif
    allocations = event_n = 0;
    mutation = f->mutation;
    last_allocation = NULL;
    for (i = 0; i < LIBIO_BIGINT_WORDS; ++i)
      expected[i] = arena[i] = initial_word(i, f->seed);
    initialize(a, f->a_k, f->a_stack, f->a_sign, f->a_n, f->a);
    initialize(b, f->b_k, f->b_stack, f->b_sign, f->b_n, f->b);
    initialize(c, f->c_k, f->c_stack, f->c_sign, f->c_n, f->c);
    switch (f->method) {
    case 0: result = offset(Brealloc(f->is_null ? NULL : a, (int)f->argument)); break;
    case 1: result = offset(multadd(a, (int)f->argument, (int)f->addend)); break;
    case 2: result = offset(lshift(a, (int)f->argument)); break;
    case 3: result = offset(diff(f->is_null ? NULL : c, a, b)); break;
    case 4:
      value.d = b2d(a, e);
      result = ((unsigned long long)value.words[HIWORD] << 32) | value.words[LOWORD];
      break;
    case 5:
      value.words[HIWORD] = (unsigned32)(f->double_bits >> 32);
      value.words[LOWORD] = (unsigned32)f->double_bits;
      result = offset(d2b(f->is_null ? NULL : c, value.d, e, bits));
      break;
    default: return 3;
    }
    check_u32((unsigned)result, (unsigned)f->result, "return_low", 0);
    check_u32((unsigned)(result >> 32), (unsigned)(f->result >> 32), "return_high", 0);
    check_u32(event_n, f->event_n, "event_count", 0);
    for (i = 0; i < event_n; ++i)
      for (j = 0; j < 3; ++j)
        check_u32(events[i][j], f->events[i][j], "event", i * 3 + j);
    for (i = 0; i < f->delta_n; ++i) {
      if (f->deltas[i][0] >= LIBIO_BIGINT_WORDS ||
          (i && f->deltas[i - 1][0] >= f->deltas[i][0])) return 4;
      expected[f->deltas[i][0]] = f->deltas[i][1];
    }
    for (i = 0; i < LIBIO_BIGINT_WORDS; ++i)
      check_u32(arena[i], expected[i], "arena", i);
  }
  free(arena);
  printf("libio Bigint: %lu checks across %u complete original fixtures\n", checks, fixture_count);
  return 0;
}
