#include "olm/olm.h"
#include "ruby_olm.h"

static VALUE last_error(VALUE self)
{
    OlmSession *this;
    Data_Get_Struct(self, OlmSession, this);
    
    return rb_str_new2(olm_session_last_error(this));
}
        
static VALUE initialize(int argc, VALUE *argv, VALUE self)
{
    VALUE pickle, password;
    OlmSession *this;
    Data_Get_Struct(self, OlmSession, this);
    
    (void)rb_scan_args(argc, argv, "11", &pickle, &password);

    if(rb_obj_is_kind_of(pickle, rb_cString) != Qtrue){
        
        rb_raise(rb_eTypeError, "pickle must be kind of String");
    }
    
    if(password != Qnil){
        
        if(rb_obj_is_kind_of(password, rb_cString) != Qtrue){
            
            rb_raise(rb_eTypeError, "password must be kind of String");
        }
    }
    else{
        
        password = rb_str_new2("");
    }

    if(olm_unpickle_session(this, RSTRING_PTR(password), RSTRING_LEN(password), RSTRING_PTR(dup_string(pickle)), RSTRING_LEN(pickle)) == olm_error()){
    
        raise_olm_error(olm_session_last_error(this));     
    }

    return self;
}

static VALUE initialize_outbound(VALUE self, VALUE account, VALUE identity, VALUE one_time_key)
{    
    size_t size;
    OlmSession *this;
    OlmAccount *a;
    
    Data_Get_Struct(self, OlmSession, this);
    Data_Get_Struct(account, OlmAccount, a);
 
    size = olm_create_outbound_session_random_length(this);
    
    if(rb_obj_is_instance_of(account, rb_eval_string("RubyOlm::Account")) != Qtrue){
    
        rb_raise(rb_eTypeError, "account must be an instance of RubyOlm::Account");
    }
    if(rb_obj_is_kind_of(identity, rb_eval_string("String")) != Qtrue){
    
        rb_raise(rb_eTypeError, "identity must be kind of String");
    }
    if(rb_obj_is_kind_of(one_time_key, rb_eval_string("String")) != Qtrue){
    
        rb_raise(rb_eTypeError, "one_time_key must be kind of String");
    }
    
    if(olm_create_outbound_session(this, a,
            RSTRING_PTR(identity), RSTRING_LEN(identity),
            RSTRING_PTR(one_time_key), RSTRING_LEN(one_time_key),
            RSTRING_PTR(get_random(size)), size
            ) == olm_error()){
        raise_olm_error(olm_session_last_error(this));
    }
    
    return self;
}

static VALUE initialize_inbound(int argc, VALUE *argv, VALUE self)
{    
    VALUE account, one_time_message, identity;
    
    identity = Qnil;
    
    (void)rb_scan_args(argc, argv, "21", &account, &one_time_message, &identity);
    
    OlmSession *this;
    Data_Get_Struct(self, OlmSession, this);
    
    OlmAccount *a;
    Data_Get_Struct(account, OlmAccount, a);
    
    if(rb_obj_is_kind_of(one_time_message, rb_eval_string("RubyOlm::PreKeyMessage")) != Qtrue){
        
        rb_raise(rb_eTypeError, "one_time_message must be kind of PreKeyMessage");
    }
    
    one_time_message = rb_funcall(one_time_message, rb_intern("to_s"), 0);
    
    if(identity == Qnil){

        if(olm_create_inbound_session(this, a,
                RSTRING_PTR(dup_string(one_time_message)), RSTRING_LEN(one_time_message)
                ) == olm_error()){
            raise_olm_error(olm_session_last_error(this));
        }
    }
    else{
        
        if(olm_create_inbound_session_from(this, a,
                RSTRING_PTR(identity), RSTRING_LEN(identity),
                RSTRING_PTR(dup_string(one_time_message)), RSTRING_LEN(one_time_message)
                ) == olm_error()){
            raise_olm_error(olm_session_last_error(this));
        }
    }
    
    return self;
}

