if (WIN32)
STORM_SETUP(
    TARGET_NAME renderer
    TYPE storm_module
    DEPENDENCIES common legacy_stdio_definitions
)
else()
STORM_SETUP(
    TARGET_NAME renderer
    TYPE storm_module
    DEPENDENCIES common
)
endif()