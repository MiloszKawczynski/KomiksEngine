#pragma once

#include <glm/glm.hpp>

#include "AK/Badge.h"
#include "Component.h"

class Curve : public Component
{
public:
    static std::shared_ptr<Curve> create();

    explicit Curve(AK::Badge<Curve>);

#if EDITOR
    virtual void draw_editor() override;
#endif

    std::vector<glm::vec2> points = {};
    glm::vec2 get_point_at(float x) const;
    float get_y_at(float x) const;
    void add_points(std::initializer_list<glm::vec2> new_points);
    int get_point_index_before(float x) const;
    int get_point_index_after(float x) const;
    float length() const;

protected:
    explicit Curve();
};
