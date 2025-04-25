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

private:
    i32 m_rows_number = 9;

    std::vector<std::weak_ptr<Entity>> m_grid_rows = {};
};
