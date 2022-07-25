if (WIN32)
    set(SYSTEM_DEPS "directx")
else()
set(SYSTEM_DEPS ${LIBSAFEC_LIBRARIES})
endif()

STORM_SETUP(
    TARGET_NAME common
    TYPE library
    DEPENDENCIES shared_headers util ${SPDLOG_LIBRARIES} ${SYSTEM_DEPS}
)