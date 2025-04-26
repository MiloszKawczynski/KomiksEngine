#pragma once

#include "Component.h"

#include "AK/Badge.h"
#include "Game/Jeep.h"

#include <glm/detail/type_quat.hpp>
#include <glm/fwd.hpp>
#include <glm/vec2.hpp>

class JeepReflector final : public Component
{
public:
    static std::shared_ptr<JeepReflector> create();

    explicit JeepReflector(AK::Badge<JeepReflector>);

    virtual void awake() override;
    virtual void start() override;
    virtual void update() override;

#if EDITOR
    virtual void draw_editor() override;
#endif

    std::weak_ptr<Jeep> jeep = {};

    void set_destination(glm::vec2 destination);

private:
    std::weak_ptr<SpotLight> m_spotlight = {};

    glm::quat m_default_rotation = {};
    glm::quat m_target_rot = {};
    glm::vec3 m_destination = {};
    glm::vec3 m_pushed_destination = {};
};
