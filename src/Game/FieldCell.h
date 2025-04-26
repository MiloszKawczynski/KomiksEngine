#pragma once
#include "AK/Badge.h"
#include "Component.h"
#include "FieldGrid.h"

class FieldCell : public Component
{
public:
    static std::shared_ptr<FieldCell> create();
    static std::shared_ptr<FieldCell> create(std::shared_ptr<std::vector<bool>> const& grid_ref, u32 id, float cell_size_and_offset);
    explicit FieldCell(AK::Badge<FieldCell>, std::shared_ptr<std::vector<bool>> const& grid_ref, u32 id, float cell_size_and_offset);
    explicit FieldCell(AK::Badge<FieldCell>);
    virtual void initialize() override;

    NON_SERIALIZED
    std::weak_ptr<std::vector<bool>> grid_ref = {};

    NON_SERIALIZED
    u32 id = 0;

#if EDITOR
    virtual void draw_editor() override;
#endif

private:
    std::weak_ptr<FieldGrid> m_field_grid_ref = {};

    float cell_size_and_offset = 0.0f;
};
