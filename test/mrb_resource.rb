##
## Resource Test
##

assert("Resource.getrlimit") do
  s, h = Resource.getrlimit(Resource::RLIMIT_NOFILE)
  assert_true s >= 0
  assert_true h >= s
end

assert("Resource.setrlimit") do
  s, h = Resource.getrlimit(Resource::RLIMIT_NOFILE)
  s -= 1
  h -= 1

  Resource.setrlimit(Resource::RLIMIT_NOFILE, s, h)
  s2, h2 = Resource.getrlimit(Resource::RLIMIT_NOFILE)

  assert_equal s, s2
  assert_equal h, h2
end

assert("Resource.getprlimit") do
  pid = fork do
    exec "/bin/bash", "-c", "sleep 60"
  end

  s, h = Resource.getprlimit(pid, Resource::RLIMIT_NOFILE)
  assert_true s >= 0
  assert_true h >= s
  Process.kill :TERM, pid
end

assert("Resource.setprlimit") do
  pid = fork do
    exec "/bin/bash", "-c", "sleep 60"
  end

  s, h = Resource.getprlimit(pid, Resource::RLIMIT_NOFILE)
  s -= 1
  h -= 1

  Resource.setprlimit(pid, Resource::RLIMIT_NOFILE, s, h)
  s2, h2 = Resource.getprlimit(pid, Resource::RLIMIT_NOFILE)

  assert_equal s, s2
  assert_equal h, h2
  Process.kill :TERM, pid
end

assert("Resource.getrusage") do
  assert_true(Resource.getrusage(Resource::RUSAGE_SELF)[:ru_utime] > 0)
end

assert("Resource::Rusage#ru_utime") do
  assert_true(Resource::Rusage.new(Resource::RUSAGE_SELF).ru_utime > 0)
end

# TODO: add other tests
