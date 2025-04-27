#pragma once

#include "Component.h"
#include "Cow.h"
#include "Game/Path.h"
#include "Komiks/UFO.h"
#include "Sound.h"
#include <Game/FieldGrid.h>
#include <Game/Jeep.h>
#include <Game/WheatOverlay.h>
#include <ScreenText.h>

class UFO;

class CowManager final : public Component
{

public:
    static std::shared_ptr<CowManager> create();

    explicit CowManager(AK::Badge<CowManager>);

    static std::shared_ptr<CowManager> get_instance();
    virtual void awake() override;
    virtual void update() override;
#if EDITOR
    virtual void draw_editor() override;
#endif

    void get_spawn_paths();
    glm::vec2 get_random_position_with_minimal_distance(glm::vec3 current_position) const;

    std::vector<std::weak_ptr<Path>> paths = {};
    std::vector<std::weak_ptr<Cow>> cows = {};
    std::weak_ptr<Truther> truther = {};
    std::weak_ptr<UFO> ufo = {};
    std::weak_ptr<Jeep> jeep = {};
    std::weak_ptr<WheatOverlay> wheat_overlay = {};
    std::weak_ptr<ScreenText> clock_text_ref = {};
    std::weak_ptr<FieldGrid> friel_grid = {};

    NON_SERIALIZED
    float map_time = 180.0f;
    NON_SERIALIZED
    float time = 0.0f;

    float event_timer = 0.0f;

    bool does_level_ended = false;

private:
    void spawn_truther();
    void spawn_cow();
    void spawn_ufo();
    void spawn_jeep();
    void activate_ufo();
    void activate_jeep();
    void change_jeep_direction();
    void end_level();

    inline static std::shared_ptr<CowManager> m_instance;
};
