#pragma once
#include "AK/Badge.h"
#include "Component.h"
#include "FieldGrid.h"
#include "Komiks/Wheat.h"

class FieldCell : public Component
{
public:
    static std::shared_ptr<FieldCell> create();
    static std::shared_ptr<FieldCell> create(u32 id, float cell_size_and_offset);
    explicit FieldCell(AK::Badge<FieldCell>, u32 id, float cell_size_and_offset);
    explicit FieldCell(AK::Badge<FieldCell>);
    virtual void initialize() override;
    virtual void awake() override;
    virtual void update() override;

    // This percent of wheat needs to be bended to treat a cell as bended
    bool is_cell_bended(float percent_limit);

#if EDITOR
    virtual void draw_editor() override;
#endif

private:
    std::shared_ptr<FieldGrid> m_field_grid_ref = {};
    std::vector<std::shared_ptr<Wheat>> m_wheats = {};
    u32 m_id = 0;

    float m_cell_size_and_offset = 0.0f;
};
