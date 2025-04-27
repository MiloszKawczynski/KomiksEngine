#include "Jeep.h"

#include "AK/AK.h"
#include "AK/Math.h"
#include "Camera.h"
#include "Entity.h"
#include "ExampleUIBar.h"
#include "Factory.h"
#include "Floater.h"
#include "GameController.h"
#include "Globals.h"
#include "IceBound.h"
#include "Komiks/Wheat.h"
#include "LevelController.h"
#include "Lighthouse.h"
#include "PhysicsEngine.h"
#include "Player.h"
#include "Port.h"
#include "ResourceManager.h"
#include "SceneSerializer.h"
#include "Ship.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/random.hpp>

#if EDITOR
#include "imgui_extensions.h"
#include <imgui.h>
#endif
#include "Path.h"

std::shared_ptr<Sound> Jeep::m_jeep_sound = {};

std::shared_ptr<Jeep> Jeep::create()
{
    return std::make_shared<Jeep>(AK::Badge<Jeep> {});
}

Jeep::Jeep(AK::Badge<Jeep>)
{
}

void Jeep::awake()
{
    set_can_tick(true);

    find_new_target();

    entity->transform->set_local_position({m_target.x, 0.375f, m_target.y});
}

void Jeep::fixed_update()
{
    i32 horizontal = 0;
    i32 vertical = 0;

    if (is_active)
    {
        if (!m_sound_was_spawned && m_jeep_sound == nullptr)
        {
            m_jeep_sound = Sound::play_sound("./res/audio/Komiks/silnik4.wav", true);
            m_jeep_sound->set_volume(6.5f);
            m_sound_was_spawned = true;
        }

        if (m_targeting_player)
        {
            m_target = AK::convert_3d_to_2d(player.lock()->entity->transform->get_position());
        }
        else
        {
            find_new_target();
        }

        if (m_invincibility_timer > 0.0f)
        {
            m_invincibility_timer -= fixed_delta_time;
        }

        horizontal = glm::sign(m_target.x - entity->transform->get_position().x);
        vertical = glm::sign(m_target.y - entity->transform->get_position().z);

        m_speed.x += horizontal * acceleration;
        m_speed.y += vertical * acceleration;

        if (horizontal == 0)
        {
            m_speed.x -= glm::sign(m_speed.x) * deceleration;
        }

        if (vertical == 0)
        {
            m_speed.y -= glm::sign(m_speed.y) * deceleration;
        }

        if (abs(m_speed.x) < deceleration)
        {
            m_speed.x = 0.0f;
        }

        if (abs(m_speed.y) < deceleration)
        {
            m_speed.y = 0.0f;
        }

        if (glm::length(m_speed) > maximum_speed)
        {
            m_speed = glm::normalize(m_speed) * maximum_speed;
        }

        glm::vec3 speed_vector = glm::vec3(m_speed.x, 0.0f, m_speed.y);
        speed_vector *= fixed_delta_time;

        entity->transform->set_local_position(entity->transform->get_local_position() + speed_vector);

        if (glm::length(m_speed) >= maximum_speed * 0.25f)
        {
            float rotation = atan2(speed_vector.x, speed_vector.z) * (180.0f / 3.14f) - 180.0f;
            float actual_rotation = entity->transform->get_euler_angles().y;

            while (rotation > 180.0f)
                rotation -= 360.0f;
            while (rotation < -180.0f)
                rotation += 360.0f;

            while (actual_rotation > 180.0f)
                actual_rotation -= 360.0f;
            while (actual_rotation < -180.0f)
                actual_rotation += 360.0f;

            float delta = rotation - actual_rotation;
            while (delta > 180.0f)
                delta -= 360.0f;
            while (delta < -180.0f)
                delta += 360.0f;

            float new_rotation = actual_rotation + delta * 0.1f;

            entity->transform->set_euler_angles({0.0f, new_rotation, 0.0f});
        }

        glm::vec2 const direction =
            glm::normalize(glm::vec2(entity->transform->get_local_position().x, entity->transform->get_local_position().z));

        {
            if (player.expired())
            {
                return;
            }

            auto const l_player = player.lock();
            auto const jeep_entity_pos = AK::convert_3d_to_2d(entity->transform->get_position());
            auto const player_pos = AK::convert_3d_to_2d(l_player->entity->transform->get_position());

            // Determine if player is close enough to target
            if (m_invincibility_timer <= 0.0f && glm::distance(jeep_entity_pos, player_pos) < light_range)
            {
                m_targeting_player = true;
            }

            if (glm::distance(jeep_entity_pos, player_pos) < m_player_stun_range)
            {
                player.lock()->stun();

                m_targeting_player = false;
                m_invincibility_timer = 5.0f;
            }
        }
    }
    else
    {
        if (m_jeep_sound != nullptr)
        {
            m_jeep_sound->stop_with_fade(2000.0f);
            m_sound_was_spawned = false;
        }
    }
}

#if EDITOR
void Jeep::draw_editor()
{
    Component::draw_editor();

    ImGuiEx::InputFloat("Acceleration", &acceleration);
    ImGuiEx::InputFloat("Deceleration", &deceleration);
    ImGuiEx::InputFloat("Maximum speed", &maximum_speed);
    ImGuiEx::draw_ptr("Truther", player);
}
#endif

glm::vec2 Jeep::get_speed() const
{
    return m_speed;
}

void Jeep::handle_input()
{
}

void Jeep::find_new_target()
{
    auto path = entity->get_component<Path>();

    m_point_on_path += 0.0004f * direction;

    if (m_point_on_path > 1.0f)
    {
        m_point_on_path = 0.0f;
    }

    if (m_point_on_path < 0.0f)
    {
        m_point_on_path = 1.0f;
    }

    m_target = entity->get_component<Path>()->get_point_at(m_point_on_path);
}
