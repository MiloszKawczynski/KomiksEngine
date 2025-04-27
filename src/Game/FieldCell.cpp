#include "FieldCell.h"

#include "AK/AK.h"
#include "Model.h"
#include "SceneSerializer.h"
#include "imgui_extensions.h"

class Model;

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
}

void FieldCell::awake()
{
    Component::awake();

    set_can_tick(true);

    m_field_grid_ref = entity->transform->parent.lock()->entity.lock()->get_component<FieldGrid>();
    if (m_field_grid_ref != nullptr)
        Debug::log("FieldGrid in a FieldCell component instance is NULL!", DebugType::Error);

    i32 wheat_count = 16;
    for (i32 i = 0; i < wheat_count; i++)
    {
        float const random_offset_1 = AK::random_float(0.45f, 0.55f);
        float const random_offset_2 = AK::random_float(0.45f, 0.55f);

        auto const wheat = AK::random_bool() ? SceneSerializer::load_prefab("Wheat") : SceneSerializer::load_prefab("Wheat2");
        float x = (i % 4) * 0.18f - m_cell_size_and_offset * random_offset_1; // X position (8 per row)
        float z = (i / 4) * 0.18f - m_cell_size_and_offset * random_offset_2; // Z position (new row every 8)
        wheat->transform->set_local_position({x, 0.0f, z});
        wheat->transform->set_parent(entity->transform);
        wheat->is_serialized = false;
        glm::vec3 const e = wheat->transform->get_euler_angles();
        wheat->transform->set_euler_angles({e.x, AK::random_float(0.0f, 360.0f), e.z});
        m_wheats.push_back(wheat->get_component<Wheat>());
    }
}

void FieldCell::update()
{
    if (is_cell_bended(35))
    {
        m_field_grid_ref->set_cell(1, m_id);
        // auto x = Debug::draw_debug_box(entity->transform->get_position(), {}, {0.69f, 0.69f, 0.69f}, 0.5f);
        // x->get_component<Model>()->set_rasterizer_draw_type(RasterizerDrawType::Solid);
    }

    if (!is_cell_bended(35))
    {
        m_field_grid_ref->set_cell(0, m_id);
    }
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
