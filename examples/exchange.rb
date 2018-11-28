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

puts bob_msg
puts alice_msg

alice_saved_account = alice.to_pickle
alice_saved_session = alice_session.to_pickle

puts alice_saved_account
puts alice_saved_session

Account.from_pickle(alice_saved_account)
Session.from_pickle(alice_saved_session)
