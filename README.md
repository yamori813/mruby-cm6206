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
#!/usr/local/bin/mruby

opt = ARGV[0]

para = -1
if opt == "44.1" then
  para = 0
elsif opt == "48" then
  para = 2
elsif opt == "32" then
  para = 3
elsif opt == "96" then
  para = 6
end

if para != -1 then
  t = CM6206.new
  t.open
  t.write 0, para << 12
#  p t.read 0
else
  p "sr.rb [44.1,48,32,96]"
end
```

## License
under the BSD License:
- see LICENSE file
