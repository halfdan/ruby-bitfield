require 'rake/extensiontask'

spec = Gem::Specification.new do |spec|
  spec.name              = 'bitfield'
  spec.platform          = Gem::Platform::RUBY
  spec.extensions        = FileList["ext/**/extconf.rb"]
  spec.version           = '0.9.3'
  spec.date              = Time.now.strftime "%Y-%m-%d"
  spec.author            = 'Fabian Becker'
  spec.email             = 'halfdan@xnorfz.de'
  spec.homepage          = 'https://github.com/halfdan/ruby-bitfield/'
  spec.summary           = "Wraps boost::dynamic_bitset and makes it available as a Ruby class 
                         for fast operations on a bitset"

  spec.description       = 'This C extension wraps boost::dynamic_bitset and makes it available
                         as a native Ruby class. The bitset behaves like an an array allowing
                         only values of 0 and 1.'

  spec.required_ruby_version = '>= 1.8.1'

  spec.files = FileList["LICENSE", "README.rdoc", "Rakefile", "ext/**/*.cpp", "ext/**/*.hpp", "spec/*"]

  spec.add_development_dependency 'rspec'
  spec.test_files = ["spec/bitfield_spec.rb"]

  spec.extra_rdoc_files = %w( README.rdoc ext/bitfield/bitfield.cpp )
  spec.rdoc_options.concat ['--main','README.rdoc', '-E cpp=c']
end

# add your default gem packing task
Gem::PackageTask.new(spec) do |pkg|
end

Rake::ExtensionTask.new('bitfield', spec)
