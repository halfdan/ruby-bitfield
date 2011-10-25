require 'rake/extensiontask'

spec = Gem::Specification.new do |s|
  s.name              = 'bitfield'
  s.platform          = Gem::Platform::RUBY
  s.extensions        = FileList["ext/**/extconf.rb"]
  s.version           = '0.9.1'
  s.date              = "2011-10-25"
  s.author            = 'Fabian Becker'
  s.email             = 'halfdan@xnorfz.de'
  s.homepage          = 'https://github.com/halfdan/ruby-bitfield/'
  s.summary           = "Wraps boost::dynamic_bitset and makes it available as a Ruby class 
                         for fast operations on a bitset"

  s.description       = 'This C extensions wraps boost::dynamic_bitset and makes it available
                         as a native Ruby class. The bitset behaves like an an array allowing
                         only values of 0 and 1.'
  
  s.files = FileList["LICENSE", "README.rdoc", "Rakefile", "ext/**/*.cpp", "ext/**/*.hpp"]
end

# add your default gem packing task
Gem::PackageTask.new(spec) do |pkg|
end

Rake::ExtensionTask.new('bitfield', spec)
