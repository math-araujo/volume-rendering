#include <array>
#include <cmath>
#include <iostream>
#include <memory>
#include <stdexcept>

#include <glm/glm.hpp>
#include <imgui-SFML.h>
#include <imgui.h>

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

sf::Color vector_to_color(const sf::Vector3f& vector)
{
    return sf::Color{static_cast<std::uint8_t>(vector.x * 255.0f), static_cast<std::uint8_t>(vector.y * 255.0f),
                     static_cast<std::uint8_t>(vector.z * 255.0f)};
}

float beer_lambert_transmittance(float distance, float absorption_coeff)
{
    return std::exp(-distance * absorption_coeff);
}

sf::Vector3f volume_scattering(float transmittance, const sf::Vector3f& background,
                               const sf::Vector3f& volume = sf::Vector3f{0.0f, 0.0f, 0.0f})
{
    return (transmittance * background) + (1.0f - transmittance) * volume;
}

struct Ray
{
    glm::vec3 origin{};
    glm::vec3 direction{};

    glm::vec3 evaluate(float parameter) const
    {
        return origin + (parameter * direction);
    }
};

struct Sphere
{
    float absorption_coeff{0.1f};
    float radius{1.0f};
    glm::vec3 center{0.0f, 0.0f, -4.0f};
    sf::Vector3f color{1.0f, 0.0f, 1.0f};

    bool intersect(const Ray& ray, float& t0, float& t1) const
    {
        const float quadratic_coeff{glm::dot(ray.direction, ray.direction)};
        const glm::vec3 center_to_origin{ray.origin - center};
        const float linear_coeff{2.0f * glm::dot(ray.direction, center_to_origin)};
        const float independent_coeff{glm::dot(center_to_origin, center_to_origin) - radius * radius};

        const float discriminant{linear_coeff * linear_coeff - 4 * quadratic_coeff * independent_coeff};
        if (discriminant < 0.0f)
        {
            return false;
        }

        t0 = (-linear_coeff - discriminant) / (2.0f * quadratic_coeff);
        t1 = (-linear_coeff + discriminant) / (2.0f * quadratic_coeff);
        return true;
    }
};

struct SceneTracer
{
    virtual ~SceneTracer() = default;
    virtual sf::Vector3f operator()(const Ray& ray, const Sphere& sphere) const = 0;
};

// Chapter 1 - Ray Casting with Beer-Lambert Law
struct VolumeRayCasting : public SceneTracer
{
    ~VolumeRayCasting() override = default;

    sf::Vector3f operator()(const Ray& ray, const Sphere& sphere) const override
    {
        float t0{0.0f};
        float t1{0.0f};
        const sf::Vector3f background{0.0f, 1.0f, 1.0f};
        if (sphere.intersect(ray, t0, t1))
        {
            const glm::vec3 first_hit{ray.evaluate(t0)};
            const glm::vec3 second_hit{ray.evaluate(t1)};
            const float distance{glm::length(second_hit - first_hit)};
            const float transmittance{beer_lambert_transmittance(distance, sphere.absorption_coeff)};
            return volume_scattering(transmittance, background, sphere.color);
        }

        return background;
    }
};

class ImageData
{
public:
    ImageData(const sf::Vector2u& dimensions, float vertical_fov = 90.0f) :
        image_size_{dimensions}, aspect_ratio_{static_cast<float>(image_size_.x) / static_cast<float>(image_size_.y)},
        vertical_fov_{vertical_fov}, tan_fvov_{std::tan(glm::radians(vertical_fov_ / 2.0f))}
    {
        image_.create(image_size_.x, image_size_.y, sf::Color::Black);
        load_image();
        sprite_.setTexture(texture_, true);
    }

    void render_image(const glm::vec3& ray_origin, const Sphere& sphere, const SceneTracer& trace_scene)
    {
        for (std::uint32_t y = 0; y < image_size_.y; ++y)
        {
            for (std::uint32_t x = 0; x < image_size_.x; ++x)
            {
                glm::vec3 pixel_screen_coordinates{
                    ((2.0f * ((x + 0.5f) / image_size_.x)) - 1.0f) * aspect_ratio_ * tan_fvov_,
                    (-1 * ((2.0f * ((y + 0.5f) / image_size_.y)) - 1.0f)) * tan_fvov_, -1.0f};

                Ray ray{.origin = ray_origin, .direction = glm::normalize(pixel_screen_coordinates - ray_origin)};
                image_.setPixel(x, y, vector_to_color(trace_scene(ray, sphere)));
            }
        }

        load_image();
    }

    void set_color(const sf::Color& color)
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

    void set_color(const sf::Vector3f& color)
    {
        set_color(vector_to_color(color));
    }

    void draw(sf::RenderWindow& window)
    {
        window.draw(sprite_);
    }

private:
    const sf::Vector2u image_size_;
    const float aspect_ratio_;
    const float vertical_fov_;
    const float tan_fvov_;
    sf::Image image_{};
    sf::Texture texture_{};
    sf::Sprite sprite_{};

    void load_image()
    {
        if (!texture_.loadFromImage(image_))
        {
            throw std::runtime_error{"Failed to load image to texture"};
        }
    }
};

int main()
{
    const sf::Vector2u image_size{800, 600};
    const sf::Vector3f background{0.0f, 1.0f, 1.0f};
    ImageData image_data{image_size};
    sf::RenderWindow window{sf::VideoMode{image_size.x, image_size.y}, "Volume Renderer"};

    sf::Clock delta_clock;
    bool imgui_init = ImGui::SFML::Init(window);
    if (!imgui_init)
    {
        std::cerr << "Failed to initialize ImGui::SFML" << std::endl;
        return 1;
    }

    bool update{false};
    Sphere sphere{};
    std::unique_ptr<SceneTracer> tracer{std::make_unique<VolumeRayCasting>()};
    image_data.render_image(glm::vec3{0.0f, 0.0f, 0.0f}, sphere, *tracer);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(window, event);
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        ImGui::SFML::Update(window, delta_clock.restart());
        ImGui::Begin("Settings");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);
        if (ImGui::TreeNode("Volume"))
        {
            update = ImGui::SliderFloat("Absorption Coefficient", &sphere.absorption_coeff, 0.0f, 1.0f);
            if (update)
            {
                image_data.render_image(glm::vec3{0.0f, 0.0f, 0.0f}, sphere, *tracer);
                update = false;
            }
            ImGui::TreePop();
        }
        ImGui::End();

        window.clear(sf::Color::Black);
        image_data.draw(window);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
}