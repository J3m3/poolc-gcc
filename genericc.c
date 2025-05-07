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

void test_ints(void) {
  Ints v;
#if SUPPORTS_VEC_INIT
  vec_init(&v, 1, 3, 5, 8);
#else
  memset(&v, 0, sizeof(Ints));
  vec_push(&v, 1);
  vec_push(&v, 3);
  vec_push(&v, 5);
  vec_push(&v, 8);
#endif

  assert(vec_len(&v) == 4);
  assert(vec_at(&v, 2) == 5);

  vec_push(&v, 13);
  assert(vec_len(&v) == 5);
  assert(vec_at(&v, 4) == 13);

  assert(vec_pop(&v) == 13);
  assert(vec_len(&v) == 4);

#if SUPPORTS_VEC_FIND
  {
    ssize_t even_idx = vec_find(&v, is_even);
    assert(even_idx == 3);
  }
  {
    assert(vec_pop(&v) == 8);
    ssize_t even_idx = vec_find(&v, is_even);
    assert(even_idx == -1);
  }
#endif

  vec_clear(&v);
  assert(vec_len(&v) == 0);

  vec_free(&v);
}

void test_points(void) {
  Points pts1;
  Point p1 = {1, 2}, p2 = {3, 4}, origin = {0, 0};
#if SUPPORTS_VEC_INIT
  vec_init(&pts1, p1, p2, origin);
#else
  memset(&pts1, 0, sizeof(Points));
  vec_push(&pts1, p1);
  vec_push(&pts1, p2);
  vec_push(&pts1, origin);
#endif

  assert(vec_len(&pts1) == 3);
  assert(vec_at(&pts1, 0).x == 1);
  assert(vec_at(&pts1, 2).x == 0);

#if SUPPORTS_VEC_FIND
  ssize_t idx = vec_find(&pts1, is_origin);
  assert(idx == 2);
#endif

  vec_free(&pts1);

#if SUPPORTS_VEC_NEW
  Points pts2 = vec_new(Points, (Point){1, 2}, (Point){3, 4}, (Point){5, 6});
  Point p3 = vec_pop(&pts2);
  assert(p3.x == 5);
  assert(p3.y == 6);
  vec_free(&pts2);
#endif
}

void test_static_strings(void) {
  StaticStrings strs1;
#if SUPPORTS_VEC_INIT
  vec_init(&strs1, "foo", "bar", "hello", "baz");
#else
  memset(&strs1, 0, sizeof(StaticStrings));
  vec_push(&strs1, "foo");
  vec_push(&strs1, "bar");
  vec_push(&strs1, "hello");
  vec_push(&strs1, "baz");
#endif

  assert(vec_len(&strs1) == 4);
  assert(strcmp(vec_at(&strs1, 2), "hello") == 0);

#if SUPPORTS_VEC_FIND
  ssize_t idx = vec_find(&strs1, match_hello);
  assert(idx == 2);
#endif

  vec_free(&strs1);

#if SUPPORTS_VEC_NEW
  StaticStrings strs2 = vec_new(StaticStrings);
  const char *poolc = "poolc";
  vec_push(&strs2, poolc);
  const char *last = vec_at(&strs2, vec_len(&strs2) - 1);
  assert(strncmp(last, "poolc", strlen(poolc)) == 0);
  vec_free(&strs2);
#endif
}

int main(void) {
  test_ints();
  test_points();
  test_static_strings();
  printf("PASS!\n");
  return 0;
}
