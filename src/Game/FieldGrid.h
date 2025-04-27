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
    void set_cell(bool value, u32 id);
    bool get_cell_value(u32 x, u32 y);
    std::weak_ptr<Entity> get_cell();
    float calculate_faked_similarity(); // In percent

    i32 rows_number = 13;
    float punishment_multiplier = 0.1f;
    float punishment_power = 1.3f;

private:
    void generate_grid_colliders();
    float cell_size_and_offset = 9.0f / static_cast<float>(rows_number);
    std::vector<std::weak_ptr<Entity>> m_grid_rows = {};

    // A DATABASE OF REFERENCE GRID PATTERNS
    // This is a vector containing patterns (vectors of 0s and 1s)
    std::vector<pattern> m_patterns = {{
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //
        0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, //
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, //
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, //
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, //
        0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, //
        0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, //
        0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, //
        0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, //
        0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, //
        0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, //
        0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, //
    }};

    // clang-format off
    pattern m_current_field_status =
    {
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
    // clang-format on

    u32 m_selected_pattern_id = 0;
};
