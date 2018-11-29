#include "olm/olm.h"
#include "ext_lib_olm.h"

static VALUE last_error(VALUE self)
{
    OlmUtility *this;
    Data_Get_Struct(self, OlmUtility, this);
    
    return rb_str_new2(olm_utility_last_error(this));
}

static VALUE ed25519_verify(VALUE self, VALUE data, VALUE key, VALUE signature)
{
    VALUE retval = Qtrue;
    OlmUtility *this;
    Data_Get_Struct(self, OlmUtility, this);
    
    if(olm_ed25519_verify(this, RSTRING_PTR(key), RSTRING_LEN(key), RSTRING_PTR(data), RSTRING_LEN(data), RSTRING_PTR(dup_string(signature)), RSTRING_LEN(signature)) == olm_error()){
        
        retval = Qfalse;
    }
    
    return retval;
}

static VALUE sha256(VALUE self, VALUE data)
{
    size_t size;
    OlmUtility *this;
    Data_Get_Struct(self, OlmUtility, this);

    size = olm_sha256_length(this);
    uint8_t buf[size];
    
    (void)olm_sha256(this, RSTRING_PTR(data), RSTRING_LEN(data), buf, size);
    
    return rb_str_new(buf, size);
}

static void _free(void *ptr)
{    
    olm_clear_utility(ptr);
    free(ptr);
}

static VALUE _alloc(VALUE klass)
{
    OlmUtility *this;    
    VALUE self;
    
    self = Data_Wrap_Struct(klass, 0, _free, calloc(1, olm_utility_size())); 
    
    Data_Get_Struct(self, OlmUtility, this);   
    
    (void)olm_utility((void *)this);
    
    return self;
}

void utility_init(void)
{
    VALUE cRubyOLM = rb_define_module("RubyOlm");    
    VALUE cUtility = rb_define_class_under(cRubyOLM, "Utility", rb_cObject);
    
    rb_define_alloc_func(cUtility, _alloc);
    
    rb_define_method(cUtility, "sha256", sha256, 1);
    rb_define_method(cUtility, "ed25519_verify", sha256, 3);
}
