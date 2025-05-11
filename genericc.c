// We can definitely use -I for different include paths, but let's go with much
// simpler apporach. Change below line to `#include "genericc_template.h"`
// This removes setting up compiler_commands.json or similar stuffs for your
// language server to work properly.
#include "genericc.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

DEFINE_VEC(Ints, int);
DEFINE_VEC(StaticStrings, const char *);
typedef struct {
  int x;
  int y;
} Point;
DEFINE_VEC(Points, Point);

int is_even(int x) { return x % 2 != 0; }
int is_origin(Point p) { return !(p.x == 0 && p.y == 0) ? 1 : 0; }
int match_hello(const char *s) {
  const char *hello = "hello";
  return strncmp(s, hello, strlen(hello));
}

// === Tests for vec_push/pop/at/len/clear ===
void test_vec_basic_ints(void) {
  Ints v = {0};
  vec_push(&v, 5 + 5);
  vec_push(&v, 10 + 10);
  vec_push(&v, 15 + 15);

  assert(vec_len(&v) == 3);
  assert(vec_at(&v, 1) == 20);
  assert(vec_pop(&v) == 30);
  assert(vec_len(&v) == 2);

  vec_clear(&v);
  assert(vec_len(&v) == 0);

  vec_free(&v);
}

void test_vec_basic_points(void) {
  Points pts = {0};
  vec_push(&pts, ((Point){1, 2}));
  vec_push(&pts, ((Point){3, 4}));
  vec_push(&pts, ((Point){5, 6}));

  assert(vec_len(&pts) == 3);
  assert(vec_at(&pts, 1).x == 3);
  assert(vec_at(&pts, 1).y == 4);

  Point last = vec_pop(&pts);
  assert(last.x == 5 && last.y == 6);
  assert(vec_len(&pts) == 2);

  vec_clear(&pts);
  assert(vec_len(&pts) == 0);

  vec_free(&pts);
}

void test_vec_basic_static_strings(void) {
  StaticStrings strs = {0};
  const char *s1 = "alpha";
  const char *s2 = "beta";
  const char *s3 = "gamma";
  vec_push(&strs, s1);
  vec_push(&strs, s2);
  vec_push(&strs, s3);

  assert(vec_len(&strs) == 3);
  assert(strncmp(vec_at(&strs, 1), "beta", strlen(s2)) == 0);

  const char *last = vec_pop(&strs);
  assert(strncmp(last, "gamma", strlen(s3)) == 0);
  assert(vec_len(&strs) == 2);

  vec_clear(&strs);
  assert(vec_len(&strs) == 0);

  vec_free(&strs);
}

// === Tests for vec_init_with ===
void test_vec_init_with_ints(void) {
  Ints v;
  vec_init_with(int, &v, 1, 2, 3);

  assert(vec_len(&v) == 3);
  assert(vec_at(&v, 0) == 1);

  vec_free(&v);
}

void test_vec_init_with_points(void) {
  Points v;
  vec_init_with(Point, &v, (Point){0, 0}, (Point){1, 2});

  assert(vec_len(&v) == 2);
  assert(vec_at(&v, 1).y == 2);

  vec_free(&v);
}

void test_vec_init_with_static_strings(void) {
  StaticStrings v;
  const char *s1 = "foo";
  const char *s2 = "bar";
  char *s3 = "baz";
  vec_init_with(const char *, &v, s1, s2, s3);

  assert(strncmp(vec_at(&v, 1), "bar", strlen(s2)) == 0);

  vec_free(&v);
}

// === Tests for vec_foreach ===
void test_vec_foreach_with_ints(void) {
  Ints v;
  vec_init_with(int, &v, 2, 3, 5, 7, 11);

  size_t ref_idx = 0;
  int ref_arr[] = {2, 3, 5, 7, 11};
  vec_foreach_with(int, x, &v) {
    size_t idx = x - v.items;
    assert(idx == ref_idx++);
    assert(*x == ref_arr[idx]);
  }

  vec_free(&v);
}

void test_vec_foreach_with_points(void) {
  Points v;
  vec_init_with(Point, &v, (Point){2, 3}, (Point){5, 7}, (Point){11, 13});

  size_t ref_idx = 0;
  Point ref_arr[] = {(Point){2, 3}, (Point){5, 7}, (Point){11, 13}};
  vec_foreach_with(Point, p, &v) {
    size_t idx = p - v.items;
    assert(idx == ref_idx++);
    assert(p->x == ref_arr[idx].x);
    assert(p->y == ref_arr[idx].y);
  }

  vec_free(&v);
}

void test_vec_foreach_with_static_strings(void) {
  StaticStrings v;
  const char *s1 = "foo";
  const char *s2 = "bar";
  char *s3 = "baz";
  vec_init_with(const char *, &v, s1);
  vec_push(&v, s2);
  vec_push(&v, s3);

  size_t ref_idx = 0;
  const char *ref_arr[] = {s1, s2, s3};
  vec_foreach_with(const char *, s, &v) {
    size_t idx = s - v.items;
    assert(idx == ref_idx++);
    assert(strncmp(*s, ref_arr[idx], strlen(*s)) == 0);
  }

  vec_free(&v);
}

