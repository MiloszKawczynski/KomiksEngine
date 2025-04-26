#include "Truther.h"

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

std::shared_ptr<Truther> Truther::create()
{
    return std::make_shared<Truther>(AK::Badge<Truther> {});
}

Truther::Truther(AK::Badge<Truther>)
{
}

void Truther::awake()
{
    set_can_tick(true);

    // This is for generating wheat field
    // i32 wheat_count = 2500;
    // auto const wheat_container = Entity::create("WheatContainer");
    // for (i32 i = 0; i < wheat_count; i++)
    // {
    //     auto wheat = SceneSerializer::load_prefab("Wheat");
    //     float x = (i % 50) * 0.18f; // X position (8 per row)
    //     float z = (i / 50) * 0.18f; // Z position (new row every 8)
    //     wheat->transform->set_local_position({x, 0.0f, z});
    //     wheat->transform->set_parent(wheat_container->transform);
    // }
}

void Truther::update()
{
    auto const truther_position = entity->transform->get_position();

    for (auto& wheat : Wheat::all_wheat)
    {
        if (wheat.expired())
        {
            continue;
        }

        auto const l_wheat = wheat.lock();

        if (glm::distance(truther_position, l_wheat->entity->transform->get_position()) < 0.15f)
        {
            l_wheat->set_bended(truther_bends, AK::convert_3d_to_2d(l_wheat->entity->transform->get_position() - truther_position));
        }
    }
}

void Truther::fixed_update()
{
    i32 horizontal = 0;
    i32 vertical = 0;

    if (Input::input->get_key(GLFW_KEY_D))
    {
        horizontal++;
    }
    if (Input::input->get_key(GLFW_KEY_A))
    {
        horizontal--;
    }
    if (Input::input->get_key(GLFW_KEY_W))
    {
        vertical--;
    }
    if (Input::input->get_key(GLFW_KEY_S))
    {
        vertical++;
    }

    wheat_overlay.lock()->is_flash_on = false;
    if (Input::input->get_key(GLFW_KEY_LEFT_SHIFT))
    {
        horizontal = 0;
        vertical = 0;

        wheat_overlay.lock()->is_flash_on = true;
    }

    if (m_height == 0.0f)
    {
        m_speed.x += horizontal * acceleration;
        m_speed.y += vertical * acceleration;
    }
    else
    {
        if (is_sucked)
        {
            m_speed.x += horizontal * acceleration * 0.2f;
            m_speed.y += vertical * acceleration * 0.2f;
        }
    }

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

    if (glm::length(m_speed) > maximum_speed && (m_height == 0.0f || is_sucked))
    {
        m_speed = glm::normalize(m_speed) * maximum_speed;
    }

    if (Input::input->get_key(GLFW_KEY_SPACE) && m_height == 0.0f)
    {
        m_velocity += jump_power;
        m_speed *= jump_horizontal_power;
    }

    if (!is_sucked)
    {
        if (m_height != 0.0f)
        {
            m_velocity -= gravitation;
        }
    }
    else
    {
        suck();
    }

    if (m_height < 0.0f)
    {
        m_height = 0.0f;
        m_velocity = 0.0f;
    }

    m_height += m_velocity;

    glm::vec3 speed_vector = glm::vec3(m_speed.x, 0.0f, m_speed.y);
    speed_vector *= fixed_delta_time;

    entity->transform->set_local_position(entity->transform->get_local_position() + speed_vector);
    entity->transform->set_local_position({entity->transform->get_local_position().x, m_height, entity->transform->get_local_position().z});

    if (glm::length(m_speed) >= maximum_speed * 0.25f)
    {
        float rotation = atan2(speed_vector.x, speed_vector.z) * (180.0f / 3.14f) - 90.0f;
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

    handle_input();
}

#if EDITOR
void Truther::draw_editor()
{
    Component::draw_editor();

    ImGui::Text("Does the truther bend or unbend wheat?");
    ImGui::Text("For testing.");
    ImGui::Checkbox("Bending", &truther_bends);

    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    ImGuiEx::InputFloat("Acceleration", &acceleration);
    ImGuiEx::InputFloat("Deceleration", &deceleration);
    ImGuiEx::InputFloat("Maximum speed", &maximum_speed);
    ImGuiEx::InputFloat("Jump power", &jump_power);
    ImGuiEx::InputFloat("Gravitation", &gravitation);
    ImGuiEx::InputFloat("jump_horizontal_power", &jump_horizontal_power);
    ImGuiEx::InputFloat("suck power", &suck_power);

    ImGuiEx::draw_ptr("Wheat Overlay", wheat_overlay);
}
#endif

void Truther::on_trigger_enter(std::shared_ptr<Collider2D> const& other)
{
}
void Truther::on_trigger_exit(std::shared_ptr<Collider2D> const& other)
{
}

void Truther::on_destroyed()
{
    Component::on_destroyed();
}

glm::vec2 Truther::get_speed() const
{
    return m_speed;
}

void Truther::suck()
{
    if (m_height < 1.25f)
    {
        m_height += suck_power;
    }
}

void Truther::handle_input()
{
}
