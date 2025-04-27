#include "FieldGrid.h"

#include "AK/Math.h"
#include "Collider2D.h"
#include "FieldCell.h"
#include "glm/ext/quaternion_exponential.hpp"
#include "imgui_extensions.h"

#include <algorithm>
#include <glm/gtc/random.hpp>

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
    ImGui::DragFloat("Punishment multiplier", &punishment_multiplier, 0.001f, 0.0f, 0.1f);
    ImGui::DragFloat("Punishment power", &punishment_power, 0.01f, 0.0f, 3.0f);
    Debug::log(std::to_string(calculate_faked_similarity()), DebugType::Warning);

    // std::vector<bool> pattern = m_patterns[m_selected_pattern_id];
    // for (u32 i = 0; i < pattern.size(); i++)
    // {
    //     if (pattern[i] == true)
    //         Debug::draw_debug_box(entity->transform->children[i]->get_position(), {}, {0.69f, 0.69f, 0.69f}, 0.5f);
    // }
}
#endif

void FieldGrid::set_cell(bool value, u32 x, u32 y)
{
    m_current_field_status[rows_number * y + x] = value;
}

void FieldGrid::set_cell(bool value, u32 id)
{
    m_current_field_status[id] = value;
}

bool FieldGrid::get_cell_value(u32 x, u32 y)
{
    return m_current_field_status[rows_number * y + x];
}

std::weak_ptr<Entity> FieldGrid::get_cell()
{
    return entity->transform->children.at(glm::linearRand(0, rows_number * rows_number - 1))->entity;
}

float FieldGrid::calculate_faked_similarity()
{
    float similarity = 0.0f;

    u32 any_bended = 0;
    u32 correctly_bended_from_pattern = 0;
    u32 bended_in_reference = 0;

    for (u32 i = 0; i < m_current_field_status.size(); i++)
    {
        auto const& ith_cell_field = m_current_field_status[i];
        auto const& ith_cell_pattern = m_patterns[m_selected_pattern_id][i];

        if (ith_cell_field == true)
            any_bended++;

        if (ith_cell_pattern == true)
        {
            bended_in_reference++;

            if (ith_cell_field == true)
                correctly_bended_from_pattern++;
        }
    }

    u32 const cells_of_difference = abs(static_cast<float>(bended_in_reference) - static_cast<float>(any_bended));
    similarity = static_cast<float>(correctly_bended_from_pattern) / static_cast<float>(bended_in_reference);
    similarity -= punishment_multiplier * std::pow(static_cast<float>(cells_of_difference), punishment_power);

    if (similarity >= 0.15)
        similarity *= 1.1f;

    similarity = std::clamp(similarity, 0.0f, 1.0f);

    return similarity * 100.0f; // %
}

void FieldGrid::set_pattern(u32 id)
{
    pattern m_current_field_status = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //
    };

    m_selected_pattern_id = id;
}

void FieldGrid::generate_grid_colliders()
{
    for (u32 i = 0; i < rows_number; i++)
    {
        for (u32 j = 0; j < rows_number; j++)
        {
            auto const cell_entity = Entity::create("cell_collider");
            auto const grid_ptr = std::make_shared<pattern>(m_current_field_status);
            cell_entity->add_component(FieldCell::create(i * rows_number + j, cell_size_and_offset));
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
