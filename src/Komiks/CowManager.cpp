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
#include <Game/EndScreenFoliage.h>
#include <Globals.h>

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

    // TODO: Change to proper sound path
    Sound::play_sound("./res/audio/ost_loop.wav", true);

    get_spawn_paths();

    spawn_truther();

    spawn_cow();
    spawn_cow();
    spawn_cow();

    spawn_ufo();

    spawn_jeep();

    time = map_time;
}

void CowManager::update()
{
    event_timer += delta_time;

    if (event_timer >= 40.0f)
    {
        activate_ufo();
        event_timer = 0.0f;
    }

    std::string min = "00";
    std::string sec = "00";
    AK::extract_time(time, min, sec);

    auto const clock_locked = clock_text_ref.lock();
    clock_locked->set_text(min + ":" + sec);

    clock_locked->color = 0xFFD6856B;
    clock_locked->font_size = 65;

    if (time > 0.0f)
    {
        time -= delta_time;
    }
    else
    {
        if (!does_level_ended)
        {
            end_level();
            does_level_ended = true;
        }
    }
}

#if EDITOR
void CowManager::draw_editor()
{
    Component::draw_editor();

    ImGui::Text((std::to_string(event_timer)).c_str());
    ImGuiEx::draw_ptr("Wheat Overlay", wheat_overlay);
    ImGuiEx::draw_ptr("clock text ref", clock_text_ref);
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

void CowManager::spawn_truther()
{
    std::shared_ptr<Entity> new_truther = SceneSerializer::load_prefab("Truther");

    glm::vec2 spawn_position = glm::vec2(0.0f, 4.5f);

    new_truther->transform->set_local_position({spawn_position.x, 0.0f, spawn_position.y});

    auto const truther_comp = new_truther->get_component<Truther>();
    truther = truther_comp;
    truther.lock()->wheat_overlay = wheat_overlay;
}

void CowManager::spawn_cow()
{
    std::shared_ptr<Entity> cow = SceneSerializer::load_prefab("Cow");

    std::weak_ptr<Path> const path = paths[std::rand() % paths.size()];
    glm::vec2 spawn_position = path.lock()->get_point_at(glm::linearRand(0.0f, 1.0f));

    cow->transform->set_local_position({spawn_position.x, 0.0f, spawn_position.y});

    auto const cow_comp = cow->get_component<Cow>();
    cows.push_back(cow_comp);

    cow_comp->cow_manager = std::static_pointer_cast<CowManager>(shared_from_this());
}

void CowManager::spawn_ufo()
{
    std::shared_ptr<Entity> new_ufo = SceneSerializer::load_prefab("UFO");

    glm::vec2 spawn_position = glm::vec2((rand() % 2 == 0 ? -9.0f : 9.0), (rand() % 2 == 0 ? -7.0f : 7.0f));

    new_ufo->transform->set_local_position({spawn_position.x, 2.0f, spawn_position.y});

    auto const ufo_comp = new_ufo->get_component<UFO>();
    ufo = ufo_comp;
    ufo.lock()->truther = truther;
}

void CowManager::spawn_jeep()
{
    std::shared_ptr<Entity> new_jeep = SceneSerializer::load_prefab("JeepAndReflector");

    glm::vec2 spawn_position = glm::vec2((rand() % 2 == 0 ? -9.0f : 9.0), (rand() % 2 == 0 ? -7.0f : 7.0f));

    new_jeep->transform->set_local_position({spawn_position.x, 0.0f, spawn_position.y});

    auto const jeep_comp = new_jeep->transform->children[0]->entity.lock()->get_component<Jeep>();
    jeep = jeep_comp;
    jeep.lock()->player = truther;
}

void CowManager::activate_ufo()
{
    ufo.lock()->choose_position();
}

void CowManager::end_level()
{
    auto end_screen = SceneSerializer::load_prefab("EndScreenFoliage");
    end_screen->get_component<EndScreenFoliage>()->update_background();
}
