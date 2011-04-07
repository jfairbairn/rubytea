Gem::Specification.new do |s|
  s.name = 'rubytea'
  s.version = '0.1.2'
  s.files = `git ls-files`.split("\n")
  s.extensions = ["ext/extconf.rb"]
  s.author = 'James Fairbairn'
  s.email = 'james@netlagoon.com'
  s.homepage = 'https://github.com/jfairbairn/rubytea'

  s.require_paths = %w(ext)
  s.summary = "A wrapper around the XXTEA block encryption algorithm"

  s.add_development_dependency 'rake-compiler', '~> 0.7.6'

end
