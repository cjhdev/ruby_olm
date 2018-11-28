require 'minitest/autorun'
require 'ruby_olm'

class TestExchange < Minitest::Test

  include RubyOlm
  
  def test_exchange

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

    # Bob can now update his list of marked otk (since he knows one has been used)
    bob.update_otk(bob_session)

    # Bob can decrypt Alice's message
    bob_msg = bob_session.decrypt(encrypted)

    assert_equal alice_msg, bob_msg
  
  end

end
