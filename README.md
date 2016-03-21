# mruby-resource   [![Build Status](https://travis-ci.org/harasou/mruby-resource.svg?branch=master)](https://travis-ci.org/harasou/mruby-resource)
Resource class
## install by mrbgems
- add conf.gem line to `build_config.rb`

```ruby
MRuby::Build.new do |conf|

    # ... (snip) ...

    conf.gem :github => 'harasou/mruby-resource'
end
```
## example
```ruby
p Resource.hi
#=> "hi!!"
t = Resource.new "hello"
p t.hello
#=> "hello"
p t.bye
#=> "hello bye"
```

## License
under the MIT License:
- see LICENSE file
