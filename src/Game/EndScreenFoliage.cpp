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
#include <Komiks/CowManager.h>
#include <glm/gtx/easing.hpp>

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

    percentage_text.lock()->color = 0xFF007800;

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
                    Clock::get_instance()->update_visibility(false);
                    entity->destroy_immediate();
                    return;
                }
            }
        }
    }

    if (m_timer == 0.0f)
    {
        Sound::play_sound("./res/audio/Komiks/dzyngiel.mp3");
    }

    m_timer += delta_time;

    float t = glm::clamp(m_timer / 6.0f, 0.0f, 1.0f); // progress [0,1]
    float eased_t = glm::quarticEaseIn(t); // smooth it

    percentage = glm::mix(percentage, percentage_gained, eased_t); // interpolate

    float scale = percentage / 100; //glm::mix(percentage / 100, 0.0f, 0.95f);

    percentage_text.lock()->set_text(std::to_string(static_cast<int>(percentage)) + "%");
    percentage_text.lock()->font_size = (percentage / 100.0f) * 3 * 40 + 40;
    percentage_bar.lock()->entity->transform->set_local_position({0.0, -0.95f * (1 - scale), 0.0f});
    percentage_bar.lock()->entity->transform->set_local_scale({0.8f, scale, 1.0f});

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
    ImGuiEx::draw_ptr("percentage text", percentage_text);
    ImGuiEx::draw_ptr("percentage bar", percentage_bar);
}
#endif

void EndScreenFoliage::update_background()
{
    entity->get_component<Panel>()->background_path = m_win_background_path;

    entity->get_component<Panel>()->reprepare();
}

void EndScreenFoliage::next_level()
{
    CowManager::get_instance()->clear_map();
    CowManager::get_instance()->setup_level();
    hide();
}

void EndScreenFoliage::hide()
{
    glfwSetInputMode(Engine::window->get_glfw_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    m_is_hiding = true;
    m_is_in_screen = false;

    m_appear_counter = 0.0f;
}
