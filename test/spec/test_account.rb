require 'minitest/autorun'
require 'ruby_olm'

describe "Account" do

  let(:account){ RubyOlm::Account.new }

  # returns cached one-time-keys which have not yet been marked as published
  #
  describe "#otk" do
  
    let(:rv){ account.otk['curve25519'] }
  
    it("returns a Hash"){ rv.must_be_kind_of Hash }
    
    describe "return value" do
    
      describe "before #gen_otk" do
      
        describe "before #mark_otk" do
        
          it("is empty"){ rv.size.must_equal 0 }
        
        end
        
        describe "after #mark_otk" do
        
          before{ account.mark_otk }
        
          it("is empty"){ rv.size.must_equal 0 }
        
        end
        
      end
    
      describe "after #gen_otk" do
      
        let(:max){ account.max_otk }
        let(:n){ rand(0..max) }
      
        before{ account.gen_otk(n) }
     
        describe "before #mark_otk" do
        
          it("has n keys"){ rv.size.must_equal n }
        
        end
        
        describe "after #mark_otk" do
        
          before{ account.mark_otk }
        
          it("is empty"){ rv.size.must_equal 0 }
        
        end
     
      end
        
    end
  
  end
  
  # creates inbound and outbound sessions
  #
  describe "session factory" do
  
    let(:remote){ RubyOlm::Account.new }
  
    before do
      remote.gen_otk
      account.gen_otk
    end
    
    describe "#outbound_session" do
    
      it("creates session") { account.outbound_session(remote.ik['curve25519'], remote.otk['curve25519'].values.first).must_be_kind_of RubyOlm::Session }
      
    end
    
    describe "#inbound_session" do
      
      let(:remote_session){ remote.outbound_session(account.ik['curve25519'], account.otk['curve25519'].values.first) }
      let(:remote_message){ remote_session.encrypt("hello") }
      
      it("creates session") { account.inbound_session(remote_message).must_be_kind_of RubyOlm::Session }
      
    end
    
    describe "#inbound_session from known remote" do
      
      let(:remote_session){ remote.outbound_session(account.ik['curve25519'], account.otk['curve25519'].values.first) }
      let(:remote_message){ remote_session.encrypt("hello") }
      
      it("creates session") { account.inbound_session(remote_message, remote.ik['curve25519']).must_be_kind_of RubyOlm::Session }
      
    end
    
  end
  
end
