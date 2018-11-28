module RubyOlm

  class Session
  
    def self.from_pickle(pickle, password="")
      Session.new(pickle, password)
    end
    
  end

end
