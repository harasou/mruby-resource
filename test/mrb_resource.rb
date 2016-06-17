##
## Resource Test
##

assert("Resource.getrusage") do
  assert_true(Resource.getrusage(Resource::RUSAGE_SELF)[:ru_utime] > 0)
end

assert("Resource::Rusage#ru_utime") do
  assert_true(Resource::Rusage.new(Resource::RUSAGE_SELF).ru_utime > 0)
end

# TODO: add other tests