static VALUE has_received_message(VALUE self)
{
    OlmSession *this;
    Data_Get_Struct(self, OlmSession, this);
    
    return (olm_session_has_received_message(this) == 0) ? Qfalse : Qtrue;
}

static VALUE get_session_id(VALUE self)
{
    OlmSession *this;
    Data_Get_Struct(self, OlmSession, this);
    
    size_t size = olm_session_id_length(this);
    uint8_t buf[size];
    
    if(olm_session_id(this, buf, size) != size){
        
        raise_olm_error(olm_session_last_error(this));
    }
    
    return rb_str_new((char *)buf, size);    
}

static VALUE will_receive(int argc, VALUE *argv, VALUE self)
{
    VALUE one_time_message, identity;
    size_t result;
    OlmSession *this;
    Data_Get_Struct(self, OlmSession, this);

    identity = Qnil;

    (void)rb_scan_args(argc, argv, "11", &one_time_message, &identity);

    if(rb_obj_is_kind_of(one_time_message, rb_eval_string("RubyOlm::PreKeyMessage")) != Qtrue){
        
        rb_raise(rb_eTypeError, "one_time_message must be kind of PreKeyMessage");
    }

    one_time_message = rb_funcall(one_time_message, rb_intern("to_s"), 0);

    if(identity == Qnil){
    
        result = olm_matches_inbound_session(this, 
                RSTRING_PTR(dup_string(one_time_message)), RSTRING_LEN(one_time_message)
        );
    }
    else{
        
        result = olm_matches_inbound_session_from(this, 
                RSTRING_PTR(identity), RSTRING_LEN(identity),
                RSTRING_PTR(dup_string(one_time_message)), RSTRING_LEN(one_time_message)
        );
    }
    
    if(result == olm_error()){
        
        raise_olm_error(olm_session_last_error(this));
    }
    
    return (result == 1) ? Qtrue : Qfalse;
}

static VALUE message_type(VALUE self)
{
    OlmSession *this;
    VALUE retval;
    Data_Get_Struct(self, OlmSession, this);
    
    if(olm_encrypt_message_type(this) == OLM_MESSAGE_TYPE_PRE_KEY){
        
        retval = rb_eval_string("RubyOlm::PreKeyMessage");
    }
    else if(olm_encrypt_message_type(this) == OLM_MESSAGE_TYPE_MESSAGE){
        
        retval = rb_eval_string("RubyOlm::Message");
    }
    else{
        
        rb_bug("olm_encrypt_message_type()");
    }
    
    return retval;
}

static VALUE encrypt(VALUE self, VALUE plain)
{
    size_t cipher_size, random_size;
    void *ptr;
    OlmSession *this;
    VALUE retval, type;
    Data_Get_Struct(self, OlmSession, this);
    
    cipher_size = olm_encrypt_message_length(this, RSTRING_LEN(plain));
    random_size = olm_encrypt_random_length(this);
    
    if((ptr = malloc(cipher_size)) == NULL){
        
        rb_raise(rb_eNoMemError, "%s()", __FUNCTION__);
    }
    
    type = message_type(self);
    
    if(olm_encrypt(this, RSTRING_PTR(plain), RSTRING_LEN(plain),
            RSTRING_PTR(get_random(random_size)), random_size,
            ptr, cipher_size
            ) == olm_error()){
        free(ptr);
        raise_olm_error(olm_session_last_error(this));
    }
    
    retval = rb_funcall(type, rb_intern("new"), 1, rb_str_new(ptr, cipher_size));
    
    free(ptr);
    
    return retval;
}

