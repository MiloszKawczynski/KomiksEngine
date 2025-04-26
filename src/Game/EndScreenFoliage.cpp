#include "EndScreenFoliage.h"

#include "AK/AK.h"
#include "AK/Math.h"
#include "Clock.h"
#include "Entity.h"
#include "GameController.h"
#include "Globals.h"
#include "Input.h"
#include "LevelController.h"
#include "Model.h"
#include "Panel.h"
#include "ResourceManager.h"
#include "SceneSerializer.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/random.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/vec2.hpp>

#if EDITOR
#include "imgui_extensions.h"
#include <imgui.h>
#endif

std::shared_ptr<EndScreenFoliage> EndScreenFoliage::create()
{
    return std::make_shared<EndScreenFoliage>(AK::Badge<EndScreenFoliage> {});
}

EndScreenFoliage::EndScreenFoliage(AK::Badge<EndScreenFoliage>)
{
}

void EndScreenFoliage::awake()
{
    glfwSetInputMode(Engine::window->get_glfw_window(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    update_background();

    Clock::get_instance()->update_visibility(true);

    set_can_tick(true);
}

void EndScreenFoliage::on_enabled()
{
    if (!next_level_button.expired())
    {
        next_level_button.lock()->on_unclicked.attach(&EndScreenFoliage::next_level, shared_from_this());
    }
}

void EndScreenFoliage::on_disabled()
{
    if (!next_level_button.expired())
    {
        next_level_button.lock()->on_unclicked.detach(shared_from_this());
    }
}

void EndScreenFoliage::update()
{
    if (!m_is_animation_end || m_is_hiding)
    {
        if (!m_is_in_screen)
        {
            if (m_appear_counter < 1.0f)
            {
                m_appear_counter += delta_time * 0.75f;
                update_screen_position();
            }
            else
            {
                m_is_in_screen = true;
                m_appear_counter = 1.0f;

                update_screen_position();

                m_appear_counter = 0.0f;

                if (m_is_hiding)
                {
                    entity->destroy_immediate();
                }
            }
        }
    }

    if (Input::input->get_key_down(GLFW_KEY_F4))
    {
        hide();

        // Thanks for playing screen is presented, we don't want to disable the cursor
        glfwSetInputMode(Engine::window->get_glfw_window(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

#if EDITOR
void EndScreenFoliage::draw_editor()
{
    Popup::draw_editor();

    ImGuiEx::draw_ptr("Next level button", next_level_button);
}
#endif

void EndScreenFoliage::update_background()
{
    entity->get_component<Panel>()->background_path = m_win_background_path;

    entity->get_component<Panel>()->reprepare();
}

void EndScreenFoliage::next_level()
{
    hide();
}

void EndScreenFoliage::hide()
{
    glfwSetInputMode(Engine::window->get_glfw_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    m_is_hiding = true;
    m_is_in_screen = false;

    m_appear_counter = 0.0f;
}
