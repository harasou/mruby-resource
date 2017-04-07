MRuby::Gem::Specification.new('mruby-resource') do |spec|
  spec.license = 'MIT'
  spec.authors = 'SOGO Haraguchi'

  spec.add_test_dependency 'mruby-process'
  spec.add_test_dependency 'mruby-exec', github: 'haconiwa/mruby-exec'
end
