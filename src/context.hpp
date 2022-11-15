#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector3.hpp>
#include <glm/glm.hpp>

// Forward declarations
namespace primitives
{

class Sphere;
class Box;

} // namespace primitives

namespace scene
{

class SceneTracer;

} // namespace scene

namespace render
{

class Context
{
public:
    Context(const sf::Vector2u& dimensions, float vertical_fov = 45.0f);

    void render_image(const glm::vec3& ray_origin, const primitives::Sphere& sphere,
                      const scene::SceneTracer& trace_scene);
    void render_image(const glm::vec3& ray_origin, const primitives::Box& box, const scene::SceneTracer& trace_scene);

    void set_color(const sf::Color& color);
    void set_color(const sf::Vector3f& color);
    void draw(sf::RenderWindow& window);

private:
    const sf::Vector2u image_size_;
    const float aspect_ratio_;
    const float vertical_fov_;
    const float tan_fvov_;
    sf::Image image_{};
    sf::Texture texture_{};
    sf::Sprite sprite_{};

    void load_image();
};

} // namespace render

#endif // CONTEXT_HPP