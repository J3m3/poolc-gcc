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
  vec_init(&v, ((Point){0, 0}), ((Point){1, 2}));

  assert(vec_len(&v) == 2);
  assert(vec_at(&v, 1).y == 2);

  vec_free(&v);
}

void test_vec_init_static_strings(void) {
  StaticStrings v;
  const char *s1 = "foo";
  const char *s2 = "bar";
  vec_init(&v, s1, s2);

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

// === Tests for vec_new ===
#if SUPPORTS_VEC_NEW
void test_vec_new_ints(void) {
  Ints v = vec_new(Ints, 4, 5, 6);

  assert(vec_len(&v) == 3);
  assert(vec_at(&v, 2) == 6);

  vec_free(&v);
}

void test_vec_new_points(void) {
  Points v = vec_new(Points, (Point){1, 2}, (Point){3, 4});

  Point p = vec_pop(&v);
  assert(p.x == 3 && p.y == 4);

  vec_free(&v);
}

void test_vec_new_static_strings(void) {
  StaticStrings v = vec_new(StaticStrings);

  const char *s = "poolc";
  vec_push(&v, s);
  assert(strncmp(vec_at(&v, 0), "poolc", strlen(s)) == 0);

  vec_free(&v);
}
#endif

int main(void) {
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

#if SUPPORTS_VEC_NEW
  test_vec_new_ints();
  test_vec_new_points();
  test_vec_new_static_strings();
#endif

  printf("PASS!\n");
  return 0;
}
