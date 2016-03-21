##
## Resource Test
##

assert("Resource#hello") do
  t = Resource.new "hello"
  assert_equal("hello", t.hello)
end

assert("Resource#bye") do
  t = Resource.new "hello"
  assert_equal("hello bye", t.bye)
end

assert("Resource.hi") do
  assert_equal("hi!!", Resource.hi)
end
