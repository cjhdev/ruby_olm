#ifndef RUBY_OLM_H
#define RUBY_OLM_H

#include <ruby.h>

/* convert error string to exception and raise it */
void raise_olm_error(const char *error);
VALUE get_random(size_t size);

/* necessary to avoid copy-on-write weirdness */
VALUE dup_string(VALUE str);

#endif
