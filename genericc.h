#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#if defined(__GNUC__) || defined(__clang__)
#define HAS_TYPEOF 1
#define HAS_STMT_EXPRS 1
#elif defined(_MSC_FULL_VER) && _MSC_FULL_VER >= 193933428
#define HAS_TYPEOF 1
#define HAS_STMT_EXPRS 0
#else
#define HAS_TYPEOF 0
#define HAS_STMT_EXPRS 0
#endif

#ifndef SUPPORTS_VEC_INIT
#define SUPPORTS_VEC_INIT HAS_TYPEOF
#endif
#ifndef SUPPORTS_VEC_NEW
#define SUPPORTS_VEC_NEW (HAS_TYPEOF && HAS_STMT_EXPRS)
#endif
#ifndef SUPPORTS_VEC_FIND
#define SUPPORTS_VEC_FIND (HAS_TYPEOF && HAS_STMT_EXPRS)
#endif

#define INITIAL_CAP 8
#define CAP_INC_FACTOR 2

// SAFETY: This is neither reentrant nor thread-safe!
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
        (vec)->capacity *= CAP_INC_FACTOR;                                     \
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
    if ((vec)->items != NULL)                                                  \
      free((vec)->items);                                                      \
  } while (0)

#define vec_init_with(elem_type, vec, ...)                                     \
  do {                                                                         \
    (vec)->length = 0;                                                         \
    (vec)->capacity = 0;                                                       \
    (vec)->items = NULL;                                                       \
                                                                               \
    elem_type _tmp[] = {__VA_ARGS__};                                          \
    size_t _n = sizeof(_tmp) / sizeof(elem_type);                              \
                                                                               \
    for (size_t _i = 0; _i < _n; ++_i)                                         \
      vec_push((vec), _tmp[_i]);                                               \
  } while (0)

#if HAS_TYPEOF

// Uses GNU-style `typeof`, standardized in C23:
//   - typeof(expr) deduces the type of `expr` without evaluating it
//   - Safe even if `expr` is NULL or uninitialized, unless VLA is involved
//   - References:
//     - https://en.cppreference.com/w/c/language/typeof
//     - https://gcc.gnu.org/onlinedocs/gcc/Typeof.html
// Compatibility:
//   - GCC / Clang: supported as an extension
//   - MSVC: supported since Visual Studio 17.9 or later, or cl.exe version
//           19.3933428 or later (`/std:clatest` option required)
// Edge case handling:
//   - Safe for zero arguments (no memory access unless element count > 0)
// Caveat:
//   - Do **not** call `vec_init` on a vector that has already been initialized
//     (i.e., without a prior call to `vec_free`), or memory will leak.
#define vec_init(vec, ...)                                                     \
  vec_init_with(typeof(*(vec)->items), (vec), __VA_ARGS__)

#else // HAS_TYPEOF

#pragma message(                                                               \
    "Warning: vec_init is disabled on this compiler; use vec_init_with")

#endif // HAS_TYPEOF

#if HAS_STMT_EXPRS && HAS_TYPEOF

// Uses GNU-style statement expressions:
//   - A compound statement enclosed in parentheses may appear as an expression
//     in GNU C.
//   - References:
//     - https://gcc.gnu.org/onlinedocs/gcc/Statement-Exprs.html
// Compatibility:
//   - GCC / Clang: supported as an extension
//   - MSVC: not supported
// Note:
//   - Why use a function pointer instead of passing a value to find?
//     When comparing null-terminated C strings, we usually don't want to
//     compare their addresses using `==`; instead, we want to compare the
//     contents character by character using something like `strncmp`.
//     Using a function pointer gives users more flexibility in defining
//     custom comparison logic, so we prefer this approach.
#define vec_find(vec, f)                                                       \
  ({                                                                           \
    ssize_t _res = -1;                                                         \
    for (size_t i = 0; i < (vec)->length; ++i) {                               \
      if ((f)(vec_at(vec, i)) == 0) {                                          \
        _res = i;                                                              \
        break;                                                                 \
      }                                                                        \
    }                                                                          \
    _res;                                                                      \
  })

#define vec_new(type, ...)                                                     \
  ({                                                                           \
    type _v = {0};                                                             \
                                                                               \
    typeof(*_v.items) _tmp[] = {__VA_ARGS__};                                  \
    size_t _n = sizeof(_tmp) / sizeof(*_v.items);                              \
                                                                               \
    for (size_t _i = 0; _i < _n; ++_i)                                         \
      vec_push(&_v, _tmp[_i]);                                                 \
                                                                               \
    _v;                                                                        \
  })

#else // HAS_STMT_EXPRS && HAS_TYPEOF

#pragma message("Warning: vec_find and vec_new are disabled on this compiler.")

#endif // HAS_STMT_EXPRS && HAS_TYPEOF
