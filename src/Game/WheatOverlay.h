#pragma once

#include "Component.h"

#include "AK/Badge.h"
#include "PointLight.h"

class WheatOverlay final : public Component
{
public:
    static std::shared_ptr<WheatOverlay> create();

    explicit WheatOverlay(AK::Badge<WheatOverlay>);
#if EDITOR
    virtual void draw_editor() override;
#endif

    virtual void awake() override;
    virtual void update() override;

private:
    float m_flash = 0.0f;
    float m_timer = 0.0f;
};
