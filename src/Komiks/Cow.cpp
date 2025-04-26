#include "Cow.h"

#include "AK/AK.h"
#include "Camera.h"
#include "CowManager.h"
#include "Entity.h"
#include "Globals.h"
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
}

void Cow::start()
{
    if (cow_manager.expired())
    {
        return;
    }

    auto const l_cow_manager = cow_manager.lock();

    m_destination = l_cow_manager->get_random_position_with_minimal_distance(entity->transform->get_position());
}

void Cow::update()
{
    entity->transform->set_position(
        AK::convert_2d_to_3d(AK::move_towards(AK::convert_3d_to_2d(entity->transform->get_position()), m_destination, 0.3f * delta_time)));

    static float target_epsilon = 0.05f;
    if (glm::distance(AK::convert_3d_to_2d(entity->transform->get_position()), m_destination) < target_epsilon)
    {
        if (cow_manager.expired())
        {
            return;
        }

        auto const l_cow_manager = cow_manager.lock();

        m_destination = l_cow_manager->get_random_position_with_minimal_distance(entity->transform->get_position());
    }

    auto const cow_position = entity->transform->get_position();
    for (auto& wheat : Wheat::all_wheat)
    {
        auto const l_wheat = wheat.lock();

        if (glm::distance(cow_position, l_wheat->entity->transform->get_position()) < 0.15f)
        {
            l_wheat->set_bended(false, AK::convert_3d_to_2d(l_wheat->entity->transform->get_position() - cow_position));
        }
    }
}
