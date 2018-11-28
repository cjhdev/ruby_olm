module RubyOlm

  class Account

    # @return [Account]
    def self.from_pickle(pickle, password="")
      Account.new(pickle: pickle, password: password)
    end
    
    # @param [type] defaults to 'curve25519' 
    # @return [String]
    def ik(type='curve25519')
      raise RangeError unless identity_keys[type]
      identity_keys[type]
    end
    
    # @param [type] defaults to 'curve25519'
    # @param [value] return only the key values and not the ids 
    #
    # @return [Array<Hash>] if !values
    # @return [Array<String>] if values
    def otk(values=true, type='curve25519')
      if values
        one_time_keys[type].values
      else
        one_time_keys[type]
      end
    end
    
    def gen_otk(number=1)
      generate_one_time_keys(number)
    end
    
    alias_method :mark_otk, :mark_keys_as_published
    alias_method :max_otk, :max_number_of_one_time_keys
    alias_method :update_otk, :remove_one_time_keys
    
  end

end
