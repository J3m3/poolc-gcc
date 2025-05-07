#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#define INITIAL_CAP 8

#define DEFINE_VEC(name, type)                                                 \
  typedef struct {                                                             \
    type *items;                                                               \
    size_t length;                                                             \
    size_t capacity;                                                           \
  } name

#define vec_push(vec, item)                                                    \
  do {                                                                         \
    if ((vec)->capacity <= (vec)->length) {                                    \
      if ((vec)->capacity == 0)                                                \
        (vec)->capacity = INITIAL_CAP;                                         \
      else                                                                     \
        (vec)->capacity *= 2;                                                  \
      (vec)->items =                                                           \
          realloc((vec)->items, (vec)->capacity * sizeof(*(vec)->items));      \
    }                                                                          \
    (vec)->items[(vec)->length++] = (item);                                    \
  } while (0)

#define vec_pop(vec)                                                           \
  (assert((vec)->length > 0 && "Cannot pop from empty vector"),                \
   (vec)->items[--(vec)->length])

#define vec_at(vec, i)                                                         \
  (assert((i) < (vec)->length && "Index out of bounds"), (vec)->items[(i)])

#define vec_len(vec) (vec)->length

#define vec_clear(vec) (vec)->length = 0

#define vec_free(vec)                                                          \
  do {                                                                         \
    vec_clear((vec));                                                          \
    (vec)->capacity = 0;                                                       \
    free((vec)->items);                                                        \
  } while (0)

// Uses GNU-style `typeof`, standardized in C23:
//   - typeof(expr) deduces the type of `expr` without evaluating it
//   - Safe even if `expr` is NULL or uninitialized, unless VLA is involved
//   - References:
//     - https://en.cppreference.com/w/c/language/typeof
//     - https://gcc.gnu.org/onlinedocs/gcc/Typeof.html
// Compatibility:
//   - GCC / Clang: supported as an extension
//   - MSVC: supported since Visual Studio 17.9 or later, or cl.exe version
//           19.3933428 or later
// Edge case handling:
//   - Safe for zero arguments (no memory access unless element count > 0)
// Caveat:
//   - Do **not** call `vec_init` on a vector that has already been initialized
//     (i.e., without a prior call to `vec_free`), or memory will leak.
#define vec_init(vec, ...)                                                     \
  do {                                                                         \
    typeof(*(vec)) *_v = (vec);                                                \
    _v->length = 0;                                                            \
    _v->capacity = 0;                                                          \
    _v->items = NULL;                                                          \
                                                                               \
    typeof(*_v->items) _tmp[] = {__VA_ARGS__};                                 \
    size_t _n = sizeof(_tmp) / sizeof(typeof(*_v->items));                     \
                                                                               \
    for (size_t _i = 0; _i < _n; ++_i)                                         \
      vec_push(_v, _tmp[_i]);                                                  \
  } while (0)

// Uses GNU-style statement expressions:
//   - A compound statement enclosed in parentheses may appear as an expression
//     in GNU C.
//   - References:
//     - https://gcc.gnu.org/onlinedocs/gcc/Statement-Exprs.html
// Compatibility:
//   - GCC / Clang: supported as an extension
//   - MSVC: not supported
#define vec_find(vec, f)                                                       \
  ({                                                                           \
    ssize_t _res = -1;                                                         \
    for (size_t i = 0; i < vec_len(vec); ++i) {                                \
      if ((f)(vec_at(vec, i)) == 0) {                                          \
        _res = i;                                                              \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
    _res;                                                                      \
  })
