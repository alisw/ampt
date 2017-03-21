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

if(HEPMC_ROOT)
  link_directories(${HEPMC_ROOT}/lib)
  add_executable(parser
    AmptToHepMCParserMain.cpp
    AmptToHepMCParser.cpp
    )
  target_include_directories(parser PUBLIC ${HEPMC_ROOT}/include)
  target_link_libraries(parser HepMC)

  set (BINS ${BINS} parser)
endif()

install(TARGETS ${BINS}
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  )
