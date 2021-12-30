if (WIN32)
STORM_SETUP(
    TARGET_NAME xinterface
    TYPE storm_module
    DEPENDENCIES common ddraw amstrmid util
    TEST_DEPENDENCIES catch2
)
else()
STORM_SETUP(
    TARGET_NAME xinterface
    TYPE storm_module
    DEPENDENCIES common util
    TEST_DEPENDENCIES catch2
)
endif()
