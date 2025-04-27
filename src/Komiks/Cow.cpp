#include "Cow.h"

#include "AK/AK.h"
#include "AK/Math.h"
#include "Camera.h"
#include "Collider2D.h"
#include "CowManager.h"
#include "Entity.h"
#include "Game/Truther.h"
#include "Globals.h"
#include "Model.h"
#include "Wheat.h"

#include <glm/gtc/random.hpp>

#if EDITOR
#include "imgui_extensions.h"
#endif

std::shared_ptr<Cow> Cow::create()
{
    return std::make_shared<Cow>(AK::Badge<Cow> {});
}

Cow::Cow(AK::Badge<Cow>)
{
}

void Cow::awake()
{
    set_can_tick(true);

    m_collider = entity->get_component<Collider2D>();
}

void Cow::start()
{
    if (cow_manager.expired())
    {
        return;
    }

    auto child = Entity::create("ModelChild");
    child->transform->set_parent(entity->transform);

    i32 rand = AK::random_int(0, 1);

    if (rand == 0)
    {
        child->add_component<Model>(Model::create("./res/models/cow/cow.gltf", default_material));
    }
    else
    {
        child->add_component<Model>(Model::create("./res/models/cow_brown/cow.gltf", default_material));
    }

    child->transform->set_local_position({-0.013f, 0.0f, -0.046f});
    child->transform->set_euler_angles({0.0f, 90.0f, 0.0f});
    child->transform->set_local_scale({1.65f, 1.65f, 1.65f});

    auto const l_cow_manager = cow_manager.lock();

    m_destination = l_cow_manager->get_random_position_with_minimal_distance(entity->transform->get_position());
}

void Cow::fixed_update()
{
    Component::fixed_update();

    if (!is_sucked)
    {
        if (m_height == 0.0f)
        {
            if (m_stopped_timer > 0.0f)
            {
                m_stopped_timer -= delta_time;
            }
            else
            {
                entity->transform->set_position(AK::convert_2d_to_3d(
                    AK::move_towards(AK::convert_3d_to_2d(entity->transform->get_position()), m_destination, 0.3f * delta_time)));
            }
        }
        else
        {
            if (m_height != 0.0f)
            {
                m_velocity -= m_gravitation;
            }

            if (m_height < 0.0f)
            {
                m_height = 0.0f;
                m_velocity = 0.0f;
            }

            m_height += m_velocity;
            entity->transform->set_local_position(
                {entity->transform->get_local_position().x, m_height, entity->transform->get_local_position().z});
        }
    }
    else
    {
        if (m_height < 1.25f)
        {
            m_height += 0.01f;
        }

        entity->transform->set_local_position(
            {entity->transform->get_local_position().x, m_height, entity->transform->get_local_position().z});
    }

    static float target_epsilon = 0.05f;
    if (glm::distance(AK::convert_3d_to_2d(entity->transform->get_position()), m_destination) < target_epsilon)
    {
        if (cow_manager.expired())
        {
            return;
        }

        auto const l_cow_manager = cow_manager.lock();

        m_destination = l_cow_manager->get_random_position_with_minimal_distance(entity->transform->get_position());
        m_stopped_timer = glm::linearRand(m_time_to_stop_range.x, m_time_to_stop_range.y);
    }

    auto const cow_position = entity->transform->get_position();
    for (auto& wheat : Wheat::all_wheat)
    {
        auto const l_wheat = wheat.lock();

        if (glm::distance(cow_position, l_wheat->entity->transform->get_position()) < 0.5f)
        {
            l_wheat->set_bended(false, AK::convert_3d_to_2d(l_wheat->entity->transform->get_position() - cow_position));
        }
    }

    // Mincing

    if (m_stopped_timer <= 0.0f)
    {
        glm::vec3 pos = entity->transform->get_position();
        float const sin_mapped_value = AK::Math::map_range_clamped(-1.0f, 1.0f, -0.05f, 0.05f, sin(glfwGetTime() * 8.0f));
        pos.y += sin_mapped_value;
        entity->transform->set_position(pos);
    }

    // Rotating

    glm::vec2 const direction = m_destination - AK::convert_3d_to_2d(entity->transform->get_position());
    float rotation = atan2(direction.x, direction.y) * (180.0f / 3.14f) - 90.0f;
    float actual_rotation = entity->transform->get_euler_angles().y;

    while (rotation > 180.0f)
        rotation -= 360.0f;
    while (rotation < -180.0f)
        rotation += 360.0f;

    while (actual_rotation > 180.0f)
        actual_rotation -= 360.0f;
    while (actual_rotation < -180.0f)
        actual_rotation += 360.0f;

    float delta = rotation - actual_rotation;
    while (delta > 180.0f)
        delta -= 360.0f;
    while (delta < -180.0f)
        delta += 360.0f;

    float new_rotation = actual_rotation + delta * 0.05f;

    entity->transform->set_euler_angles({0.0f, new_rotation, 0.0f});
}

void Cow::on_collision_enter(std::shared_ptr<Collider2D> const& other)
{
    auto const truther = other->entity->get_component<Truther>();

    if (truther == nullptr)
    {
        return;
    }

    auto speed = truther->get_speed();

    auto const collider_locked = m_collider.lock();
    collider_locked->add_force(speed * 0.05f);
    collider_locked->velocity = glm::clamp(collider_locked->velocity, {-3.0f, -3.0f}, {3.0f, 3.0f});

    // Choose another random destination after being poked by the player

    if (cow_manager.expired())
    {
        return;
    }

    auto const l_cow_manager = cow_manager.lock();

    m_destination = l_cow_manager->get_random_position_with_minimal_distance(entity->transform->get_position());
}

void Cow::on_trigger_enter(std::shared_ptr<Collider2D> const& other)
{
    auto const jeep = other->entity->get_component<Jeep>();

    if (jeep == nullptr)
    {
        return;
    }

    auto speed = jeep->get_speed();

    auto const collider_locked = m_collider.lock();
    collider_locked->add_force(speed * 0.6f);
    collider_locked->velocity = glm::clamp(collider_locked->velocity, {-3.0f, -3.0f}, {3.0f, 3.0f});

    // Choose another random destination after being poked by jeep

    if (cow_manager.expired())
    {
        return;
    }

    auto const l_cow_manager = cow_manager.lock();

    m_destination = l_cow_manager->get_random_position_with_minimal_distance(entity->transform->get_position());
}

void Cow::play_sound_per_chance(float const chance) const
{
    if (glm::linearRand(0.0f, 1.0f) < chance)
    {
        i32 rand = AK::random_int(1, 8);
        auto sound = Sound::play_sound("./res/audio/Komiks/krowa" + std::to_string(rand) + ".wav");
        sound->set_volume(1.6f);
    }
}
