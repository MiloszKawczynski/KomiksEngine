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
    m_timer += delta_time;

    m_flash = std::pow(std::sin(m_timer), 2);

    entity->get_component<Quad>()->set_alpha(m_flash);
}

#if EDITOR
void WheatOverlay::draw_editor()
{
    Component::draw_editor();
}
#endif
