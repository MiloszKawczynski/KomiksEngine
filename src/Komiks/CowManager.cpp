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
    auto instance = std::make_shared<CowManager>(AK::Badge<CowManager> {});

    if (m_instance)
    {
        Debug::log("Instance already exists in the scene.", DebugType::Error);
    }

    m_instance = instance;
    return instance;
}

CowManager::CowManager(AK::Badge<CowManager>)
{
}

std::shared_ptr<CowManager> CowManager::get_instance()
{
    return m_instance;
}

void CowManager::clear_map()
{
    for (auto& wheat : Wheat::all_wheat)
    {
        wheat.lock()->set_bended(false);
    }

    truther.lock()->entity->destroy_immediate();

    if (!jeep.expired())
    {
        jeep.lock()->entity->destroy_immediate();
    }

    if (!ufo.expired())
    {
        ufo.lock()->entity->destroy_immediate();
    }

    for (auto& cow : cows)
    {
        if (!cow.expired())
        {
            cow.lock()->entity->destroy_immediate();
        }
    }
}

void CowManager::awake()
{
    set_can_tick(true);

    Sound::play_sound("./res/audio/Komiks/foliarzostpropozycja3.mp3");

    get_spawn_paths();

    does_level_ended = false;

    spawn_truther();

    event_timer = 0.0f;

    switch (m_level)
    {
    case (0):
    {
        spawn_cow();

        time = 20.0f;
        break;
    }

    case (1):
    {
        spawn_cow();
        spawn_jeep();

        time = 120.0f;
        break;
    }

    case (2):
    {
        spawn_cow();
        spawn_ufo();
        spawn_jeep();

        time = 180.0f;
        break;
    }
    default:
        break;
    }
}

void CowManager::update()
{
    event_timer += delta_time;

    if (event_timer >= 40.0f && !does_level_ended)
    {
        switch (m_level)
        {
        case (0):
        {
            break;
        }

        case (1):
        {
            if (jeep.lock()->is_active == false)
            {
                activate_jeep();
            }
            else
            {
                change_jeep_direction();
            }
            break;
        }

        case (2):
        {
            if (jeep.lock()->is_active == false)
            {
                activate_jeep();
            }
            else
            {
                activate_ufo();
                change_jeep_direction();
            }
            break;
        }
        default:
            break;
        }

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
    ImGuiEx::draw_ptr("friel grid", friel_grid);
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

void CowManager::activate_jeep()
{
    jeep.lock()->is_active = true;
}

void CowManager::change_jeep_direction()
{
    jeep.lock()->direction *= -1.0f;
}

void CowManager::end_level()
{
    m_level++;
    does_level_ended = true;

    auto end_screen = SceneSerializer::load_prefab("EndScreenFoliage");
    auto end_screen_comp = end_screen->get_component<EndScreenFoliage>();

    end_screen_comp->update_background();
    end_screen_comp->percentage_gained = friel_grid.lock()->calculate_faked_similarity();
}
