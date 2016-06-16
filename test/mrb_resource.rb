##
## Resource Test
##

assert("Resource.getrusage") do
  before = Resource.getrusage Resource::RUSAGE_SELF
  1000.times {}
  after = Resource.getrusage Resource::RUSAGE_SELF
  assert_true((after[:ru_utime] - before[:ru_utime]) > 0)
end

# TODO: add other tests
