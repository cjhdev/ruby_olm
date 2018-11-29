#include "olm/olm.h"
#include "ext_lib_olm.h"

static VALUE last_error(VALUE self)
{
    OlmAccount *this;
    Data_Get_Struct(self, OlmAccount, this);
    
    return rb_funcall(rb_eval_string("RubyOlm::OlmError"), rb_intern("from_string"), 1, rb_str_new2(olm_account_last_error(this)));
}

static VALUE initialize(int argc, VALUE *argv, VALUE self)
{
    VALUE pickle, password, opts;
    size_t size;
    OlmAccount *this;
    Data_Get_Struct(self, OlmAccount, this);
    
    (void)rb_scan_args(argc, argv, "0:", &opts);

    opts = (opts == Qnil) ? rb_hash_new(): opts;
        
    pickle = rb_hash_aref(opts, ID2SYM(rb_intern("pickle")));
    password = rb_hash_aref(opts, ID2SYM(rb_intern("password")));

    if(pickle != Qnil){

        if(rb_obj_is_kind_of(pickle, rb_cString) != Qtrue){
            
            rb_raise(rb_eTypeError, "pickle must be kind of String");
        }
    }
    
    if(password != Qnil){
        
        if(rb_obj_is_kind_of(password, rb_cString) != Qtrue){
            
            rb_raise(rb_eTypeError, "password must be kind of String");
        }
    }
    else{
        
        password = rb_str_new2("");
    }
    
    if(pickle != Qnil){    
        
        if(olm_unpickle_account(this, RSTRING_PTR(password), RSTRING_LEN(password), RSTRING_PTR(dup_string(pickle)), RSTRING_LEN(pickle)) == olm_error()){
            
            raise_olm_error(olm_account_last_error(this));
        }
    }
    else{
        
        size = olm_create_account_random_length(this);
        
        if(olm_create_account(this, RSTRING_PTR(get_random(size)), size) == olm_error()){
            
            raise_olm_error(olm_account_last_error(this));
        }
    }
    
    return self;
}

static VALUE identity_keys(VALUE self)
{
    OlmAccount *this;
    size_t size;
    Data_Get_Struct(self, OlmAccount, this);
    
    size = olm_account_identity_keys_length(this);
    uint8_t buf[size];
    
    if(olm_account_identity_keys(this, buf, size) != size){
        
        raise_olm_error(olm_account_last_error(this));
    }
    
    return rb_funcall(rb_eval_string("JSON"), rb_intern("parse"), 1, rb_str_new((char *)buf, size));    
}

static VALUE generate_one_time_keys(VALUE self, VALUE number)
{
    size_t size;
    OlmAccount *this;
    Data_Get_Struct(self, OlmAccount, this);
    
    size = olm_account_generate_one_time_keys_random_length(this, NUM2SIZET(number));        
    
    if(olm_account_generate_one_time_keys(this, NUM2SIZET(number), RSTRING_PTR(get_random(size)), size) == olm_error()){
        
        raise_olm_error(olm_account_last_error(this));
    }
    
    return self;
}

static VALUE one_time_keys(VALUE self)
{
    VALUE retval;
    size_t size;
    void *ptr;    
    OlmAccount *this;
    Data_Get_Struct(self, OlmAccount, this);
    
    size = olm_account_one_time_keys_length(this);
    
    if((ptr = malloc(size)) == NULL){
        
        rb_raise(rb_eNoMemError, "%s()", __FUNCTION__);
    }
    
    if(olm_account_one_time_keys(this, ptr, size) != size){
        
        free(ptr);
        raise_olm_error(olm_account_last_error(this));
    }
    
    retval = rb_funcall(rb_eval_string("JSON"), rb_intern("parse"), 1, rb_str_new(ptr, size));
    
    free(ptr);
    
    return retval;
}

static VALUE sign(VALUE self, VALUE message)
{
    VALUE retval;
    size_t size;
    void *ptr;    
    OlmAccount *this;
    Data_Get_Struct(self, OlmAccount, this);
    
    size = olm_account_signature_length(this);
    
    if((ptr = malloc(size)) == NULL){
        
        rb_raise(rb_eNoMemError, "%s()", __FUNCTION__);
    }
    
    if(olm_account_sign(this, RSTRING_PTR(message), RSTRING_LEN(message), ptr, size) == olm_error()){
        
        free(ptr);
        raise_olm_error(olm_account_last_error(this));
    }
    
    retval = rb_str_new(ptr, size);
    
    free(ptr);
    
    return retval;
}

