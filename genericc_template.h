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
#define DEFINE_VEC(name, type)

#define vec_push(vec, item)

#define vec_pop(vec)

#define vec_at(vec, i)

#define vec_len(vec)

#define vec_clear(vec)

#define vec_free(vec)

#define vec_init_with(elem_type, vec, ...)

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
#define vec_init(vec, ...)

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
#define vec_find(vec, f)

#define vec_new(type, ...)

#else // HAS_STMT_EXPRS && HAS_TYPEOF

#pragma message("Warning: vec_find and vec_new are disabled on this compiler.")

#endif // HAS_STMT_EXPRS && HAS_TYPEOF
