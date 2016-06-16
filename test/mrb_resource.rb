##
## Resource Test
##

assert("Resource.getrusage") do
  before = Resource.getrusage Resource::RUSAGE_SELF
  10000.times {}
  after = Resource.getrusage Resource::RUSAGE_SELF
  assert_true((after[:ru_utime] - before[:ru_utime]) > 0)
end

assert("Resource::Rusage#ru_utime") do
  before_utime = Resource::Rusage.new(Resource::RUSAGE_SELF).ru_utime
  10000.times {}
  after_utime = Resource::Rusage.new(Resource::RUSAGE_SELF).ru_utime
  assert_true((after_utime - before_utime) > 0)
end

# TODO: add other tests