static VALUE decrypt(VALUE self, VALUE cipher)
{
    size_t plain_size, plain_max, type;
    void *ptr;
    OlmSession *this;
    VALUE retval, data;
    Data_Get_Struct(self, OlmSession, this);
    
    if(rb_obj_is_kind_of(cipher, rb_eval_string("RubyOlm::Message")) == Qtrue){
        
        type = OLM_MESSAGE_TYPE_MESSAGE;
    }
    else if(rb_obj_is_kind_of(cipher, rb_eval_string("RubyOlm::PreKeyMessage")) == Qtrue){
        
        type = OLM_MESSAGE_TYPE_PRE_KEY;
    }
    else{
        
        rb_raise(rb_eTypeError, "cipher must be kind of Message or PreKeyMessage");
    }
    
    data = rb_funcall(cipher, rb_intern("to_s"), 0);
    
    if((plain_max = olm_decrypt_max_plaintext_length(this, type, RSTRING_PTR(dup_string(data)), RSTRING_LEN(data))) == olm_error()){
        
        raise_olm_error(olm_session_last_error(this));
    } 
    
    /* size of output will be less than size of input */
    if((ptr = malloc(plain_max)) == NULL){
        
        rb_raise(rb_eNoMemError, "%s()", __FUNCTION__);
    }
    
    if((plain_size = olm_decrypt(this, 
            type,
            RSTRING_PTR(dup_string(data)), RSTRING_LEN(data),  
            ptr, plain_max)) == olm_error())
    {
        free(ptr);
        raise_olm_error(olm_session_last_error(this));
    }
    
    retval = rb_str_new(ptr, plain_size);
    
    free(ptr);
    
    return retval;
}

static VALUE to_pickle(int argc, VALUE *argv, VALUE self)
{
    VALUE password, retval;
    OlmSession *this;
    void *ptr;
    size_t size;
    Data_Get_Struct(self, OlmSession, this);
    
    (void)rb_scan_args(argc, argv, "01", &password);
    
    password = (password == Qnil) ? rb_str_new2("") : password;
   
    size = olm_pickle_session_length(this);
   
    if((ptr = malloc(size)) == NULL){
        
        rb_raise(rb_eNoMemError, "%s()", __FUNCTION__);
    }
    
    if(olm_pickle_session(this, RSTRING_PTR(password), RSTRING_LEN(password), ptr, size) != size){
        
        free(ptr);
        raise_olm_error(olm_session_last_error(this));
    } 
    
    retval = rb_str_new(ptr, size);
    
    free(ptr);
    
    return retval;
}

static void _free(void *ptr)
{    
    olm_clear_session(ptr);
    free(ptr);
}

static VALUE _alloc(VALUE klass)
{
    OlmSession *this;    
    VALUE self;
    
    self = Data_Wrap_Struct(klass, 0, _free, calloc(1, olm_session_size())); 
    
    Data_Get_Struct(self, OlmSession, this);   
    
    (void)olm_session((void *)this);
    
    return self;
}

void session_init(void)
{
    VALUE cRubyOLM = rb_define_module("RubyOlm");    
    VALUE cSession = rb_define_class_under(cRubyOLM, "Session", rb_cObject);
    VALUE cSessionOut = rb_define_class_under(cRubyOLM, "OutboundSession", cSession);
    VALUE cSessionIn = rb_define_class_under(cRubyOLM, "InboundSession", cSession);
    
    rb_define_alloc_func(cSession, _alloc);
    
    rb_define_method(cSessionOut, "initialize", initialize_outbound, 3);    
    rb_define_method(cSessionIn, "initialize", initialize_inbound, -1);    
    
    rb_define_method(cSession, "initialize", initialize, -1);    
    rb_define_method(cSession, "id", get_session_id, 0);    
    rb_define_method(cSession, "last_error", last_error, 0);    
    rb_define_method(cSession, "has_received_message", has_received_message, 0);    
    rb_define_method(cSession, "encrypt", encrypt, 1);    
    rb_define_method(cSession, "decrypt", decrypt, 1);
    rb_define_method(cSession, "to_pickle", to_pickle, -1);
    rb_define_method(cSession, "will_receive?", will_receive, -1);
}
