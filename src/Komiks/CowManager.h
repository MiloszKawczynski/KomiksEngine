#pragma once

#include "Component.h"
#include "Cow.h"
#include "Game/Path.h"
#include "Sound.h"

class CowManager final : public Component
{

public:
    static std::shared_ptr<CowManager> create();

    explicit CowManager(AK::Badge<CowManager>);

    virtual void awake() override;
    virtual void update() override;
#if EDITOR
    virtual void draw_editor() override;
#endif

    void get_spawn_paths();
    glm::vec2 get_random_position_with_minimal_distance(glm::vec3 current_position) const;

    std::vector<std::weak_ptr<Path>> paths = {};
    std::vector<std::weak_ptr<Cow>> cows = {};

private:
    void spawn_cow();
};
