##
## Resource Test
##

assert("Resource::Rusage#ru_utime vs Resource.getrusage") do
  a = Resource.getrusage(Resource::RUSAGE_SELF)[:ru_utime]
  100000.times { Resource.getrusage(Resource::RUSAGE_SELF)[:ru_utime] }
  b = Resource.getrusage(Resource::RUSAGE_SELF)[:ru_utime] 
  c = b - a

  a = Resource.getrusage(Resource::RUSAGE_SELF)[:ru_utime]
  100000.times { Resource::Rusage.new(Resource::RUSAGE_SELF).ru_utime }
  b = Resource::Rusage.new(Resource::RUSAGE_SELF).ru_utime
  d = b - a

  assert_true(d < c)
end
