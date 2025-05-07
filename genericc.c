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
int match_hello(const char *s) { return strcmp(s, "hello"); }

void test_ints(void) {
  Ints v;
  vec_init(&v, 1, 3, 5, 8);
  assert(vec_len(&v) == 4);
  assert(vec_at(&v, 2) == 5);

  vec_push(&v, 13);
  assert(vec_len(&v) == 5);
  assert(vec_at(&v, 4) == 13);

  assert(vec_pop(&v) == 13);
  assert(vec_len(&v) == 4);
  {
    ssize_t even_idx = vec_find(&v, is_even);
    assert(even_idx == 3);
  }
  {
    assert(vec_pop(&v) == 8);
    ssize_t even_idx = vec_find(&v, is_even);
    assert(even_idx == -1);
  }

  vec_clear(&v);
  assert(vec_len(&v) == 0);

  vec_free(&v);
}

void test_points(void) {
  Points pts;
  Point p1 = {1, 2}, p2 = {3, 4}, origin = {0, 0};
  vec_init(&pts, p1, p2, origin);

  assert(vec_len(&pts) == 3);
  assert(vec_at(&pts, 0).x == 1);
  assert(vec_at(&pts, 2).x == 0);

  ssize_t idx = vec_find(&pts, is_origin);
  assert(idx == 2);

  vec_free(&pts);
}

void test_static_strings(void) {
  StaticStrings strs;
  vec_init(&strs, "foo", "bar", "hello", "baz");

  assert(vec_len(&strs) == 4);
  assert(strcmp(vec_at(&strs, 2), "hello") == 0);

  ssize_t idx = vec_find(&strs, match_hello);
  assert(idx == 2);

  vec_free(&strs);
}

int main(void) {
  test_ints();
  test_points();
  test_static_strings();
  printf("PASS!\n");
  return 0;
}