#if SUPPORTS_VEC_FOREACH
void test_vec_foreach_ints(void) {
  Ints v;
  vec_init_with(int, &v, 2, 3, 5, 7, 11);

  size_t ref_idx = 0;
  int ref_arr[] = {2, 3, 5, 7, 11};
  vec_foreach(x, &v) {
    size_t idx = x - v.items;
    assert(idx == ref_idx++);
    assert(*x == ref_arr[idx]);
  }

  vec_free(&v);
}

void test_vec_foreach_points(void) {
  Points v;
  vec_init_with(Point, &v, (Point){2, 3}, (Point){5, 7}, (Point){11, 13});

  size_t ref_idx = 0;
  Point ref_arr[] = {(Point){2, 3}, (Point){5, 7}, (Point){11, 13}};
  vec_foreach(p, &v) {
    size_t idx = p - v.items;
    assert(idx == ref_idx++);
    assert(p->x == ref_arr[idx].x);
    assert(p->y == ref_arr[idx].y);
  }

  vec_free(&v);
}

void test_vec_foreach_static_strings(void) {
  StaticStrings v;
  const char *s1 = "foo";
  const char *s2 = "bar";
  char *s3 = "baz";
  vec_init_with(const char *, &v, s1);
  vec_push(&v, s2);
  vec_push(&v, s3);

  size_t ref_idx = 0;
  const char *ref_arr[] = {s1, s2, s3};
  vec_foreach(s, &v) {
    size_t idx = s - v.items;
    assert(idx == ref_idx++);
    assert(strncmp(*s, ref_arr[idx], strlen(*s)) == 0);
  }

  vec_free(&v);
}
#endif

// === Tests for vec_init ===
#if SUPPORTS_VEC_INIT
void test_vec_init_ints(void) {
  Ints v;
  vec_init(&v, 1, 2, 3);

  assert(vec_len(&v) == 3);
  assert(vec_at(&v, 0) == 1);

  vec_free(&v);
}

void test_vec_init_points(void) {
  Points v;
  vec_init(&v, (Point){0, 0}, (Point){1, 2});

  assert(vec_len(&v) == 2);
  assert(vec_at(&v, 1).y == 2);

  vec_free(&v);
}

void test_vec_init_static_strings(void) {
  StaticStrings v;
  const char *s1 = "foo";
  const char *s2 = "bar";
  char *s3 = "baz";
  vec_init_with(const char *, &v, s1);
  vec_push(&v, s2);
  vec_push(&v, s3);

  assert(strncmp(vec_at(&v, 1), "bar", strlen(s2)) == 0);

  vec_free(&v);
}
#endif

// === Tests for vec_find ===
#if SUPPORTS_VEC_FIND
void test_vec_find_ints(void) {
  Ints v = {0};
  vec_push(&v, 1);
  vec_push(&v, 3);
  vec_push(&v, 5);
  vec_push(&v, 8);

  ssize_t idx = vec_find(&v, is_even);
  assert(idx == 3);

  vec_pop(&v);
  idx = vec_find(&v, is_even);
  assert(idx == -1);

  vec_free(&v);
}

void test_vec_find_points(void) {
  Points v = {0};
  vec_push(&v, ((Point){3, 3}));
  vec_push(&v, ((Point){1, 2}));
  vec_push(&v, ((Point){0, 0}));

  ssize_t idx = vec_find(&v, is_origin);
  assert(idx == 2);

  vec_free(&v);
}

void test_vec_find_strings(void) {
  StaticStrings v = {0};
  vec_push(&v, "foo");
  vec_push(&v, "bar");
  vec_push(&v, "hello");
  vec_push(&v, "baz");

  ssize_t idx = vec_find(&v, match_hello);
  assert(idx == 2);

  vec_free(&v);
}
#endif

// We can use #ifdef ... #endif, but let's go with simpler approach:
// commenting out irrelevant tests. Then Visual Studio users can also easily
// follow the process.
int main(void) {
  test_vec_basic_ints();

  test_vec_init_with_ints();
  test_vec_init_with_points();
  test_vec_init_with_static_strings();

  test_vec_foreach_with_ints();
  test_vec_foreach_with_points();
  test_vec_foreach_with_static_strings();

#if SUPPORTS_VEC_FOREACH
  test_vec_foreach_ints();
  test_vec_foreach_points();
  test_vec_foreach_static_strings();
#endif
#if SUPPORTS_VEC_INIT
  test_vec_init_ints();
  test_vec_init_points();
  test_vec_init_static_strings();
#endif
#if SUPPORTS_VEC_FIND
  test_vec_find_ints();
  test_vec_find_points();
  test_vec_find_strings();
#endif

  printf("PASS!\n");
  return 0;
}
