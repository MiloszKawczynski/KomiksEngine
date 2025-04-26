#include "FieldCell.h"

#include "SceneSerializer.h"
#include "imgui_extensions.h"

std::shared_ptr<FieldCell> FieldCell::create()
{
    return std::make_shared<FieldCell>(AK::Badge<FieldCell> {});
}

std::shared_ptr<FieldCell> FieldCell::create(u32 id, float cell_size_and_offset)
{
    return std::make_shared<FieldCell>(AK::Badge<FieldCell> {}, id, cell_size_and_offset);
}

FieldCell::FieldCell(AK::Badge<FieldCell>, u32 id, float cell_size_and_offset) : m_id(id), m_cell_size_and_offset(cell_size_and_offset)
{
}

FieldCell::FieldCell(AK::Badge<FieldCell>)
{
}

void FieldCell::initialize()
{
    Component::initialize();
    // if (entity->transform->parent.expired())
    //     return;

    i32 wheat_count = 16;
    for (i32 i = 0; i < wheat_count; i++)
    {
        auto wheat = SceneSerializer::load_prefab("Wheat");
        float x = (i % 4) * 0.18f - m_cell_size_and_offset * 0.5f; // X position (8 per row)
        float z = (i / 4) * 0.18f - m_cell_size_and_offset * 0.5f; // Z position (new row every 8)
        wheat->transform->set_local_position({x, 0.0f, z});
        wheat->transform->set_parent(entity->transform);
        wheat->is_serialized = false;
        m_wheats.push_back(wheat->get_component<Wheat>());
    }
}

void FieldCell::awake()
{
    Component::awake();

    set_can_tick(true);

    m_field_grid_ref = entity->transform->parent.lock()->entity.lock()->get_component<FieldGrid>();
    if (m_field_grid_ref != nullptr)
        Debug::log("FieldGrid in a FieldCell component instance is NULL!", DebugType::Error);
}

void FieldCell::update()
{
    Component::update();

    if (is_cell_bended(25) && m_id == 0)
        Debug::log("YES");
    if (!is_cell_bended(25) && m_id == 0)
        Debug::log("NO");
}

bool FieldCell::is_cell_bended(float percent_limit)
{
    u32 bended_num = 0;

    for (auto const& w : m_wheats)
    {
        if (w->is_bended())
            bended_num++;
    }

    float const percent = static_cast<float>(bended_num) / static_cast<float>(m_wheats.size()) * 100.0f;

    return percent >= percent_limit;
}

#if EDITOR
void FieldCell::draw_editor()
{
    Component::draw_editor();
}
#endif
