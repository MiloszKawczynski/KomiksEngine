#pragma once

#include "Component.h"
#include "Input.h"
#include "Truther.h"
#include "WheatOverlay.h"

class Jeep final : public Component
{
public:
    static std::shared_ptr<Jeep> create();

    explicit Jeep(AK::Badge<Jeep>);

    virtual void awake() override;
    virtual void fixed_update() override;

#if EDITOR
    virtual void draw_editor() override;
#endif

    glm::vec2 get_speed() const;

    float maximum_speed = 5.0f;
    float acceleration = 0.2f;
    float deceleration = 0.1f;

    bool is_active = false;

    std::weak_ptr<Truther> player = {};

    NON_SERIALIZED
    float light_range = 0.69f * 3.0f;

    NON_SERIALIZED
    float direction = 1.0f;

private:
    void handle_input();

    void find_new_target();

    glm::vec2 m_speed = glm::vec2(0.0f, 0.0f);
    glm::vec2 m_target;
    float m_point_on_path = 0.0f;
    float m_player_stun_range = 0.18f;
    float m_invincibility_timer = 0.0f;
    bool m_targeting_player = false;
};
