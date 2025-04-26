#include "UFO.h"

#include "AK/AK.h"
#include "Camera.h"
#include "Collider2D.h"
#include "Entity.h"
#include "Game/Truther.h"
#include "Globals.h"
#include "Wheat.h"
#include "glm/gtx/easing.hpp"

#include <glm/gtc/random.hpp>

#if EDITOR
#include "imgui_extensions.h"
#endif

std::shared_ptr<UFO> UFO::create()
{
    return std::make_shared<UFO>(AK::Badge<UFO> {});
}

UFO::UFO(AK::Badge<UFO>)
{
}

void UFO::awake()
{
    set_can_tick(true);
}

void UFO::start()
{
    m_destination = {0.0f, 2.0f, 0.0f};
    m_start_position = entity->transform->get_position();
    m_move_timer = 0.0f;
}

void UFO::update()
{
    if (m_move_timer < m_move_duration)
    {
        m_move_timer += delta_time * 0.5f;

        float t = glm::clamp(m_move_timer / m_move_duration, 0.0f, 1.0f); // progress [0,1]
        float eased_t = glm::backEaseOut(t); // smooth it

        glm::vec3 new_position = glm::mix(m_start_position, m_destination, eased_t); // interpolate

        entity->transform->set_position(new_position);
    }
    else
    {
        entity->transform->set_position(m_destination); // snap exactly at the end

        for (auto& wheat : Wheat::all_wheat)
        {
            auto const l_wheat = wheat.lock();

            if (glm::distance(AK::convert_3d_to_2d(m_destination), AK::convert_3d_to_2d(l_wheat->entity->transform->get_position()))
                < 1.69f)
            {
                l_wheat->set_bended(false, AK::convert_3d_to_2d(l_wheat->entity->transform->get_position() - m_destination));
            }
        }
    }

    glm::vec3 truther_position = truther.lock()->entity->transform->get_local_position();
    glm::vec3 my_position = entity->transform->get_local_position();

    truther.lock()->is_sucked = false;
    if (glm::distance(glm::vec2(truther_position.x, truther_position.z), glm::vec2(my_position.x, my_position.z)) < 1.69f)
    {
        truther.lock()->is_sucked = true;
        truther.lock()->entity->transform->set_local_position(glm::mix(truther_position, my_position, 0.03f));
    }
}

#if EDITOR
void UFO::draw_editor()
{
    Component::draw_editor();

    if (ImGui::Button("Spawn"))
    {
        entity->transform->set_position({6.0f, 2.0f, 0.0f});
        choose_position();
        m_start_position = entity->transform->get_position();
        m_move_timer = 0.0f;
    }

    ImGuiEx::draw_ptr("Field Grid", field_grid);
    ImGuiEx::draw_ptr("Truther", truther);
}
#endif

void UFO::choose_position()
{
    glm::vec3 target = field_grid.lock()->get_cell().lock()->transform->get_local_position();

    m_destination = {target.x, 2.0f, target.z};
}
