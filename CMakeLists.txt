cmake_minimum_required(VERSION 3.6)
project( elfelli CXX )

option(ELFELLI_PROFILING "enable profiling" OFF)

include(FindEXPAT)
include(FindGettext)
include(FindPkgConfig)
include(GNUInstallDirs)

pkg_check_modules(GTKMM REQUIRED gtkmm-2.4>=2.8 librsvg-2.0)

set (CMAKE_CXX_STANDARD 11)

add_compile_options(
  "-Wall" "-Wpedantic" "-Wextra"
  )
include_directories(${GTKMM_INCLUDE_DIRS})
link_directories(${GTKMM_LIBRARY_DIRS})
add_executable( elfelli
  src/Application.cpp
  src/Canvas.cpp
  src/Main.cpp
  src/Simulation.cpp
  src/SimulationCanvas.cpp
  src/Toolbox.cpp
  src/XmlLoader.cpp
  src/XmlWriter.cpp
  )

set(APP_DATADIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/elfelli")
set(APP_LOCALEDIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LOCALEDIR}")
add_compile_definitions(DATADIR="${APP_DATADIR}")
add_compile_definitions(LOCALEDIR="${APP_LOCALEDIR}")
if(ELFELLI_PROFILING)
  add_compile_definitions(PROFILING)
endif()

target_link_libraries(elfelli
  ${GTKMM_LIBRARIES}
  ${EXPAT_LIBRARIES}
  )

install(TARGETS elfelli
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  )
install(DIRECTORY data/
  DESTINATION ${CMAKE_INSTALL_DATADIR}/elfelli
  FILES_MATCHING
  PATTERN "*.svg"
  PATTERN "*.xml"
  )
install(FILES data/elfelli.desktop
  DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
  )
install(FILES data/elfelli-logo.svg
  DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps
  RENAME elfelli.svg
  )

find_program(RSVG
  rsvg_convert NAMES rsvg-convert
  )

add_custom_command(
  OUTPUT data/elfelli_48.png
  COMMAND ${CMAKE_COMMAND} -E make_directory data
  COMMAND "${RSVG_CONVERT}" ARGS -w 48 -h 48 -f png -o data/elfelli_48.png ${CMAKE_CURRENT_SOURCE_DIR}/data/elfelli-logo.svg
  MAIN_DEPENDENCY data/elfelli-logo.svg
  )
add_custom_target( icon ALL DEPENDS data/elfelli_48.png )
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/data/elfelli_48.png
  DESTINATION share/icons/hicolor/48x48/apps
  RENAME elfelli.png
  )

gettext_process_po_files(de
  ALL
  PO_FILES po/de.po
  )
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/de.gmo
  DESTINATION ${CMAKE_INSTALL_LOCALEDIR}/de/LC_MESSAGES
  RENAME elfelli.mo
  )
