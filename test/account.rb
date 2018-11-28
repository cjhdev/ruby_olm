require 'minitest/autorun'
require 'ruby_olm'

describe "Account" do

  let(:account){ RubyOlm::Account.new }

  # returns curve2599 identity key
  #
  describe "#ik" do
    
    let(:rv){ account.ik }
  
    it("returns a String"){ rv.must_be_instance_of String }

  end

  describe "#identity_keys" do
  
    let(:rv){ account.identity_keys }
  
    it("returns a Hash"){ rv.must_be_instance_of Hash }
  
  end
  
  # returns the maximum number of one-time-keys able to be 
  # cached by account
  #
  describe "#max_otk" do
    
    let(:rv){ account.max_otk }
    
    it("returns an unsigned integer") do
      rv.must_be_kind_of Integer
      rv.must_be :'>=', 0
    end
    
  end
  
  # generates zero or more one-time-keys which will then be cached
  # by account until such time they are:
  #
  # - overwritten by future calls to #generate_otk
  # - removed by session establishment
  #     
  describe "#gen_otk" do
  
    it("returns self"){ account.gen_otk.must_equal account }
  
  end
  
  # marks one-time-keys as published
  #
  describe "#mark_otk" do
  
    it("returns self"){ account.mark_otk.must_equal account }
  
  end
  
  # returns cached one-time-keys which have not yet been marked as published
  #
  describe "#otk" do
  
    let(:rv){ account.otk }
  
    it("returns an Array"){ rv.must_be_kind_of Array }
    
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
    
      it("creates session") { account.outbound_session(remote.ik, remote.otk.first).must_be_kind_of RubyOlm::Session }
      
    end
    
    describe "#inbound_session" do
      
      let(:remote_session){ remote.outbound_session(account.ik, account.otk.first) }
      let(:remote_message){ remote_session.encrypt("hello") }
      
      it("creates session") { account.inbound_session(remote_message).must_be_kind_of RubyOlm::Session }
      
    end
    
    describe "#inbound_session from known remote" do
      
      let(:remote_session){ remote.outbound_session(account.ik, account.otk.first) }
      let(:remote_message){ remote_session.encrypt("hello") }
      
      it("creates session") { account.inbound_session(remote_message, remote.ik).must_be_kind_of RubyOlm::Session }
      
    end
    
  end
  
end
