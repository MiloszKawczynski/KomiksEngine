#include "FieldCell.h"

#include "imgui_extensions.h"

std::shared_ptr<FieldCell> FieldCell::create()
{
    return std::make_shared<FieldCell>(AK::Badge<FieldCell> {});
}

FieldCell::FieldCell(AK::Badge<FieldCell>)
{
}

void FieldCell::initialize()
{
    Component::initialize();

    if (entity->transform->parent.expired() || entity->transform->parent.lock()->parent.expired())
        return;

    m_field_grid_ref = entity->transform->parent.lock()->parent.lock()->entity.lock()->get_component<FieldGrid>();

    if (m_field_grid_ref.expired())
        Debug::log("FieldGrid in a FieldCell component instance is NULL!", DebugType::Error);
}

#if EDITOR
void FieldCell::draw_editor()
{
    Component::draw_editor();
    ImGui::Text("Don't set this up manually.");
    ImGuiEx::draw_ptr("Preview of FieldGrid Component", m_field_grid_ref);
}
#endif
