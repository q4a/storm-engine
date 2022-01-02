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
    DEPENDENCIES common ${DXVK_NATIVE_D3D9_LIB}
)
endif()
