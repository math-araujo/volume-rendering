set(FILENAMES 
    #main.cpp
    scene_tracer.hpp scene_tracer.cpp
    primitives.hpp primitives.cpp
    ray.hpp ray.cpp
    volume.hpp volume.cpp
    phase.hpp phase.cpp
    random_gen.hpp random_gen.cpp
    density.hpp density.cpp
    context.hpp context.cpp
    util.hpp util.cpp
)

add_library(volrender STATIC ${FILENAMES})
target_link_libraries(volrender PUBLIC 
    glm::glm unofficial::noise::noise-static unofficial::noiseutils::noiseutils-static
    sfml-system sfml-graphics sfml-window
    imgui::imgui ImGui-SFML::ImGui-SFML
)
target_compile_features(volrender PRIVATE cxx_std_20)