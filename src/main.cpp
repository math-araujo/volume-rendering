#include <array>
#include <cmath>
#include <iostream>
#include <stdexcept>

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

class ImageData
{
public:
    ImageData(const sf::Vector2u& dimensions) : image_size_{dimensions}
    {
        image_.create(image_size_.x, image_size_.y, sf::Color::Black);
        load_image();
        sprite_.setTexture(texture_, true);
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
    const sf::Vector3f volume{0.0f, 0.0f, 0.0f};
    ImageData image_data{image_size};
    sf::RenderWindow window{sf::VideoMode{image_size.x, image_size.y}, "Volume Renderer"};

    sf::Clock delta_clock;
    bool imgui_init = ImGui::SFML::Init(window);
    if (!imgui_init)
    {
        std::cerr << "Failed to initialize ImGui::SFML" << std::endl;
        return 1;
    }

    float distance{10.0f};
    float absorption_coeff{0.0f};
    float transmittance{beer_lambert_transmittance(distance, absorption_coeff)};
    bool update{false};
    image_data.set_color(volume_scattering(transmittance, background, volume));

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
        ImGui::Begin("Hello, world!");
        update = ImGui::SliderFloat("Distance", &distance, 0.0f, 10.0f);
        update = ImGui::SliderFloat("Absorption Coefficient", &absorption_coeff, 0.0f, 1.0f) || update;
        if (update)
        {
            transmittance = beer_lambert_transmittance(distance, absorption_coeff);
            image_data.set_color(volume_scattering(transmittance, background, volume));
            update = false;
        }
        ImGui::Text("Transmittance: %.4f", transmittance);
        ImGui::End();

        window.clear(sf::Color::Black);
        image_data.draw(window);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
}