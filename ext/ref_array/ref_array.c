#include <ruby.h>
#include <ruby/intern.h>

VALUE cRefArray;

/* Array as reference to aray in @@refs */
typedef struct ary
{
  long id;        // index to @@refs and @@counts
  long index;     // index in @@refs[id]
  long len;       // len array

} ARRAY;

#define REFS rb_cv_get(CLASS_OF(cRefArray), "@@refs")
#define COUNTS rb_cv_get(CLASS_OF(cRefArray), "@@counts")

static VALUE ra_get_refs(VALUE klass)
{
  return REFS;
}

void ra_ary_delete(ARRAY *a)
{
  VALUE c;
  c = rb_ary_entry(COUNTS, a->id);
  
  if (FIX2LONG(c) > 1) {
    /* decriment count of references */
    rb_ary_store(COUNTS, a->id, rb_funcall(c, rb_intern("pred"), 0));
  } 
  else {
    /* Delete if other references doesn't exist */
    rb_ary_store(REFS, a->id, Qnil);
    free(a);
  }
}

static VALUE ra_new(VALUE klass, VALUE ary)
{
  ARRAY *a = malloc(sizeof(ARRAY));

  a->id = RARRAY_LEN(REFS);
  a->index = 0;
  a->len = RARRAY_LEN(ary);

  /* registrate array */
  rb_ary_push(REFS, rb_ary_dup(ary));
  rb_ary_push(COUNTS, INT2FIX(1));

  return Data_Wrap_Struct(klass, NULL, ra_ary_delete, a);
}

/* 
 *  a = RefArray.new([1,2,3])
 *  a.to_a #=> [1, 2, 3]
 * */
static VALUE ra_to_a(VALUE self)
{
  ARRAY *a;
  VALUE ary;

  Data_Get_Struct(self, ARRAY, a);
  ary  = rb_ary_entry(REFS, a->id);
  return rb_ary_subseq(ary, a->index, a->len);
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
  new_a->id = a->id;
  new_a->index = FIX2LONG(beg);
  new_a->len = FIX2LONG(end) - new_a->index + 1;


  c = rb_ary_entry(COUNTS, a->id);
  rb_ary_store(COUNTS, a->id, rb_funcall(c, rb_intern("next"), 0));

  return Data_Wrap_Struct(CLASS_OF(self), NULL, ra_ary_delete, new_a);
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
  
  ary = rb_ary_entry(REFS, a->id);
  rb_ary_store(ary, FIX2LONG(index) + a->index, val);
  return val;
}

void Init_ref_array()
{
  cRefArray = rb_define_class("RefArray", rb_cObject);

  rb_define_class_variable(cRefArray, "@@refs", rb_ary_new());
  rb_define_class_variable(cRefArray, "@@counts", rb_ary_new());
  rb_define_singleton_method(cRefArray, "refs", ra_get_refs, 0);
  rb_define_singleton_method(cRefArray, "new", ra_new, 1);

  rb_define_method(cRefArray, "to_a", ra_to_a, 0);
  rb_define_method(cRefArray, "[]", ra_get_indexer, 1);
  rb_define_method(cRefArray, "[]=", ra_set_indexer, 2);
}
