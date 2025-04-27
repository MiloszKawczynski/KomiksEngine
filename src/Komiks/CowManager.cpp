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
#include <Quad.h>

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
    flash_at_start_timer = 3.5f;

    for (auto& wheat : Wheat::all_wheat)
    {
        wheat.lock()->set_bended(false);
    }

    truther.lock()->entity->destroy_immediate();

    if (!jeep.expired())
    {
        jeep.lock()->entity->transform->parent.lock()->entity.lock()->destroy_immediate();
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

    cows.clear();
}

void CowManager::awake()
{
    set_can_tick(true);
}

void CowManager::start()
{
    auto const sound = Sound::play_sound("./res/audio/Komiks/foliarzostpropozycja3.mp3", true);

    get_spawn_paths();

    setup_level();
}

void CowManager::setup_level()
{
    does_level_ended = false;

    has_player_moved_this_level = false;

    spawn_truther();

    event_timer = 0.0f;

    switch (m_level)
    {
    case (0):
    {
        spawn_cow();
        spawn_jeep();
        activate_jeep();
        time = 31.0f;
        set_pattern(0);
        dialogue_prompt_controller.lock()->play_content(0);
        m_event_time_threshold = 10.0f;
        m_wasd_prompt = SceneSerializer::load_prefab("KomiksWASDPrompt");
        m_wasd_prompt.lock()->transform->set_position({0.162, 1.5f, 2.5f});
        m_wasd_prompt.lock()->transform->set_parent(entity->transform);

        break;
    }

    case (1):
    {
        spawn_cow();
        spawn_cow();
        spawn_jeep();
        time = 61.0f;
        m_event_time_threshold = 15.0f;
        set_pattern(2);
        dialogue_prompt_controller.lock()->play_content(1);
        m_space_prompt = SceneSerializer::load_prefab("KomiksSpacePrompt");
        m_space_prompt.lock()->transform->set_position({0.162, 1.5f, 2.5f});
        m_space_prompt.lock()->transform->set_parent(entity->transform);
        m_space_shown = true;
        break;
    }

    case (2):
    {
        spawn_cow();
        spawn_cow();
        spawn_ufo();
        spawn_jeep();
        activate_jeep();
        m_event_time_threshold = 20.0f;

        time = 91.0f;

        set_pattern(1);
        break;
    }
    default:
        break;
    }
}

void CowManager::update()
{
    if (m_frame_skip > 0)
    {
        m_frame_skip -= 1;
        return;
    }

    if (flash_at_start_timer > 0.0f)
    {
        flash_at_start_timer -= delta_time;

        wheat_overlay.lock()->is_flash_on = true;
    }

    event_timer += delta_time;

    if (event_timer >= 5.0f && m_level == 0)
        dialogue_prompt_controller.lock()->end_content();

    if (!m_wasd_prompt.expired() && event_timer >= 5.0f && m_level == 0)
        m_wasd_prompt.lock()->destroy_immediate();

    if (event_timer >= 5.0f && m_level == 1)
        dialogue_prompt_controller.lock()->end_content();

    if (!m_shift_shown && event_timer >= 8.0f && m_level == 0)
    {
        m_shift_prompt = SceneSerializer::load_prefab("KomiksShiftPrompt");
        m_shift_prompt.lock()->transform->set_position({0.15, 1.5f, 2.5f});
        m_shift_prompt.lock()->transform->set_parent(entity->transform);
        m_shift_shown = true;
    }

    if (!m_shift_prompt.expired() && event_timer >= 12.0f && m_level == 0)
        m_shift_prompt.lock()->destroy_immediate();

    Debug::log(std::to_string(event_timer));

    if (!m_space_prompt.expired() && event_timer >= 5.0f && m_level == 1)
        m_space_prompt.lock()->destroy_immediate();

    if (event_timer >= m_event_time_threshold && !does_level_ended)
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
            activate_ufo();
            change_jeep_direction();
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

    clock_locked->color = 0xFF007800;
    clock_locked->font_size = 65;

    if (time > 0.0f)
    {
        if (has_player_moved_this_level)
        {
            time -= delta_time;
        }
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
    ImGuiEx::draw_ptr("Dialogue Prompt Controller", dialogue_prompt_controller);
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

    glm::vec2 spawn_position = glm::vec2(0.0f, -4.5f);

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

    glm::vec2 spawn_position = glm::vec2((rand() % 2 == 0 ? -15.0f : 15.0), (rand() % 2 == 0 ? -15.0f : 15.0f));

    new_ufo->transform->set_local_position({spawn_position.x, 2.0f, spawn_position.y});

    auto const ufo_comp = new_ufo->get_component<UFO>();
    ufo = ufo_comp;
    ufo.lock()->truther = truther;
}

void CowManager::spawn_jeep()
{
    std::shared_ptr<Entity> new_jeep = SceneSerializer::load_prefab("JeepAndReflector");

    glm::vec2 spawn_position = glm::vec2((rand() % 2 == 0 ? -9.0f : 9.0f), (rand() % 2 == 0 ? -7.0f : 7.0f));

    auto const jeep_comp = new_jeep->transform->children[0]->entity.lock()->get_component<Jeep>();
    jeep = jeep_comp;
    jeep.lock()->player = truther;

    jeep_comp->entity->transform->set_local_position({spawn_position.x, 0.0f, spawn_position.y});
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

void CowManager::set_pattern(u32 id)
{
    auto quad = wheat_overlay.lock()->entity->get_component<Quad>();
    quad->path = "./res/textures/UI/overlay0" + std::to_string(id) + ".png";
    quad->reprepare();

    friel_grid.lock()->set_pattern(id);
}
