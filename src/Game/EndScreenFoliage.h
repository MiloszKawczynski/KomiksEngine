#pragma once

#include "AK/Badge.h"
#include "AK/Types.h"
#include "Button.h"
#include "Component.h"
#include "Popup.h"

class EndScreenFoliage final : public Popup
{
public:
    static std::shared_ptr<EndScreenFoliage> create();

    explicit EndScreenFoliage(AK::Badge<EndScreenFoliage>);

    virtual void awake() override;
    virtual void update() override;
    virtual void on_enabled() override;
    virtual void on_disabled() override;
#if EDITOR
    virtual void draw_editor() override;
#endif
    void update_background();

    void next_level();

    virtual void hide() override;

    std::weak_ptr<Button> next_level_button = {};

private:
    bool m_is_animation_end = false;

    std::string m_win_background_path = "./res/textures/UI/UFOend_lvl.png";
};
