#include "CowManager.h"

#include "AK/AK.h"
#include "Entity.h"
#include "Game/GameController.h"
#include "SceneSerializer.h"
#include "glm/gtc/random.hpp"

#include <random>

#if EDITOR
#include "imgui_extensions.h"
#endif

std::shared_ptr<CowManager> CowManager::create()
{
    return std::make_shared<CowManager>(AK::Badge<CowManager> {});
}

CowManager::CowManager(AK::Badge<CowManager>)
{
}

void CowManager::awake()
{
    set_can_tick(true);

    get_spawn_paths();

    spawn_cow();
    spawn_cow();
    spawn_cow();
}

void CowManager::update()
{
}

#if EDITOR
void CowManager::draw_editor()
{
    Component::draw_editor();
}
#endif

void CowManager::get_spawn_paths()
{
    paths.clear();

    for (auto const& path : entity->get_components<Path>())
    {
        paths.emplace_back(path);
    }
}

glm::vec2 CowManager::get_random_position_with_minimal_distance(glm::vec3 current_position) const
{
    i32 i = 0;
    glm::vec2 spawn_position;
    while (true && i < 100)
    {
        std::weak_ptr<Path> const path = paths[std::rand() % paths.size()];
        spawn_position = path.lock()->get_point_at(glm::linearRand(0.0f, 1.0f));

        if (glm::distance(current_position, AK::convert_2d_to_3d(spawn_position)) > 1.0f)
        {
            break;
        }

        i += 1;
    }

    return spawn_position;
}

void CowManager::spawn_cow()
{
    std::shared_ptr<Entity> cow = SceneSerializer::load_prefab("Cow");

    std::weak_ptr<Path> const path = paths[std::rand() % paths.size()];
    glm::vec2 spawn_position = path.lock()->get_point_at(glm::linearRand(0.0f, 1.0f));

    cow->transform->set_local_position({spawn_position.x, 0.0f, spawn_position.y});
    cow->transform->set_parent(GameController::get_instance()->current_scene.lock()->transform);

    auto const cow_comp = cow->get_component<Cow>();
    m_cows.push_back(cow_comp);

    cow_comp->cow_manager = std::static_pointer_cast<CowManager>(shared_from_this());
}
