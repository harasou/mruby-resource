##
## Resource Test
##

assert("Resource.getrusage") do
  before = Resource.getrusage Resource::RUSAGE_SELF
  1000.times {}
  after = Resource.getrusage Resource::RUSAGE_SELF
  assert_true((after[:ru_utime] - before[:ru_utime]) > 0)
end

assert("Resource::Rusage#ru_utime") do
  before_utime = Resource::Rusage.new(Resource::RUSAGE_SELF).ru_utime
  1000.times {}
  after_utime = Resource::Rusage.new(Resource::RUSAGE_SELF).ru_utime
  assert_true((after_utime - before_utime) > 0)
end

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
# TODO: add other tests
