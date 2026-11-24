#include <SFML/Graphics.hpp>
#include <algorithm>
#include <array>
#include <cmath>
#include <iomanip>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include "PhysicsModel.h"

namespace {
constexpr unsigned int width = 1000;
constexpr unsigned int height = 720;
constexpr float pivotX = 675.0f;
constexpr float pivotY = 132.0f;
constexpr float pixelsPerMeter = 170.0f;

bool loadWindowsFont(sf::Font& font) {
    const std::array<const char*, 2> paths = {
        "C:/Windows/Fonts/consola.ttf",
        "C:/Windows/Fonts/arial.ttf"
    };
    for (const char* path : paths) {
        if (font.openFromFile(path)) return true;
    }
    return false;
}

sf::Vector2f bobPosition(const PendulumModel& m) {
    return {pivotX + static_cast<float>(m.length * pixelsPerMeter * std::sin(m.theta)),
            pivotY + static_cast<float>(m.length * pixelsPerMeter * std::cos(m.theta))};
}

std::string infoString(const PendulumModel& m, bool paused) {
    std::ostringstream s;
    s << std::fixed << std::setprecision(2)
      << "Length: " << m.length << " m\n"
      << "Start angle: " << m.initialAngleDegrees() << " deg\n"
      << "Current angle: " << m.currentAngleDegrees() << " deg\n"
      << "Angular velocity: " << m.omega << " rad/s\n"
      << "Predicted period: " << m.smallAnglePeriod() << " s\n"
      << "Time: " << m.elapsed << " s\n"
      << "State: " << (paused ? "PAUSED" : "RUNNING") << "\n\n"
      << "UP/DOWN - length +/- 0.10 m\n"
      << "LEFT/RIGHT - start angle +/- 5 deg\n"
      << "P - pause | R - restart | ESC - exit\n"
      << "Length or angle changes restart simulation";
    return s.str();
}
}

int main() {
    sf::RenderWindow window(sf::VideoMode({width, height}),
                            "Lab 5 - Verhunenko Daniela - Pendulum");
    window.setFramerateLimit(60);

    sf::Font font;
    const bool hasFont = loadWindowsFont(font);
    sf::Text info(font, "", 18);
    info.setFillColor(sf::Color(230, 243, 255));
    info.setPosition({24.0f, 24.0f});

    PendulumModel model;
    bool paused = false;
    sf::CircleShape bob(26.0f);
    bob.setOrigin({26.0f, 26.0f});
    bob.setFillColor(sf::Color(100, 217, 238));
    bob.setOutlineThickness(3.0f);
    bob.setOutlineColor(sf::Color(222, 250, 255));

    sf::CircleShape pivot(8.0f);
    pivot.setOrigin({8.0f, 8.0f});
    pivot.setPosition({pivotX, pivotY});
    pivot.setFillColor(sf::Color(255, 193, 75));

    sf::RectangleShape header({425.0f, 290.0f});
    header.setPosition({12.0f, 12.0f});
    header.setFillColor(sf::Color(15, 31, 49, 225));

    sf::Clock clock;
    double accumulator = 0.0;
    float captionTimer = 1.0f;
    constexpr double fixedStep = 1.0 / 240.0;
    std::vector<sf::Vector2f> trace;

    while (window.isOpen()) {
        const float frameDt = std::min(clock.restart().asSeconds(), 0.05f);
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                using Key = sf::Keyboard::Key;
                bool changed = false;
                switch (key->code) {
                case Key::Escape: window.close(); break;
                case Key::P: paused = !paused; break;
                case Key::R: model.reset(); changed = true; paused = false; break;
                case Key::Up: model.setLength(model.length + 0.1); changed = true; break;
                case Key::Down: model.setLength(model.length - 0.1); changed = true; break;
                case Key::Right:
                    model.setInitialAngleDegrees(model.initialAngleDegrees() + 5.0);
                    changed = true; break;
                case Key::Left:
                    model.setInitialAngleDegrees(model.initialAngleDegrees() - 5.0);
                    changed = true; break;
                default: break;
                }
                if (changed) { accumulator = 0.0; trace.clear(); }
            }
        }

        if (!paused) {
            accumulator += frameDt;
            while (accumulator >= fixedStep) {
                model.step(fixedStep);
                trace.push_back(bobPosition(model));
                if (trace.size() > 650) trace.erase(trace.begin());
                accumulator -= fixedStep;
            }
        }
        captionTimer += frameDt;
        const sf::Vector2f endpoint = bobPosition(model);
        bob.setPosition(endpoint);

        sf::VertexArray rope(sf::PrimitiveType::Lines);
        rope.append(sf::Vertex{{pivotX, pivotY}, sf::Color(243, 222, 168)});
        rope.append(sf::Vertex{endpoint, sf::Color(243, 222, 168)});

        sf::VertexArray vertical(sf::PrimitiveType::Lines);
        vertical.append(sf::Vertex{{pivotX, pivotY}, sf::Color(96, 126, 152, 160)});
        vertical.append(sf::Vertex{{pivotX, pivotY + 440.0f}, sf::Color(96, 126, 152, 160)});

        sf::VertexArray trajectory(sf::PrimitiveType::LineStrip);
        for (const auto& point : trace)
            trajectory.append(sf::Vertex{point, sf::Color(76, 161, 207, 140)});

        const std::string infoText = infoString(model, paused);
        if (hasFont) info.setString(infoText);
        if (captionTimer >= 0.25f) {
            std::ostringstream title;
            title << std::fixed << std::setprecision(2)
                  << "Lab 5 - Daniela | L=" << model.length
                  << " m | start=" << model.initialAngleDegrees()
                  << " deg | theta=" << model.currentAngleDegrees()
                  << " deg | T=" << model.smallAnglePeriod()
                  << " s" << (paused ? " [PAUSE]" : "");
            window.setTitle(title.str());
            captionTimer = 0.0f;
        }

        window.clear(sf::Color(24, 45, 63));
        window.draw(vertical);
        window.draw(trajectory);
        window.draw(rope);
        window.draw(pivot);
        window.draw(bob);
        window.draw(header);
        if (hasFont) window.draw(info);
        window.display();
    }
    return 0;
}
