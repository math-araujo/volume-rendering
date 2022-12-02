#include <algorithm>
#include <array>
#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>

#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>
#include <imgui-SFML.h>
#include <imgui.h>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include "context.hpp"
#include "primitives.hpp"
#include "ray.hpp"
#include "scene_tracer.hpp"

std::vector<float> read_density_from_file(std::string filename = "cachefiles/grid.40.bin")
{
    std::ifstream stream{filename, std::ios::binary};
    std::vector<float> density_data(128 * 128 * 128);
    stream.read((char*)(density_data.data()), sizeof(float) * 128 * 128 * 128);
    return density_data;
}

int main()
{
    primitives::Box box{};
    box.density = read_density_from_file();
    std::unique_ptr<scene::SceneTracer> tracer{std::make_unique<scene::VolumeVoxelGrid>()};

    const sf::Vector2u image_size{640, 480};
    const sf::Vector3f background{0.0f, 1.0f, 1.0f};
    render::Context render_context{image_size};

    sf::Clock render_clock;
    std::cout << "Rendering image..." << std::endl;
    const glm::vec3 ray_origin{0.0f, 0.0f, 0.0f};
    render_context.render_image(ray_origin, box, *tracer);
    std::cout << "Done! Time elapsed: " << render_clock.restart().asSeconds() << " seconds\n";

    sf::RenderWindow window{sf::VideoMode{image_size.x, image_size.y}, "Volume Renderer"};

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        window.clear(sf::Color::Black);
        render_context.draw(window);
        window.display();
    }
}