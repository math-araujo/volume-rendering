#include <omp.h>
#include <stdexcept>

#include <glm/gtx/transform.hpp>

#include "context.hpp"
#include "scene_tracer.hpp"
#include "util.hpp"

namespace render
{

Context::Context(const sf::Vector2u& dimensions, float vertical_fov) :
    image_size_{dimensions}, aspect_ratio_{static_cast<float>(image_size_.x) / static_cast<float>(image_size_.y)},
    vertical_fov_{vertical_fov}, tan_fvov_{std::tan(glm::radians(vertical_fov_ / 2.0f))}
{
    image_.create(image_size_.x, image_size_.y, sf::Color::Black);
    load_image();
    sprite_.setTexture(texture_, true);
}

void Context::render_image(const glm::vec3& ray_origin, const primitives::Sphere& sphere,
                           const scene::SceneTracer& trace_scene)
{
    const std::uint32_t dimensions{image_size_.x * image_size_.y};
#pragma omp parallel for schedule(dynamic, 16 * 16)
    for (std::uint32_t index = 0; index < dimensions; ++index)
    {
        const std::uint32_t y{index / image_size_.x};
        const std::uint32_t x{index % image_size_.x};
        glm::vec3 pixel_screen_coordinates{((2.0f * ((x + 0.5f) / image_size_.x)) - 1.0f) * aspect_ratio_ * tan_fvov_,
                                           (-1 * ((2.0f * ((y + 0.5f) / image_size_.y)) - 1.0f)) * tan_fvov_, -1.0f};

        geometry::Ray ray{.origin = ray_origin, .direction = glm::normalize(pixel_screen_coordinates - ray_origin)};
        image_.setPixel(x, y, util::vector_to_color(trace_scene(ray, sphere)));
    }

    load_image();
    image_.saveToFile("volume.png");
}

void Context::render_image(const glm::vec3& ray_origin, const primitives::Box& box,
                           const scene::SceneTracer& trace_scene)
{
    glm::mat4 camera_to_world{glm::translate(glm::mat4{1.0f}, glm::vec3{83.292171f, 25.137326f, 126.430772f})};
    camera_to_world = glm::rotate(camera_to_world, glm::radians(30.0f), glm::vec3{0.0f, 1.0f, 0.0f});
    camera_to_world = glm::rotate(camera_to_world, glm::radians(-15.0f), glm::vec3{1.0f, 0.0f, 0.0f});

    const std::uint32_t dimensions{image_size_.x * image_size_.y};
#pragma omp parallel for schedule(dynamic)
    for (std::uint32_t index = 0; index < dimensions; ++index)
    {
        const std::uint32_t y{index / image_size_.x};
        const std::uint32_t x{index % image_size_.x};
        glm::vec3 pixel_screen_coordinates{((2.0f * ((x + 0.5f) / image_size_.x)) - 1.0f) * aspect_ratio_ * tan_fvov_,
                                           (-1 * ((2.0f * ((y + 0.5f) / image_size_.y)) - 1.0f)) * tan_fvov_, -1.0f};
        auto ray = util::transform_ray(camera_to_world, ray_origin, pixel_screen_coordinates);
        image_.setPixel(x, y, util::vector_to_color(trace_scene(ray, box)));
    }

    load_image();
    image_.saveToFile("grid_volume.png");
}

void Context::set_color(const sf::Color& color)
{
    for (std::uint32_t y = 0; y < image_size_.y; ++y)
    {
        for (std::uint32_t x = 0; x < image_size_.x; ++x)
        {
            image_.setPixel(x, y, color);
        }
    }
    load_image();
}

void Context::set_color(const sf::Vector3f& color)
{
    set_color(util::vector_to_color(color));
}

void Context::draw(sf::RenderWindow& window)
{
    window.draw(sprite_);
}

void Context::load_image()
{
    if (!texture_.loadFromImage(image_))
    {
        throw std::runtime_error{"Failed to load image to texture"};
    }
}

} // namespace render