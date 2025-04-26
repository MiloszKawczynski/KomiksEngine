#pragma once

#include "Component.h"
#include "Input.h"
#include "WheatOverlay.h"

class Jeep final : public Component
{
public:
    static std::shared_ptr<Jeep> create();

    explicit Jeep(AK::Badge<Jeep>);

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

    float maximum_speed = 5.0f;
    float acceleration = 0.2f;
    float deceleration = 0.1f;

private:
    void handle_input();

    void find_new_target();

    glm::vec2 m_speed = glm::vec2(0.0f, 0.0f);
    glm::vec2 m_target;
    float m_point_on_path = 0.0f;
};
