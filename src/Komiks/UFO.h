#pragma once

#include "Component.h"

#include "AK/Badge.h"

#include "CowManager.h"
#include "Game/Truther.h"
#include <Game/FieldCell.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

class Thruter;

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

    void choose_position();

    std::weak_ptr<FieldGrid> field_grid = {};
    std::weak_ptr<Truther> truther = {};
    std::weak_ptr<SpotLight> attract_bean = {};
    std::weak_ptr<CowManager> cow_manager = {};

private:
    glm::vec3 m_destination = {};
    glm::vec3 m_start_position = {};
    float m_move_timer = 0.0f;
    float m_move_duration = 2.5f;
    float m_beam_radius = 20.0f;
    bool m_is_unbending_done = false;
    bool m_is_active = false;
};
