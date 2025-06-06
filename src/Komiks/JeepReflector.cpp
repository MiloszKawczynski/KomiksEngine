#include "JeepReflector.h"

#include "AK/AK.h"
#include "Camera.h"
#include "Entity.h"
#include "Globals.h"

#include <glm/gtc/random.hpp>

#if EDITOR
#include "imgui_extensions.h"
#endif

std::shared_ptr<JeepReflector> JeepReflector::create()
{
    return std::make_shared<JeepReflector>(AK::Badge<JeepReflector> {});
}

JeepReflector::JeepReflector(AK::Badge<JeepReflector>)
{
}

void JeepReflector::awake()
{
    set_can_tick(true);

    m_spotlight = entity->get_component<SpotLight>();

    // HACK: We need a really low far plane for some reason.
    m_spotlight.lock()->m_far_plane = 0.01f;

    if (m_spotlight.expired())
    {
        Debug::log("No spotlight attached to JeepReflector!", DebugType::Error);
        return;
    }
}

void JeepReflector::start()
{
    m_default_rotation = entity->transform->get_rotation();
}

void JeepReflector::update()
{
    if (jeep.expired() || m_spotlight.expired())
    {
        return;
    }

    auto const jeep_entity = jeep.lock()->entity;

    {
        auto new_position = jeep_entity->transform->get_position();
        new_position.y = 0.4f;
        entity->transform->set_position(new_position);

        auto const light_locked = m_spotlight.lock();

        auto current = new_position;
        current.y = 0.0f;
        auto const destination = AK::move_towards(current, {0.0f, 0.0f, 0.0f}, jeep.lock()->light_range);

        float spotlight_beam_width = 0.1f;
        //float const light_beam_length = glm::length(entity->transform->get_position() - light_locked->entity->transform->get_position());
        float const aperture = glm::atan(spotlight_beam_width / 1.2f);
        light_locked->cut_off = cos(aperture);
        light_locked->outer_cut_off = cos(aperture);
        light_locked->entity->transform->orient_towards(destination);
    }
}

#if EDITOR
void JeepReflector::draw_editor()
{
    Component::draw_editor();

    ImGuiEx::draw_ptr("Jeep", jeep);
}
#endif

void JeepReflector::set_destination(glm::vec2 destination)
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
