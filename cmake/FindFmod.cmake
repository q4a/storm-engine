# Find Fmod SDK

# Define:
# Fmod_FOUND
# Fmod_INCLUDE_DIR
# Fmod_LIBRARY
# Fmod_ROOT_DIR

set(Fmod_FOUND false)

find_path(Fmod_INCLUDE_DIR fmod.h "${STORM_EXTERN_DIR}/fmod/include")
mark_as_advanced(Fmod_INCLUDE_DIR)
if(Fmod_INCLUDE_DIR)
    set(Fmod_ROOT_DIR "${STORM_EXTERN_DIR}/fmod")
endif()

set(Fmod_LIBRARY_PATHS)
set(Fmod_LIBRARY_PATHS "${Fmod_ROOT_DIR}/lib")

find_library(Fmod_LIBRARY fmod_vc ${Fmod_LIBRARY_PATHS} NO_DEFAULT_PATH)
if(Fmod_INCLUDE_DIR AND Fmod_LIBRARY)
    set(Fmod_FOUND true)
endif()
mark_as_advanced(Fmod_LIBRARY)

message("Fmod_FOUND=${Fmod_FOUND}")
message("Fmod_INCLUDE_DIR=${Fmod_INCLUDE_DIR}")
message("Fmod_LIBRARY=${Fmod_LIBRARY}")
message("Fmod_ROOT_DIR=${Fmod_ROOT_DIR}")