#include "Wheat.h"

#include "AK/AK.h"
#include "Camera.h"
#include "Entity.h"
#include "Globals.h"

#include <glm/gtc/random.hpp>

#if EDITOR
#include "imgui_extensions.h"
#endif

std::vector<std::weak_ptr<Wheat>> Wheat::all_wheat = {};

std::shared_ptr<Wheat> Wheat::create()
{
    return std::make_shared<Wheat>(AK::Badge<Wheat> {});
}

Wheat::Wheat(AK::Badge<Wheat>)
{
}

void Wheat::awake()
{
    set_can_tick(true);
    all_wheat.push_back(static_pointer_cast<Wheat>(shared_from_this()));
}

void Wheat::on_destroyed()
{
    // HACK: Destroying one wheat clears the whole list.
    all_wheat.clear();
}

void Wheat::start()
{
    m_default_rotation = entity->transform->get_rotation();
}

void Wheat::update()
{
    if (!m_bending)
        return;

    glm::quat const current_quat = entity->transform->get_rotation();

    float t = 0.1f;
    glm::quat const smooth_quat = glm::slerp(current_quat, m_target_rot, t);

    glm::vec3 const smooth_euler_rad = glm::eulerAngles(smooth_quat);
    glm::vec3 const smooth_euler_deg = glm::degrees(smooth_euler_rad);
    entity->transform->set_rotation(smooth_euler_deg);

    float angle_diff = glm::degrees(glm::angle(smooth_quat * glm::inverse(m_target_rot)));
    if (angle_diff < 0.5f)
    {
        m_bending = false;
    }
}

bool Wheat::is_bended() const
{
    return m_bended;
}

// Direction is only needed if bended == true
void Wheat::set_bended(bool bended, glm::vec2 const& direction)
{
    if (m_bended == bended)
    {
        return;
    }

    m_bended = bended;
    m_bending = true;

    if (!m_bended)
    {
        m_target_rot = m_default_rotation;
    }
    else
    {
        set_destination(direction);
    }
}

void Wheat::set_destination(glm::vec2 destination)
{
    if (glm::length(destination) < 0.001f)
        return;

    destination = glm::normalize(destination);

    glm::vec3 dest_3d = {destination.x, 0.0f, destination.y};

    static glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 axis = glm::normalize(glm::cross(up, dest_3d));

    float angle_deg = 90.0f;
    float angle_rad = glm::radians(angle_deg);
    m_target_rot = glm::angleAxis(angle_rad, axis);
}
