#pragma once

#include "AK/Badge.h"
#include "AK/Types.h"
#include "Button.h"
#include "Component.h"
#include "Panel.h"
#include "Popup.h"
#include "ScreenText.h"
#include "Sound.h"

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
    float percentage = 0.0f;
    float percentage_gained = 0.0f;
    std::weak_ptr<ScreenText> percentage_text = {};
    std::weak_ptr<Panel> percentage_bar = {};

private:
    bool m_is_animation_end = false;

    std::string m_win_background_path = "./res/textures/UI/UFOend_lvl.png";
    float m_timer = 0.0f;
};
