#include "FieldGrid.h"

#include "AK/Math.h"
#include "Collider2D.h"
#include "FieldCell.h"
#include "imgui_extensions.h"

#include <algorithm>

std::shared_ptr<FieldGrid> FieldGrid::create()
{
    return std::make_shared<FieldGrid>(AK::Badge<FieldGrid> {});
}

FieldGrid::FieldGrid(AK::Badge<FieldGrid>)
{
}

void FieldGrid::initialize()
{
    Component::initialize();
    m_grid_rows.resize(rows_number);
    generate_grid_colliders();
}

#if EDITOR
void FieldGrid::draw_editor()
{
    Component::draw_editor();
    ImGui::SliderInt("Rows", &rows_number, 1, 99);
    ImGui::DragFloat("Punishment multiplier", &punishment_multiplier, 0.01f, 0.0f, 1.0f);
    Debug::log(std::to_string(calculate_faked_similarity()));
}
#endif

void FieldGrid::set_cell(bool value, u32 x, u32 y)
{
    m_current_field_status[rows_number * y + x] = value;
}

bool FieldGrid::get_cell_value(u32 x, u32 y)
{
    return m_current_field_status[rows_number * y + x];
}

float FieldGrid::calculate_faked_similarity()
{
    float similarity = 0.0f;
    u32 good = 0;

    u32 downed = 0;
    u32 downed_reference = 0;

    u32 const all = rows_number * rows_number;

    for (u32 i = 0; i < m_current_field_status.size(); i++)
    {
        if (m_current_field_status[i] == m_patterns[m_selected_pattern_id][i])
            good++;

        if (m_patterns[m_selected_pattern_id][i] == true)
            downed_reference++;

        if (m_current_field_status[i])
            downed++;
    }

    similarity = static_cast<float>(good) / static_cast<float>(all);
    similarity -= abs(static_cast<float>(downed_reference) - static_cast<float>(downed)) * punishment_multiplier;
    similarity = std::clamp(similarity, 0.0f, 1.0f);

    // similarity = AK::Math::map_range_clamped(0.5f, 1.0f, 0.0f, 1.0f, similarity);
    return similarity * 100.0f; // %
}

void FieldGrid::generate_grid_colliders()
{
    for (u32 i = 0; i < rows_number; i++)
    {
        for (u32 j = 0; j < rows_number; j++)
        {
            auto const cell_entity = Entity::create("cell_collider");
            auto const grid_ptr = std::make_shared<pattern>(m_current_field_status);
            cell_entity->add_component(FieldCell::create(j * rows_number + i, cell_size_and_offset));
            // auto const col_2d = cell_entity->add_component(Collider2D::create({cell_size_and_offset, cell_size_and_offset}));
            // col_2d->is_trigger = true;
            cell_entity->transform->set_parent(entity->transform);
            cell_entity->is_serialized = false;

            glm::vec3 pos = cell_entity->transform->get_local_position();
            pos = pos + glm::vec3(j * cell_size_and_offset, 0.0f, i * cell_size_and_offset);
            cell_entity->transform->set_local_position(pos);
        }
    }
}
