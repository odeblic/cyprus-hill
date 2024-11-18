include_directories(utils)

add_executable(a a.cpp)
add_executable(b b.cpp)
add_executable(server server.cpp)

# TODO: implement tests
add_test(NAME demo COMMAND bash -c "server 7777 & b 5555 7777 & a messages.txt")
