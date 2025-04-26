#include "FieldCell.h"

#include "SceneSerializer.h"
#include "imgui_extensions.h"

std::shared_ptr<FieldCell> FieldCell::create()
{
    return std::make_shared<FieldCell>(AK::Badge<FieldCell> {});
}

std::shared_ptr<FieldCell> FieldCell::create(std::shared_ptr<std::vector<bool>> const& grid_ref, u32 id, float cell_size_and_offset)
{
    return std::make_shared<FieldCell>(AK::Badge<FieldCell> {}, grid_ref, id, cell_size_and_offset);
}

FieldCell::FieldCell(AK::Badge<FieldCell>, std::shared_ptr<std::vector<bool>> const& grid_ref, u32 id, float cell_size_and_offset)
    : grid_ref(grid_ref), id(id), cell_size_and_offset(cell_size_and_offset)
{
}

FieldCell::FieldCell(AK::Badge<FieldCell>)
{
}

void FieldCell::initialize()
{
    Component::initialize();

    // if (entity->transform->parent.expired() || entity->transform->parent.lock()->parent.expired())
    //     return;
    //
    // m_field_grid_ref = entity->transform->parent.lock()->parent.lock()->entity.lock()->get_component<FieldGrid>();
    //
    // if (m_field_grid_ref.expired())
    //     Debug::log("FieldGrid in a FieldCell component instance is NULL!", DebugType::Error);

    i32 wheat_count = 16;
    for (i32 i = 0; i < wheat_count; i++)
    {
        auto wheat = SceneSerializer::load_prefab("Wheat");
        float x = (i % 4) * 0.18f - cell_size_and_offset * 0.5f; // X position (8 per row)
        float z = (i / 4) * 0.18f - cell_size_and_offset * 0.5f; // Z position (new row every 8)
        wheat->transform->set_local_position({x, 0.0f, z});
        wheat->transform->set_parent(entity->transform);
        wheat->is_serialized = false;
    }
}

#if EDITOR
void FieldCell::draw_editor()
{
    Component::draw_editor();
    ImGui::Text("Don't set this up manually.");
    ImGuiEx::draw_ptr("Preview of FieldGrid Component", m_field_grid_ref);
}
#endif
