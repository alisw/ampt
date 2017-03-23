cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

project(AMPT)
enable_language(Fortran)

set (BINS ampt)

add_executable(ampt
  main.f
  amptsub.f
  art1f.f
  hijing1.383_ampt.f
  hipyset1.35.f
  zpc.f
  linana.f
  )

install(TARGETS ${BINS}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  )
