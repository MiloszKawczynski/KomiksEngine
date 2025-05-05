#pragma once

#include <glm/glm.hpp>

#include "AK/Badge.h"
#include "AK/Types.h"
#include "Component.h"

enum class LinkToTypes
{
    Position,
    Rotation,
    Scale
};

enum class LinkToArgumentTypes
{
    X,
    Y,
    Z
};

class Curve : public Component
{
public:
    static std::shared_ptr<Curve> create();

    explicit Curve(AK::Badge<Curve>);

    virtual void awake() override;
    virtual void update() override;

#if EDITOR
    virtual void draw_editor() override;
    std::string link_type_to_string(LinkToTypes const type);
    std::string link_type_to_argument_string(LinkToArgumentTypes const type);
#endif

    std::vector<glm::vec2> points = {};
    bool is_smoothe = false;
    float playback_speed = 0.01f;
    float easing_from = 0.0f;
    float easing_to = 1.0f;

    glm::vec2 get_point_at(float x) const;
    float get_y_at(float x) const;
    void add_points(std::initializer_list<glm::vec2> new_points);
    int get_point_index_before(float x) const;
    int get_point_index_after(float x) const;
    float length() const;
    glm::vec2 catmull_rom(glm::vec2 const& p0, glm::vec2 const& p1, glm::vec2 const& p2, glm::vec2 const& p3, float t);
    void generate_smoothe_points(u32 segments_per_curve);
    void update_link_value();

protected:
    explicit Curve();

private:
    i32 m_smoothe_precision = 6;
    std::vector<glm::vec2> m_smoothe_points = {};
    bool m_is_playing = false;
    float m_playback_position = 0.0f;
    LinkToTypes m_link_to = LinkToTypes::Position;
    LinkToArgumentTypes m_link_to_argument = LinkToArgumentTypes::X;
};
