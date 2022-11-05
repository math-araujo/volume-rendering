set(FILENAMES 
    main.cpp
    scene_tracer.hpp scene_tracer.cpp
    primitives.hpp primitives.cpp
    ray.hpp ray.cpp
    volume.hpp volume.cpp
)

set(DIR_FILENAMES "")
foreach(FILENAME ${FILENAMES})
    list(APPEND DIR_FILENAMES ${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME})
endforeach()
set(SOURCES ${DIR_FILENAMES} PARENT_SCOPE)