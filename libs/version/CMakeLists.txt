# generate kritaversion.h
configure_file(kritaversion.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/kritaversion.h)

# generate kritagitversion.h
configure_file(kritagitversion.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/kritagitversion.h)

set(kritaversion_LIB_SRCS
    KritaVersionWrapper.cpp
)

kis_add_library(kritaversion SHARED ${kritaversion_LIB_SRCS})

# KRIMBLE 2026-10-03: a build stamp (build time + git hash) that is regenerated on EVERY build,
# so the version shown in the splash screen and logs changes with each build. The git hash from
# kritagitversion.h is only refreshed when CMake reconfigures, which is why it used to be stale.
set(KRIMBLE_BUILD_STAMP_HEADER ${CMAKE_CURRENT_BINARY_DIR}/krimble_build_stamp.h)
add_custom_target(krimble_build_stamp ALL
    COMMAND ${CMAKE_COMMAND}
        -DOUT=${KRIMBLE_BUILD_STAMP_HEADER}
        -DSRC=${CMAKE_SOURCE_DIR}
        -P ${CMAKE_CURRENT_SOURCE_DIR}/KrimbleBuildStamp.cmake
    BYPRODUCTS ${KRIMBLE_BUILD_STAMP_HEADER}
    COMMENT "Generating the Krimble build stamp")
add_dependencies(kritaversion krimble_build_stamp)
target_link_libraries(kritaversion Qt${QT_MAJOR_VERSION}::Core)
generate_export_header(kritaversion BASE_NAME kritaversion)

set_target_properties(kritaversion PROPERTIES
    VERSION ${GENERIC_KRITA_LIB_VERSION} SOVERSION ${GENERIC_KRITA_LIB_SOVERSION}
)
install(TARGETS kritaversion ${INSTALL_TARGETS_DEFAULT_ARGS} )

