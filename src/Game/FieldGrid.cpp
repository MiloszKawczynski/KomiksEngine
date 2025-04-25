#include "FieldGrid.h"

#include "imgui_extensions.h"

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
}

#if EDITOR
void FieldGrid::draw_editor()
{
    Component::draw_editor();
    ImGui::SliderInt("Rows", &m_rows_number, 1, 99);
    ImGui::Spacing();

    for (i32 i = 0; i < m_rows_number; i++)
    {
        ImGuiEx::draw_ptr("Row " + std::to_string(i + 1), m_grid_rows[i]);
    }
}
#endif
