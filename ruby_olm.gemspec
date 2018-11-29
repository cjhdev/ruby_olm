require File.expand_path("../lib/ruby_olm/version", __FILE__)
require 'time'

Gem::Specification.new do |s|
    s.name    = "ruby_olm"
    s.version = RubyOlm::VERSION
    s.date = Date.today.to_s
    s.summary = "Olm for Ruby"
    s.author  = "Cameron Harper"
    s.email = "contact@cjh.id.au"
    s.homepage = "https://github.com/cjhdev/ruby_olm"
    s.files = Dir.glob("ext/**/*.{cpp,hh,c,h,rb}") + Dir.glob("lib/**/*.rb") + Dir.glob("test/**/*.rb") + ["rakefile"]
    s.extensions = ["ext/ruby_olm/ext_lib_olm/extconf.rb"]
    s.license = 'Apache-2.0'
    s.test_files = Dir.glob("test/**/*.rb")
    s.add_development_dependency 'rake-compiler', '~> 0'
    s.add_development_dependency 'rake', '~> 0'
    s.add_development_dependency 'minitest', '~> 0'
    s.required_ruby_version = '>= 2.0'
end
