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

        m_flash = std::pow(std::sin(m_timer), 2);

        set_overlay_alpha(m_flash);
    }
    else
    {
        set_overlay_alpha(0.0f);
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
