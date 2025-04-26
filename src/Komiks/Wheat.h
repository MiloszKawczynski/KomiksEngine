#pragma once

#include "Component.h"

#include "AK/Badge.h"
#include "glm/detail/type_quat.hpp"
#include "glm/fwd.hpp"

#include <glm/vec2.hpp>

class Wheat final : public Component
{
public:
    static std::shared_ptr<Wheat> create();

    explicit Wheat(AK::Badge<Wheat>);

    virtual void awake() override;
    virtual void start() override;
    virtual void update() override;

    bool is_bended() const;
    void set_bended(bool bended, glm::vec2 const& direction = {});
    void set_destination(glm::vec2 destination);

    static std::vector<std::weak_ptr<Wheat>> all_wheat;

private:
    glm::quat m_default_rotation = {};
    glm::quat m_target_rot = {};
    glm::vec3 m_destination = {};
    glm::vec3 m_pushed_destination = {};
    bool m_bended = false;
    bool m_bending = true;
};
