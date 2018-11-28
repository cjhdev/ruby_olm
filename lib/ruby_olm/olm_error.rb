module RubyOlm

  module OlmError

    class SUCCESS < StandardError
    end
  
    class NOT_ENOUGH_RANDOM < StandardError
    end
    
    class OUTPUT_BUFFER_TOO_SMALL < StandardError
    end
    
    class BAD_MESSAGE_VERSION < StandardError
    end
    
    class BAD_MESSAGE_FORMAT < StandardError
    end
    
    class BAD_MESSAGE_MAC < StandardError
    end
    
    class BAD_MESSAGE_KEY_ID < StandardError
    end
    
    class INVALID_BASE64 < StandardError
    end
    
    class BAD_ACCOUNT_KEY < StandardError
    end
    
    class UNKNOWN_PICKLE_VERSION < StandardError
    end
    
    class CORRUPTED_PICKLE < StandardError
    end
    
    class BAD_SESSION_KEY < StandardError
    end
    
    class UNKNOWN_MESSAGE_INDEX < StandardError
    end
    
    class BAD_LEGACY_ACCOUNT_PICKLE < StandardError
    end
    
    class BAD_SIGNATURE < StandardError
    end
    
    class OLM_INPUT_BUFFER_TOO_SMALL < StandardError
    end
    
    class UnknownError < StandardError
    end

    def self.from_string(str)
      begin
        OlmError.const_get(str)
      rescue NameError
        UnknownError.new str
      end
    end

    def self.raise_from_string(str)
      raise from_string(str)
    end
  
  end
  
end
