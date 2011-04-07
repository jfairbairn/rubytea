#include <stdint.h>
#include <stdio.h>
#include <ruby.h>
#define DELTA 0x9e3779b9
#define MX (((z>>5^y<<2) + (y>>3^z<<4)) ^ ((sum^y) + (k[(p&3)^e] ^ z)))

void btea(uint32_t *v, int n, uint32_t const k[4]) {
  uint32_t y, z, sum;
  int p, rounds, e;
  if (n > 1) {          /* Coding Part */
    rounds = 6 + 52/n;
    sum = 0;
    z = v[n-1];
    do {
      sum += DELTA;
      e = (sum >> 2) & 3;
      for (p=0; p<n-1; p++) {
        y = v[p+1]; 
        z = v[p] += MX;
      }
      y = v[0];
      z = v[n-1] += MX;
    } while (--rounds);
  } else if (n < -1) {  /* Decoding Part */
    n = -n;
    rounds = 6 + 52/n;
    sum = rounds*DELTA;
    y = v[0];
    do {
      e = (sum >> 2) & 3;
      for (p=n-1; p>0; p--) {
        z = v[p-1];
        y = v[p] -= MX;
      }
      z = v[n-1];
      y = v[0] -= MX;
    } while ((sum -= DELTA) != 0);
  }
}

typedef struct {
  uint32_t key[4];
} TEAKey;

void tkey_free(void *teakey) {
  free(teakey);
}

VALUE tkey_alloc(VALUE klass) {
  TEAKey *key;
  VALUE obj;
  key = malloc(sizeof(TEAKey));
  obj = Data_Wrap_Struct(klass, 0, tkey_free, key);
}

VALUE tkey_init(VALUE self, VALUE k0, VALUE k1, VALUE k2, VALUE k3) {
  TEAKey *key;
  Data_Get_Struct(self, TEAKey, key);
  key->key[0] = NUM2UINT(k0);
  key->key[1] = NUM2UINT(k1);
  key->key[2] = NUM2UINT(k2);
  key->key[3] = NUM2UINT(k3);
  return self;
}

VALUE tkey_init_copy(VALUE copy, VALUE orig) {
  TEAKey *key_copy, *key_orig;
  Data_Get_Struct(copy, TEAKey, key_copy);
  Data_Get_Struct(orig, TEAKey, key_orig);

  if (TYPE(orig) != T_DATA || RDATA(orig)->dfree != (RUBY_DATA_FUNC)tkey_free)
    rb_raise(rb_eTypeError, "wrong argument type copying TEAKey");

  MEMCPY(key_copy, key_orig, TEAKey, 1);

  return copy;
}

VALUE rubytea_exec(VALUE self, VALUE data, VALUE rkey, char op)
{
  if (rb_obj_is_kind_of(data, rb_cArray) != Qtrue)
  {
    rb_raise(rb_eArgError, "Invalid data passed to TEA::encrypt(): array of integers expected");
    return Qnil;
  }
  int len = RARRAY_LEN(data);
  int i;
  if (len < 2)
  {
    rb_raise(rb_eArgError, "TEA::encrypt() can only encrypt an array with two or more ints");
    return Qnil;
  }
  size_t c = sizeof(uint32_t) * len;
  uint32_t *ptr = ALLOC_N(uint32_t, len);

  for (i = 0; i < len; ++i)
  {
    ptr[i] = NUM2UINT(rb_ary_entry(data, i));
  }

  TEAKey *key;

  Data_Get_Struct(rkey, TEAKey, key);

  btea(ptr, op * len, key->key);

  for (i = 0; i < len; ++i)
  {
    rb_ary_store(data, i, UINT2NUM(ptr[i]));
  }

  xfree(ptr);
  return data;

}

VALUE rubytea_encrypt(VALUE self, VALUE data, VALUE key)
{
  return rubytea_exec(self, data, key, 1);
}

VALUE rubytea_decrypt(VALUE self, VALUE data, VALUE key)
{
  return rubytea_exec(self, data, key, -1);
}

void Init_rubytea()
{
  VALUE RubyTEA;
  RubyTEA = rb_define_module("TEA");

  rb_define_module_function(RubyTEA, "encrypt!", rubytea_encrypt, 2);
  rb_define_module_function(RubyTEA, "decrypt!", rubytea_decrypt, 2);

  VALUE TK;
  TK = rb_define_class_under(RubyTEA, "Key", rb_cObject);
  rb_define_alloc_func(TK, tkey_alloc);
  rb_define_method(TK, "initialize", tkey_init, 4);
  rb_define_method(TK, "initialize_copy", tkey_init_copy, 1);
}

