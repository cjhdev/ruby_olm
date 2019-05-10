require 'mkmf'

dir_config('olm')

abort "Missing olm, or olm too old (need at least 2.3.0)" unless have_library("olm", "olm_encrypt")

create_makefile('ruby_olm')
