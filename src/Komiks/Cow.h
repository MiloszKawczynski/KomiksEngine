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
    virtual void update() override;
    virtual void on_collision_enter(std::shared_ptr<Collider2D> const& other) override;

    std::weak_ptr<CowManager> cow_manager = {};

private:
    glm::vec2 m_destination = {};
    std::weak_ptr<Collider2D> m_collider = {};
};
