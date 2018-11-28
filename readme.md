ruby_olm: Olm for Ruby
======================

A Ruby wrapper for the [olm](https://git.matrix.org/git/olm/) 
double ratchet implementation from [matrix](https://matrix.org/blog/home/).

This wrapper provides classes and methods that line up with
the [olm](ext/ruby_olm/ext_lib_olm/olm/include/olm/olm.h) interface, as
well as a set of helpers and shorter aliases.

Very much a work in progress.

[![Build Status](https://travis-ci.org/cjhdev/ruby_olm.svg?branch=master)](https://travis-ci.org/cjhdev/ruby_olm)


## Installation

The gem packages and builds olm source for your convenience. This means:

- Your system needs to be capable of building native extensions
- You don't need to install olm separately

Using bundle: 

~~~ console
bundle install
~~~ 

Then require as:

~~~ ruby
require 'ruby_olm'
~~~

## Example

Alice wants to send a message to Bob:

~~~ ruby
require 'ruby_olm'

include RubyOlm

alice = Account.new
bob = Account.new

# Alice wants to send a message to Bob
alice_msg = "hello world"

# Bob generates a one-time-key
bob.gen_otk

# Alice must have Bob's identity and one-time-key to make a session
alice_session = alice.outbound_session(bob.ik, bob.otk.first)

# Bob marks all one-time-keys as published
bob.mark_otk

# Alice can encrypt
encrypted = alice_session.encrypt(alice_msg)

# Bob can create a session from this first message
bob_session = bob.inbound_session(encrypted)

# Bob can now update his list of marked otk
bob.update_otk(bob_session)

# Bob can decrypt Alice's message
bob_msg = bob_session.decrypt(encrypted)
~~~

Account and Session instances can be serialised and deserialised 
using the `#to_pickle` and `::from_pickle` methods. This is handy
for saving and restoring state: 

~~~ ruby
# save
alice_saved_account = Alice.to_pickle
alice_saved_session = alice_session.to_pickle

# restore
Account.from_pickle(alice_saved_account)
Session.from_pickle(alice_saved_session)
~~~

Finally, be aware that olm produces and consumes base64 encoded byte strings
instead of raw byte strings.

## Running Tests

~~~ console
bundle exec rake test
~~~

## Todo

- documentation
- more testing
- add support for megolm and utility functions
- replace built-in olm crypto with Ruby openssl

## What is an Olm?

[https://en.wikipedia.org/wiki/Olm](https://en.wikipedia.org/wiki/Olm).

## License

Apache 2.0
