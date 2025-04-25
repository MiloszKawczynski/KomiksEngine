#pragma once
#include "AK/Badge.h"
#include "Component.h"
#include "FieldGrid.h"

class FieldCell : public Component
{
public:
    static std::shared_ptr<FieldCell> create();
    explicit FieldCell(AK::Badge<FieldCell>);
    virtual void initialize() override;

#if EDITOR
    virtual void draw_editor() override;
#endif

private:
    std::weak_ptr<FieldGrid> m_field_grid_ref = {};
};
