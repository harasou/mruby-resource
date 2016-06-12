# mruby-resource   [![Build Status](https://travis-ci.org/harasou/mruby-resource.svg?branch=master)](https://travis-ci.org/harasou/mruby-resource)

mruby-resouce is the binding of setrlimit(2) and getrlimit(2), getrusage(2) system call

## install by mrbgems
- add conf.gem line to `build_config.rb`

```ruby
MRuby::Build.new do |conf|

    # ... (snip) ...

    conf.gem :github => 'harasou/mruby-resource'
end
```

## example

- setrlimit

    ```ruby
    include Resource
    lim = setrlimit(RLIMIT_CPU,5)  #=> nil
    ```

- getrlimit

    ```ruby
    include Resource
    lim = getrlimit(RLIMIT_STACK)                                       #=> [8388608, 1.844674407371e+19]
    lim.map{|i| i == RLIM_INFINITY ? "unlimited" : "#{i/(1024**2)}MB" } #=> ["8MB", "unlimited"]
    ```

- getrusage(resource)

    ```ruby
    include Resource
    ru = getrusage(RUSAGE_SELF)      #=> {:ru_utime=>0.008506, :ru_stime=>0.008506, :ru_maxrss=>2512, :ru_ixrss=>0, :ru_idrss=>0, :ru_isrss=>0, :ru_minflt=>466, :ru_majflt=>0, :ru_nswap=>0, :ru_inblock=>0, :ru_oublock=>0, :ru_msgsnd=>0, :ru_msgrcv=>0, :ru_nsignals=>0, :ru_nvcsw=>4, :ru_nivcsw=>9}
    ```

## License
under the MIT License:

- see [LICENSE](LICENSE) file
