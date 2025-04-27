#pragma once

#include "Component.h"

#include "AK/Badge.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

class CowManager;

class Cow final : public Component
{
public:
    static std::shared_ptr<Cow> create();

    explicit Cow(AK::Badge<Cow>);

    virtual void awake() override;
    virtual void start() override;
    virtual void fixed_update() override;
    virtual void on_collision_enter(std::shared_ptr<Collider2D> const& other) override;
    virtual void on_trigger_enter(std::shared_ptr<Collider2D> const& other) override;

    void play_sound_per_chance(float const chance) const;

    std::weak_ptr<CowManager> cow_manager = {};
    bool is_sucked = false;

private:
    glm::vec2 m_destination = {};
    std::weak_ptr<Collider2D> m_collider = {};
    float m_height = 0.0f;
    float m_gravitation = 0.001f;
    float m_velocity = 0.0f;
    float m_stopped_timer = 0.0f;
    glm::vec2 m_time_to_stop_range = {5.0f, 15.0f};
};
