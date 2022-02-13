# mruby-cm6206   [![build](https://github.com/yamori813/mruby-cm6206/actions/workflows/ci.yml/badge.svg)](https://github.com/yamori813/mruby-cm6206/actions/workflows/ci.yml)
CM6206 class
## install by mrbgems
- add conf.gem line to `build_config.rb`

```ruby
MRuby::Build.new do |conf|

    # ... (snip) ...

    conf.gem :github => 'yamori813/mruby-cm6206'
end
```
## example
```ruby
t = CM6206.new
p t.open
p t.read 0
#=> 8192
```

## License
under the BSD License:
- see LICENSE file
