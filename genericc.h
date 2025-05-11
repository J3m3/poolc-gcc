#ifndef GENERICC_H
#define GENERICC_H

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
#ifndef SUPPORTS_VEC_FOREACH
#define SUPPORTS_VEC_FOREACH HAS_TYPEOF
#endif
#ifndef SUPPORTS_VEC_FIND
#define SUPPORTS_VEC_FIND (HAS_TYPEOF && HAS_STMT_EXPRS)
#endif

#define INITIAL_CAP 8
#define CAP_INC_FACTOR 2

#define TYPE_EQ(expr, type) _Generic((expr), type: 1, default: 0)

// SAFETY: This is neither reentrant nor thread-safe!
#define DEFINE_VEC(name, type)                                                 \
  typedef struct {                                                             \
    type *items;                                                               \
    size_t length;                                                             \
    size_t capacity;                                                           \
  } name

#define vec_reserve(vec, expected_cap)                                         \
  do {                                                                         \
    if ((vec)->capacity < expected_cap) {                                      \
      if ((vec)->capacity == 0)                                                \
        (vec)->capacity = INITIAL_CAP;                                         \
      while ((vec)->capacity < expected_cap)                                   \
        (vec)->capacity *= CAP_INC_FACTOR;                                     \
      (vec)->items =                                                           \
          realloc((vec)->items, (vec)->capacity * sizeof(*(vec)->items));      \
      assert((vec)->items != NULL && "Cannot allocate more memory");           \
    }                                                                          \
  } while (0)

#define vec_push(vec, item)                                                    \
  do {                                                                         \
    vec_reserve((vec), (vec)->length + 1);                                     \
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

// Caveat:
//   - Do **not** call `vec_init` on a vector that has already been initialized
//     (i.e., without a prior call to `vec_free`), or memory will leak.
//   - Let me first post warnings I got when I compile with `-pedantic`:
//
// clang-format off
// <source>:248:24: warning: passing no argument for the '...' parameter of a variadic macro is a C23 extension [-Wc23-extensions]
//    248 |   vec_init_with(int, &v);
//        |                        ^
//  <source>:77:9: note: macro 'vec_init_with' defined here
//     77 | #define vec_init_with(elem_type, vec, ...)                                     \
//        |         ^
//  <source>:248:3: warning: use of an empty initializer is a C23 extension [-Wc23-extensions]
//    248 |   vec_init_with(int, &v);
//        |   ^
//  <source>:86:24: note: expanded from macro 'vec_init_with'
//     86 |     elem_type _tmp[] = {__VA_ARGS__};                                          \
//        |                        ^
//  <source>:248:3: warning: zero size arrays are an extension [-Wzero-length-array]
//  <source>:86:24: note: expanded from macro 'vec_init_with'
//     86 |     elem_type _tmp[] = {__VA_ARGS__};                                          \
//        |                        ^
// clang-format on
//
//     At first glance, these warnings are expected. To work around the issue,
//     we can count the number of arguments given as `...`, and dispatch
//     different `vec_init` (e.g. `vec_init0`, `vec_initN`). The below article
//     shows how can we determine if `...` is empty in C99:
//
//     https://gustedt.wordpress.com/2010/06/08/detect-empty-macro-arguments/#comment-169
//     `- referred by:
//        https://t6847kimo.github.io/blog/2019/02/04/Remove-comma-in-variadic-macro.html
//
//     However, clang 19.1.7 still generates same warnings even though we use
//     something like below:
//     (https://github.com/torvalds/linux/blob/3ce9925823c7d6bb0e6eb951bf2db0e9e182582d/include/linux/args.h#L21)
//
//
//     #define __COUNT_ARGS(_0, _1, _2, _3, _4, _5, _6, _n, X...) _n
//     #define COUNT_ARGS(X...) __COUNT_ARGS(, ##X, 6, 5, 4, 3, 2, 1, 0)
//
//     This is weird, since there should be an empty token in the view of C
//     preprocessor according to the comment(the link above) from Gustedt. This
//     is a big rabbit-hole...
//
//     Furthermore, gcc 14.2 emits the following error, whereas clang 19.1.7
//     only issues a warning for the same code. This distinction is important
//     because gcc treats the issue as a compilation blocker, while clang
//     permits the build to continue:
//
//     <source>:96:15: error: zero or negative size array '_tmp'
//        96 |     elem_type _tmp[] = {__VA_ARGS__};                           \
//           |               ^~~~
//
//     So users should ensure that they pass at least one argument for `...` to
//     make their code portable. It's generally not a good idea to impose this
//     responsibility to user. However, as we have seen, dealing with this issue
//     only with C standard features is too verbose, especially considering that
//     this file is intended for education. Rather, I think sharing this kind of
//     consideration is itself educational, so I'm leaving it this as-is.
#define vec_init_with(elem_type, vec, ...)                                     \
  do {                                                                         \
    _Static_assert(TYPE_EQ(*(vec)->items, elem_type),                          \
                   "Incompatible element type");                               \
                                                                               \
    (vec)->length = 0;                                                         \
    (vec)->capacity = 0;                                                       \
    (vec)->items = NULL;                                                       \
                                                                               \
    elem_type _tmp[] = {__VA_ARGS__};                                          \
    size_t _n = sizeof(_tmp) / sizeof(elem_type);                              \
                                                                               \
    vec_reserve((vec), _n);                                                    \
    memcpy((vec)->items + (vec)->length, _tmp, sizeof(_tmp));                  \
    (vec)->length += _n;                                                       \
  } while (0)

// Note:
//   - `it` here is a pointer to the current element.
#define vec_foreach_with(elem_type, it, vec)                                   \
  for (elem_type *it = (vec)->items; it < (vec)->items + (vec)->length; ++it)

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
// Caveat: same as `vec_init_with`
#define vec_init(vec, ...)                                                     \
  vec_init_with(typeof(*(vec)->items), (vec), __VA_ARGS__)

#define vec_foreach(it, vec)                                                   \
  vec_foreach_with(typeof(*(vec)->items), (it), (vec))

#else // HAS_TYPEOF

#pragma message(                                                               \
    "Warning: vec_init and vec_foreach is disabled on this compiler; use vec_init_with")

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

#else // HAS_STMT_EXPRS && HAS_TYPEOF

#pragma message("Warning: vec_find and vec_new are disabled on this compiler.")

#endif // HAS_STMT_EXPRS && HAS_TYPEOF

#endif // GENERICC_H
