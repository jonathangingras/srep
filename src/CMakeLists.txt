set(ROOT_HEADERS
  srep.hh
  )

set(HEADERS
  barrier.hh
  barrier_impl.hh
  channel.hh
  channel_impl.hh
  insertion_queue.hh
  insertion_queue_impl.hh
  io_exchange.hh
  io_exchange_impl.hh
  map_state.hh
  serializable_state.hh
  serializable_state_impl.hh
  state.hh
  state_impl.hh
  )

set(SRC
  ${HEADERS}
  barrier.cc
  map_state.cc
  )

add_library(srep
  ${SRC}
  )

target_link_libraries(srep PRIVATE nlohmann_json::nlohmann_json)

install(FILES ${ROOT_HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
install(FILES ${HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/srep)
install(TARGETS srep DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
