set(ROOT_HEADERS
  srep.hh
  )

set(HEADERS
  barrier.hh
  barrier_impl.hh
  channel.hh
  channel_impl.hh
  distributor.hh
  distributor_impl.hh
  http.hh
  http_impl.hh
  insertion_queue.hh
  insertion_queue_impl.hh
  io_exchange.hh
  io_exchange_impl.hh
  jsonable_state_update.hh
  map_state.hh
  receiver.hh
  receiver_impl.hh
  serializable_state.hh
  serializable_state_impl.hh
  state.hh
  state_impl.hh
  thread_pool.hh
  thread_pool_impl.hh
  transmitter.hh
  type_assertions.hh
  )

set(SRC
  ${HEADERS}
  barrier.cc
  http.cc
  map_state.cc
  receiver.cc
  thread_pool.cc
  transmitter.cc
  )

add_library(srep
  ${SRC}
  )

target_link_libraries(srep PRIVATE nlohmann_json::nlohmann_json Boost::system OpenSSL::SSL)

install(FILES ${ROOT_HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
install(FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/srep)
install(TARGETS srep DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
