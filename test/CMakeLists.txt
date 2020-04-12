find_package(Threads REQUIRED)
include(ExternalProject)

ExternalProject_Add(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.10.0
  CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}/ext"
  UPDATE_COMMAND ""
  )
include_directories("${CMAKE_CURRENT_BINARY_DIR}/ext/include")
link_directories("${CMAKE_CURRENT_BINARY_DIR}/ext/lib")
set(googletest_LIBRARIES gtest gtest_main gmock gmock_main pthread)

function(add_test_executable target)
  add_executable(${ARGV})
  add_dependencies(${target} googletest)
  add_test(${target} ${target})
  target_link_libraries(${target} ${googletest_LIBRARIES} srep)
endfunction()

add_test_executable(insertion_queue_test insertion_queue_test.cc)
add_test_executable(channel_test channel_test.cc)
add_test_executable(barrier_test barrier_test.cc)
add_test_executable(state_test state_test.cc)
add_test_executable(map_state_test map_state_test.cc)
add_test_executable(serializable_state_test serializable_state_test.cc)
add_test_executable(thread_pool_test thread_pool_test.cc)
add_test_executable(http_test http_test.cc)
add_test_executable(peer_distributor_test peer_distributor_test.cc)

add_executable(network_server_test network_server_test.cc)
add_executable(generate_random_bin generate_random_bin.cc)
add_dependencies(network_server_test googletest generate_random_bin)
target_link_libraries(network_server_test ${googletest_LIBRARIES} srep)
add_test(network_server_test "sh"
  ${CMAKE_CURRENT_SOURCE_DIR}/network_server_test.sh
  ${CMAKE_CURRENT_BINARY_DIR}/network_server_test
  ${CMAKE_CURRENT_SOURCE_DIR}/ssl/server.crt)

add_executable(network_client_test network_client_test.cc)
add_dependencies(network_client_test googletest)
target_link_libraries(network_client_test ${googletest_LIBRARIES} srep)
add_test(network_client_test "sh"
  ${CMAKE_CURRENT_SOURCE_DIR}/network_client_test.sh
  ${CMAKE_CURRENT_BINARY_DIR}/network_server_test
  ${CMAKE_CURRENT_BINARY_DIR}/network_client_test)

add_executable(network_client_long_test network_client_long_test.cc)
add_dependencies(network_client_long_test googletest)
target_link_libraries(network_client_long_test ${googletest_LIBRARIES} srep)
add_test(network_client_long_test "sh"
  ${CMAKE_CURRENT_SOURCE_DIR}/network_client_test.sh
  ${CMAKE_CURRENT_BINARY_DIR}/network_server_test
  ${CMAKE_CURRENT_BINARY_DIR}/network_client_long_test)

add_test_executable(remote_https_peer_test remote_https_peer_test.cc)
