MRuby::Gem::Specification.new('mruby-cm6206') do |spec|
  spec.license = 'BSD'
  spec.authors = 'Hiroki Mori'

  if (`uname`.chomp =~ /freebsd/i)
    spec.linker.libraries << ['usb']
  else
    spec.linker.libraries << ['usb-1.0']
  end
end
