#pragma once

#include "Component.h"
#include "Input.h"
#include "WheatOverlay.h"

class Truther final : public Component
{
public:
    static std::shared_ptr<Truther> create();

    explicit Truther(AK::Badge<Truther>);

    virtual void awake() override;
    virtual void update() override;
    virtual void fixed_update() override;

#if EDITOR
    virtual void draw_editor() override;
#endif

    virtual void on_trigger_enter(std::shared_ptr<Collider2D> const& other) override;

    virtual void on_trigger_exit(std::shared_ptr<Collider2D> const& other) override;

    virtual void on_destroyed() override;

    glm::vec2 get_speed() const;

    void suck();
    void stun();

    // Bend or unbend
    bool truther_bends = true;

    float maximum_speed = 5.0f;
    float acceleration = 0.2f;
    float deceleration = 0.1f;
    float jump_power = 0.5f;
    float jump_horizontal_power = 2.0f;
    float gravitation = 0.01f;
    float suck_power = 0.01f;

    bool is_sucked = false;

    std::weak_ptr<WheatOverlay> wheat_overlay = {};

private:
    void handle_input();

    glm::vec2 m_speed = glm::vec2(0.0f, 0.0f);
    float m_height = 0.0f;
    float m_velocity = 0.0f;
    float m_stun_timer = 0.0f;
};
