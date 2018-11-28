require 'mkmf'
require 'fileutils'

# there is probably a better way to do this
#
# Originally I configured vpath to find the sources in the original
# location but mkmf was building other sources in the library dir
# that had the same name as the objects in the src directory causing
# confusing issues where symbols were undefined.
#
# Solution eventually was to copy the sources I wanted to a staging
# dir so mkmf wont get confused.

staging = File.join(__dir__, "staging")

FileUtils::rm_f(File.join(staging, "*"))

srcs = %w[
  src/account.cpp
  src/base64.cpp
  src/cipher.cpp
  src/crypto.cpp
  src/memory.cpp
  src/message.cpp
  src/pickle.cpp
  src/ratchet.cpp
  src/session.cpp
  src/utility.cpp
  src/pk.cpp
  src/ed25519.c
  src/error.c
  src/inbound_group_session.c
  src/megolm.c
  src/olm.cpp
  src/outbound_group_session.c
  src/pickle_encoding.c
  lib/crypto-algorithms/aes.c
  lib/crypto-algorithms/sha256.c
  lib/curve25519-donna/curve25519-donna.c
].map{|f|File.join(__dir__, "olm", f)}

srcs.each do |f|
  FileUtils::cp(f, staging)
end

$VPATH << staging

$srcs = (Dir[File.join(__dir__, "*.c")] + srcs).map{|f|File.basename(f)}

%w[
  .
  olm/include
  olm/lib
  olm/lib/crypto-algorithms
].each do |dir|
  $INCFLAGS << " -I#{File.join(__dir__, dir)}"    
end

$CXXFLAGS += " -std=c++11 -fPIC"
$CFLAGS += " -std=c99 -fPIC"

# fixme: pattern match these in olm/common.mk
# or is there some way to have mkmf include into the makefile?
$defs << "-DOLMLIB_VERSION_MAJOR=3"
$defs << "-DOLMLIB_VERSION_MINOR=0"
$defs << "-DOLMLIB_VERSION_PATCH=0"
    
create_makefile('ruby_olm/ext_lib_olm')

