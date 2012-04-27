#include <ruby.h>
#include <ruby/intern.h>

VALUE cRefArray;

typedef struct ary
{
  VALUE ary;     // ary
  long begin;    // begin index
  long len;      // len array
  long count;    // ref count
  void *src;     // pointer to other array

} ARRAY;

void ra_mark(void *a)
{
  rb_gc_mark(((ARRAY*)a)->ary);
}

void ra_free(ARRAY *a)
{
  if (a->count <= 1) {
    rb_ary_free(a->ary);
    free(a);
  }
}

void ra_free_ref(ARRAY *a)
{
  if (a->src != NULL) {
    ((ARRAY*)a->src)->count--;
    free(a);
  }
}

static VALUE ra_new(VALUE klass, VALUE ary)
{
  ARRAY *a = malloc(sizeof(ARRAY));

  a->ary =rb_ary_dup(ary);
  a->begin = 0;
  a->len = RARRAY_LEN(ary);
  a->count = 1;
  a->src = NULL;

  return Data_Wrap_Struct(klass, ra_mark, ra_free, a);
}

/* 
 *  a = RefArray.new([1,2,3])
 *  a.to_a #=> [1, 2, 3]
 * */
static VALUE ra_to_a(VALUE self)
{
  ARRAY *a;

  Data_Get_Struct(self, ARRAY, a);
  return rb_ary_subseq(a->ary, a->begin, a->len);
}

/* 
 *  a = RefArray.new([1,2,3])
 *  b = a[0..1]
 *  b.to_a #=> [1,2]
 * */
static VALUE ra_get_indexer(VALUE self, VALUE index)
{

  ARRAY *a, *new_a;
  VALUE beg, end, c;
  int i = 0;

  Data_Get_Struct(self, ARRAY, a);
 
  rb_range_values(index, &beg, &end, &i);

  new_a = malloc(sizeof(ARRAY));
  new_a->ary = a->ary;
  new_a->begin = FIX2LONG(beg);
  new_a->len = FIX2LONG(end) - new_a->begin + 1;
  new_a->count = 0;
  new_a->src = (void*)a;

  return Data_Wrap_Struct(CLASS_OF(self), NULL, ra_free_ref, new_a);
}

/* 
 * a = RefArray.new([1,2,3])
 * b = a[0..1]
 * b[0] = 999
 * a.to_a #=> [999, 2, 3]
 * */
static VALUE ra_set_indexer(VALUE self, VALUE index, VALUE val)
{
  ARRAY *a;
  VALUE ary;
  Data_Get_Struct(self, ARRAY, a);
  
  rb_ary_store(a->ary, FIX2LONG(index) + a->begin, val);
  return val;
}

void Init_ref_array()
{
  cRefArray = rb_define_class("RefArray", rb_cObject);

  rb_define_singleton_method(cRefArray, "new", ra_new, 1);

  rb_define_method(cRefArray, "to_a", ra_to_a, 0);
  rb_define_method(cRefArray, "[]", ra_get_indexer, 1);
  rb_define_method(cRefArray, "[]=", ra_set_indexer, 2);
}
