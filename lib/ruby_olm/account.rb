module RubyOlm

  class Account

    # @param pickle [String] pickled state
    # @param password [String] password used to encrypt pickled state
    # @return [Account]
    def self.from_pickle(pickle, password="")
      Account.new(pickle: pickle, password: password)
    end
    
    def gen_otk(number=1)
      generate_one_time_keys(number)
    end
    
    alias_method :ik,       :identity_keys
    alias_method :otk,      :one_time_keys
    alias_method :mark_otk, :mark_keys_as_published
    alias_method :max_otk,    :max_number_of_one_time_keys
    alias_method :update_otk, :remove_one_time_keys
    
  end

end
