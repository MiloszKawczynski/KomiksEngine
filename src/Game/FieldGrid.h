#pragma once
#include "AK/Badge.h"
#include "AK/Types.h"
#include "Component.h"
#include "Entity.h"

class FieldGrid final : public Component
{
public:
    static std::shared_ptr<FieldGrid> create();
    virtual void initialize() override;
    explicit FieldGrid(AK::Badge<FieldGrid>);

#if EDITOR
    virtual void draw_editor() override;
#endif

    void set_cell(bool value, u32 x, u32 y);
    bool get_cell_value(u32 x, u32 y);
    float calculate_faked_similarity(); // In percent

    float punishment_multiplier = 0.08f;

private:
    void generate_grid_colliders();

    i32 m_rows_number = 13;
    float cell_size_and_offset = 9.0f / 13.0f;
    std::vector<std::weak_ptr<Entity>> m_grid_rows = {};

    // A DATABASE OF REFERENCE GRID PATTERNS
    // This is a vector containing patterns (vectors of 0s and 1s)
    std::vector<pattern> m_patterns = {{
        0, 0, 0, 0, 0, 0, 0, 0, 0, //
        0, 0, 1, 0, 0, 0, 1, 0, 0, //
        0, 0, 1, 0, 0, 0, 1, 0, 0, //
        0, 0, 0, 0, 0, 0, 0, 0, 0, //
        0, 0, 0, 0, 0, 0, 0, 0, 0, //
        0, 1, 0, 0, 0, 0, 0, 1, 0, //
        0, 1, 0, 0, 0, 0, 0, 1, 0, //
        0, 0, 1, 1, 1, 1, 1, 0, 0, //
        0, 0, 0, 0, 0, 0, 0, 0, 0, //
    }};
    u32 m_selected_pattern_id = 0;

    // clang-format off
    pattern m_current_field_status =
    {
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, //
    0, 0, 0, 0, 1, 1, 0, 0, 0, //
    0, 0, 1, 1, 0, 1, 1, 0, 0, //
    0, 1, 1, 0, 0, 0, 1, 1, 0, //
    0, 0, 0, 0, 0, 0, 1, 1, 0, //
    0, 0, 1, 1, 0, 1, 1, 0, 0, //
    0, 0, 1, 0, 0, 1, 0, 0, 0, //
    0, 0, 1, 1, 1, 1, 0, 0, 0, //
    0, 0, 0, 0, 0, 0, 0, 0, 0, //
}
    };
    // clang-format on
};
