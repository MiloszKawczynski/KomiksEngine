#include "WheatOverlay.h"

#include "Entity.h"
#include "Game/LevelController.h"
#include "Model.h"
#include "Player.h"
#include "ResourceManager.h"
#include "SceneSerializer.h"

#if EDITOR
#include "imgui_extensions.h"
#endif
#include <Globals.h>
#include <Quad.h>

std::shared_ptr<WheatOverlay> WheatOverlay::create()
{
    return std::make_shared<WheatOverlay>(AK::Badge<WheatOverlay> {});
}

WheatOverlay::WheatOverlay(AK::Badge<WheatOverlay>)
{
}

void WheatOverlay::awake()
{
    set_can_tick(true);
}

void WheatOverlay::update()
{
    if (is_flash_on)
    {
        m_timer += delta_time;

        m_flash = std::pow(std::sin(m_timer), 2) * 0.25 + 0.25;

        if (m_flash < 0.26)
        {
            is_flash_on = false;
        }

        set_overlay_alpha(m_flash);
    }
    else
    {
        if (m_flash > 0.0f)
        {
            m_flash = std::lerp(m_flash, 0.0f, 0.1f);
            set_overlay_alpha(m_flash);
        }
        else
        {
            m_timer = 0.0f;
            set_overlay_alpha(0.0f);
        }
    }
}

void WheatOverlay::set_overlay_alpha(float alpha)
{
    entity->get_component<Quad>()->set_alpha(alpha);
}

#if EDITOR
void WheatOverlay::draw_editor()
{
    Component::draw_editor();
}
#endif