static VALUE mark_keys_as_published(VALUE self)
{
    OlmAccount *this;
    Data_Get_Struct(self, OlmAccount, this);
    
    (void)olm_account_mark_keys_as_published(this);
    
    return self; 
}

static VALUE max_number_of_one_time_keys(VALUE self)
{
    OlmAccount *this;
    Data_Get_Struct(self, OlmAccount, this);
    
    return SIZET2NUM(olm_account_max_number_of_one_time_keys(this));
}

static VALUE remove_one_time_keys(VALUE self, VALUE session)
{
    OlmAccount *this;
    Data_Get_Struct(self, OlmAccount, this);
    
    OlmSession *s;
    Data_Get_Struct(session, OlmSession, s);
    
    if(olm_remove_one_time_keys(this, s) == olm_error()){
        
        raise_olm_error(olm_account_last_error(this));
    }
       
    return self;
}

static VALUE outbound_session(VALUE self, VALUE identity_key, VALUE pre_key)
{
    return rb_funcall(rb_eval_string("RubyOlm::OutboundSession"), rb_intern("new"), 3, self, identity_key, pre_key);    
}

static VALUE inbound_session(int argc, VALUE *argv, VALUE self)
{
    VALUE args[] = {self, Qnil, Qnil};
    
    (void)rb_scan_args(argc, argv, "11", &args[1], &args[2]);
    
    return rb_class_new_instance(sizeof(args)/sizeof(*args), args, rb_eval_string("RubyOlm::InboundSession"));
}

static VALUE to_pickle(int argc, VALUE *argv, VALUE self)
{
    VALUE password, retval;
    OlmAccount *this;
    void *ptr;
    size_t size;
    Data_Get_Struct(self, OlmAccount, this);
    
    (void)rb_scan_args(argc, argv, "01", &password);
    
    password = (password == Qnil) ? rb_str_new2("") : password;
   
    size = olm_pickle_account_length(this);
   
    if((ptr = malloc(size)) == NULL){
        
        rb_raise(rb_eNoMemError, "%s()", __FUNCTION__);
    }
    
    if(olm_pickle_account(this, RSTRING_PTR(password), RSTRING_LEN(password), ptr, size) != size){
        
        free(ptr);
        raise_olm_error(olm_account_last_error(this));
    } 
    
    retval = rb_str_new(ptr, size);
    
    free(ptr);
    
    return retval;
}

static void _free(void *ptr)
{    
    olm_clear_account(ptr);
    free(ptr);
}

static VALUE _alloc(VALUE klass)
{
    OlmAccount *this;    
    VALUE self;
    
    self = Data_Wrap_Struct(klass, 0, _free, calloc(1, olm_account_size())); 
    
    Data_Get_Struct(self, OlmAccount, this);   
    
    (void)olm_account((void *)this);
    
    return self;
}

void account_init(void)
{
    VALUE cRubyOLM = rb_define_module("RubyOlm");    
    VALUE cAccount = rb_define_class_under(cRubyOLM, "Account", rb_cObject);
    
    rb_define_alloc_func(cAccount, _alloc);
    
    rb_define_method(cAccount, "initialize", initialize, -1);
    rb_define_method(cAccount, "identity_keys", identity_keys, 0);
    rb_define_method(cAccount, "one_time_keys", one_time_keys, 0);
    rb_define_method(cAccount, "generate_one_time_keys", generate_one_time_keys, 1);
    rb_define_method(cAccount, "last_error", last_error, 0);
    rb_define_method(cAccount, "sign", sign, 1);
    rb_define_method(cAccount, "mark_keys_as_published", mark_keys_as_published, 0);
    rb_define_method(cAccount, "max_number_of_one_time_keys", max_number_of_one_time_keys, 0);
    rb_define_method(cAccount, "remove_one_time_keys", remove_one_time_keys, 1);
    rb_define_method(cAccount, "outbound_session", outbound_session, 2);
    rb_define_method(cAccount, "inbound_session", inbound_session, -1);
    rb_define_method(cAccount, "to_pickle", to_pickle, -1);
}
