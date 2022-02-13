##
## CM6206 Test
##

assert("CM6206#hello") do
  t = CM6206.new "hello"
  assert_equal("hello", t.hello)
end

assert("CM6206#bye") do
  t = CM6206.new "hello"
  assert_equal("hello bye", t.bye)
end

assert("CM6206.hi") do
  assert_equal("hi!!", CM6206.hi)
end
