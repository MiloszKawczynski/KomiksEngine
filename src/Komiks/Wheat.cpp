#include "Wheat.h"

#include "AK/AK.h"
#include "AK/Math.h"
#include "Camera.h"
#include "Entity.h"
#include "Globals.h"
#include "SceneSerializer.h"

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

void Wheat::update()
{
    if (!m_bended)
        return;

    glm::vec3 current_euler_deg = entity->transform->get_euler_angles();
    glm::vec3 current_euler_rad = glm::radians(current_euler_deg);
    glm::quat current_quat = glm::quat(current_euler_rad);

    float t = 0.1f;
    glm::quat smooth_quat = glm::slerp(current_quat, m_target_rot, t);

    glm::vec3 smooth_euler_rad = glm::eulerAngles(smooth_quat);
    glm::vec3 smooth_euler_deg = glm::degrees(smooth_euler_rad);
    entity->transform->set_rotation(smooth_euler_deg);
}

void Wheat::set_bended(bool bended, glm::vec2 const& direction)
{
    if (m_bended == bended)
    {
        return;
    }

    m_bended = bended;
    set_destination(direction);
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

    // Step 4: Convert to Euler angles in degrees
    glm::vec3 euler_rad = glm::eulerAngles(m_target_rot);
    m_destination = glm::degrees(euler_rad);
}
