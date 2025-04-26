#include "FieldGrid.h"

#include "AK/Math.h"
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
    m_grid_rows.resize(m_rows_number);
    Debug::log(std::to_string(calculate_faked_similarity()));
}

#if EDITOR
void FieldGrid::draw_editor()
{
    Component::draw_editor();
    ImGui::SliderInt("Rows", &m_rows_number, 1, 99);
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();

    ImGui::DragFloat("Punishment multiplier", &punishment_multiplier, 0.01f, 0.0f, 1.0f);
    Debug::log(std::to_string(calculate_faked_similarity()));

    for (i32 i = 0; i < m_rows_number; i++)
    {
        ImGuiEx::draw_ptr("Row " + std::to_string(i + 1), m_grid_rows[i]);
    }
}
#endif

void FieldGrid::set_cell(bool value, u32 x, u32 y)
{
    m_current_field_status[m_rows_number * y + x] = value;
}

bool FieldGrid::get_cell_value(u32 x, u32 y)
{
    return m_current_field_status[m_rows_number * y + x];
}

float FieldGrid::calculate_faked_similarity()
{
    float similarity = 0.0f;
    u32 good = 0;

    u32 downed = 0;
    u32 downed_reference = 0;

    u32 const all = m_rows_number * m_rows_number;

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
