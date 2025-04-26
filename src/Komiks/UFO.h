#pragma once

#include "Component.h"

#include "AK/Badge.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

class UFO final : public Component
{
public:
    static std::shared_ptr<UFO> create();

    explicit UFO(AK::Badge<UFO>);

    virtual void awake() override;
    virtual void start() override;
    virtual void update() override;

#if EDITOR
    virtual void draw_editor() override;
#endif

private:
    glm::vec3 m_destination = {};
    glm::vec3 m_start_position = {};
    float m_move_timer = 0.0f;
    float m_move_duration = 2.5f;
};
