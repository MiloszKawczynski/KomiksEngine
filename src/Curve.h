#pragma once

#include <glm/glm.hpp>

#include "AK/Badge.h"
#include "AK/Types.h"
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
    bool is_smoothe = false;

    glm::vec2 get_point_at(float x) const;
    float get_y_at(float x) const;
    void add_points(std::initializer_list<glm::vec2> new_points);
    int get_point_index_before(float x) const;
    int get_point_index_after(float x) const;
    float length() const;
    glm::vec2 catmull_rom(glm::vec2 const& p0, glm::vec2 const& p1, glm::vec2 const& p2, glm::vec2 const& p3, float t);
    void generate_smoothe_points(u32 segments_per_curve);

protected:
    explicit Curve();

private:
    i32 m_smoothe_precision = 6;
    std::vector<glm::vec2> m_smoothe_points = {};
};
