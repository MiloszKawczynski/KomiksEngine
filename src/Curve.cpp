#include "Curve.h"

#include "Entity.h"
#include "Game/LevelController.h"

#include <glm/gtc/type_ptr.inl>

#include <iostream>

#if EDITOR
#include "imgui_extensions.h"
#include <imgui.h>
#include <implot.h>
#endif

std::shared_ptr<Curve> Curve::create()
{
    return std::make_shared<Curve>(AK::Badge<Curve> {});
}

Curve::Curve(AK::Badge<Curve>)
{
    points.push_back({0.0f, 0.0f});
    points.push_back({1.0f, 1.0f});
}

Curve::Curve() = default;

void Curve::awake()
{
    set_can_tick(true);
}

void Curve::update()
{
    if (m_is_playing)
    {
        m_playback_position += playback_speed;

        if (m_playback_position > 1.0f)
        {
            m_playback_position = 1.0f;
            m_is_playing = false;
        }

        update_link_value();
    }
}

#if EDITOR
void Curve::draw_editor()
{
    Component::draw_editor();

    if (ImPlot::BeginPlot("Path visualised", nullptr, nullptr, ImVec2(-1, 0), ImPlotFlags_CanvasOnly))
    {
        ImPlot::SetupAxisLimits(ImAxis_X1, 0.0f, 1.0f, ImGuiCond_Always);
        ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 2.0f);
        ImPlot::SetupLegend(ImPlotFlags_NoLegend);

        if (ImGui::IsMouseClicked(2))
        {
            ImPlot::SetupAxisLimits(ImAxis_Y1, 0.0, 1.0, ImGuiCond_Always);
        }

        std::vector<float> xs, ys;
        for (auto const& p : points)
        {
            xs.push_back(p.x);
            ys.push_back(p.y);
        }

        if (is_smoothe)
        {
            std::vector<float> xss, yss;
            for (auto const& p : m_smoothe_points)
            {
                xss.push_back(p.x);
                yss.push_back(p.y);
            }

            ImPlot::PlotLine("##Smoothe Line", xss.data(), yss.data(), m_smoothe_points.size());
        }
        else
        {
            ImPlot::PlotLine("##Line", xs.data(), ys.data(), points.size());
        }

        for (u32 i = 0; i < points.size(); i++)
        {
            double px = xs[i];
            double py = ys[i];
            bool is_clicked = false;
            if (ImPlot::DragPoint(i, &px, &py, ImVec4(0, 0.9f, 0, 1), 4, 0, &is_clicked))
            {
                points[i].x = px;

                float left_clamp = 0.0f;
                float right_clamp = 1.0f;

                if (i > 0)
                {
                    left_clamp = points[i - 1].x;
                }

                if (i < points.size() - 1)
                {
                    right_clamp = points[i + 1].x;
                }

                points[i].x = glm::clamp(points[i].x, left_clamp, right_clamp);

                points[i].y = py;

                points[i].x = glm::clamp(points[i].x, 0.0f, 1.0f);

                points[0].x = 0.0f;
                points[points.size() - 1].x = 1.0f;
            }

            if ((i != 0 && i != points.size() - 1) && is_clicked && ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
            {
                points.erase(points.begin() + i);
                generate_smoothe_points(m_smoothe_precision);
            }
        }

        if (ImPlot::IsPlotHovered() && ImGui::IsMouseClicked(0) && ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
        {
            ImPlotPoint mouse_position = ImPlot::GetPlotMousePos();
            glm::vec2 new_point_position = {glm::clamp(mouse_position.x, 0.0, 1.0), mouse_position.y};

            u32 point_after = get_point_index_after(new_point_position.x);

            points.insert(points.begin() + point_after, new_point_position);
        }

        std::vector<float> ppxs, ppys;
        ppxs.push_back(m_playback_position);
        ppys.push_back(get_y_at(m_playback_position));
        ImPlot::PlotScatter("##Playback Point", ppxs.data(), ppys.data(), 1);
    }

    ImPlot::EndPlot();

    ImGui::Checkbox("Smoothe", &is_smoothe);
    ImGui::SameLine();

    ImGui::InputInt("Smoothe precistion", &m_smoothe_precision);

    if (ImGui::Button("Play"))
    {
        m_playback_position = 0.0f;
        m_is_playing = true;
    }

    ImGui::SameLine();

    if (ImGui::Button("Reset"))
    {
        m_playback_position = 0.0f;
        update_link_value();
    }

    ImGuiEx::InputFloat("Playback speed", &playback_speed);

    ImGui::Text("Link to...");
    ImGui::SameLine();

    if (ImGui::BeginCombo("##LinkToTypes", link_type_to_string(m_link_to).c_str()))
    {
        for (u32 i = static_cast<u32>(LinkToTypes::Position); i <= static_cast<u32>(LinkToTypes::Scale); i++)
        {
            bool const is_selected = m_link_to == static_cast<LinkToTypes>(i);

            if (ImGui::Selectable(link_type_to_string(static_cast<LinkToTypes>(i)).c_str(), is_selected))
            {
                m_link_to = static_cast<LinkToTypes>(i);
            }

            if (is_selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::SameLine();

    if (ImGui::BeginCombo("##LinkToArgumentTypes", link_type_to_argument_string(m_link_to_argument).c_str()))
    {
        for (u32 i = static_cast<u32>(LinkToArgumentTypes::X); i <= static_cast<u32>(LinkToArgumentTypes::Z); i++)
        {
            bool const is_selected = m_link_to_argument == static_cast<LinkToArgumentTypes>(i);

            if (ImGui::Selectable(link_type_to_argument_string(static_cast<LinkToArgumentTypes>(i)).c_str(), is_selected))
            {
                m_link_to_argument = static_cast<LinkToArgumentTypes>(i);
            }

            if (is_selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::InputFloat("Ease from", &easing_from);
    ImGui::SameLine();
    ImGui::InputFloat("Ease to", &easing_to);

    if (ImGui::CollapsingHeader("Points"))
    {
        if (ImGui::Button("Add point"))
        {
            if (points.size() == 0)
            {
                points.push_back({0.1f, 0.5f});
            }
            else
            {
                points.push_back({points.back().x + 0.1f, points.back().y});

                points.back().x = glm::clamp(points.back().x, 0.0f, 1.0f);
            }
        }

        for (u32 i = 0; i < points.size(); i++)
        {
            ImGuiEx::InputFloat2(("Position##" + std::to_string(i)).c_str(), glm::value_ptr(points[i]));

            ImGui::SameLine();

            ImGui::BeginDisabled(i == 0 || i == points.size() - 1);
            if (ImGui::Button(("Remove point##" + std::to_string(i)).c_str()))
            {
                points.erase(points.begin() + i);
            }
            ImGui::EndDisabled();
        }
    }

    if (is_smoothe)
    {
        generate_smoothe_points(m_smoothe_precision);
    }
}

std::string Curve::link_type_to_string(LinkToTypes const type)
{
    switch (type)
    {
    case LinkToTypes::Position:
        return "Position";

    case LinkToTypes::Rotation:
        return "Rotation";

    case LinkToTypes::Scale:
        return "Scale";

    default:
        return "Undefined Link";
    }
}

std::string Curve::link_type_to_argument_string(LinkToArgumentTypes const type)
{
    switch (type)
    {
    case LinkToArgumentTypes::X:
        return "x";

    case LinkToArgumentTypes::Y:
        return "y";

    case LinkToArgumentTypes::Z:
        return "z";

    default:
        return "Undefined Argument Link";
    }
}

#endif

float Curve::length() const
{
    float distance = 0.0f;
    for (u32 i = 0; i < points.size() - 1; i++)
    {
        distance += glm::distance(points[i], points[i + 1]);
    }

    return distance;
}

glm::vec2 Curve::get_point_at(float x) const
{
    std::vector<glm::vec2> operational_set = points;

    if (is_smoothe)
    {
        operational_set = m_smoothe_points;
    }

    if (operational_set.empty())
    {
        return {0.0f, 0.0f};
    }

    if (operational_set.size() == 1)
    {
        return operational_set[0];
    }

    x = glm::clamp(x, 0.0f, 1.0f);

    float const path_length = length();
    float const desired_length = path_length * x;
    float distance = 0.0f;

    for (u32 i = 0; i < operational_set.size() - 1; i++)
    {
        float const segment_length = glm::distance(operational_set[i], operational_set[i + 1]);

        if (distance + segment_length >= desired_length)
        {
            float const segment_ratio = (desired_length - distance) / segment_length;
            return glm::mix(operational_set[i], operational_set[i + 1], segment_ratio);
        }

        distance += segment_length;
    }

    return operational_set.back();
}

float Curve::get_y_at(float x) const
{
    if (points.empty())
    {
        return 0.0f;
    }

    x = glm::clamp(x, 0.0f, 1.0f);

    std::vector<glm::vec2> operational_set = points;

    if (is_smoothe)
    {
        operational_set = m_smoothe_points;
    }

    for (u32 i = 0; i < operational_set.size() - 1; i++)
    {
        if (x >= operational_set[i].x && x <= operational_set[i + 1].x)
        {
            float const x0 = operational_set[i].x;
            float const y0 = operational_set[i].y;
            float const x1 = operational_set[i + 1].x;
            float const y1 = operational_set[i + 1].y;

            float const t = (x - x0) / (x1 - x0);
            float const y = y0 + t * (y1 - y0);
            return y;
        }
    }

    return 0.0f;
}

void Curve::add_points(std::initializer_list<glm::vec2> new_points)
{
    points.insert(points.end(), new_points.begin(), new_points.end());
}

int Curve::get_point_index_before(float x) const
{
    int index = 0;

    for (u32 i = 0; i < points.size() - 1; i++)
    {
        if (points[i].x < x)
        {
            index = i;
            continue;
        }

        break;
    }

    return index;
}

int Curve::get_point_index_after(float x) const
{
    int index = 0;

    for (u32 i = 0; i < points.size(); i++)
    {
        if (points[i].x < x)
        {
            continue;
        }

        index = i;
        break;
    }

    return index;
}

glm::vec2 Curve::catmull_rom(glm::vec2 const& p0, glm::vec2 const& p1, glm::vec2 const& p2, glm::vec2 const& p3, float t)
{
    float t2 = t * t;
    float t3 = t2 * t;

    return 0.5f * (2.0f * p1 + (p2 - p0) * t + (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2 + (-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3);
}

void Curve::generate_smoothe_points(u32 segments_per_curve)
{
    m_smoothe_points.clear();

    if (points.size() < 2)
    {
        return;
    }

    std::vector<glm::vec2> points_with_doubled_edges;
    points_with_doubled_edges.push_back(points.front());
    points_with_doubled_edges.insert(points_with_doubled_edges.end(), points.begin(), points.end());
    points_with_doubled_edges.push_back(points.back());

    for (u32 i = 0; i < points_with_doubled_edges.size() - 3; i++)
    {
        glm::vec2 p0 = points_with_doubled_edges[i];
        glm::vec2 p1 = points_with_doubled_edges[i + 1];
        glm::vec2 p2 = points_with_doubled_edges[i + 2];
        glm::vec2 p3 = points_with_doubled_edges[i + 3];

        for (int j = 0; j < segments_per_curve; j++)
        {
            float t = float(j) / segments_per_curve;
            m_smoothe_points.push_back(catmull_rom(p0, p1, p2, p3, t));
        }
    }

    m_smoothe_points.push_back(points.back());
}

void Curve::update_link_value()
{
    glm::vec3 position = entity->transform->get_local_position();
    glm::vec3 rotation = entity->transform->get_euler_angles();
    glm::vec3 scale = entity->transform->get_local_scale();

    switch (m_link_to)
    {
    case LinkToTypes::Position:
        switch (m_link_to_argument)
        {
        case LinkToArgumentTypes::X:
            position.x = glm::mix(easing_from, easing_to, get_y_at(m_playback_position));
            break;

        case LinkToArgumentTypes::Y:
            position.y = glm::mix(easing_from, easing_to, get_y_at(m_playback_position));
            break;

        case LinkToArgumentTypes::Z:
            position.z = glm::mix(easing_from, easing_to, get_y_at(m_playback_position));
            break;

        default:
            break;
        }
        break;

    case LinkToTypes::Rotation:
        switch (m_link_to_argument)
        {
        case LinkToArgumentTypes::X:
            rotation.x = glm::mix(easing_from, easing_to, get_y_at(m_playback_position));
            break;

        case LinkToArgumentTypes::Y:
            rotation.y = glm::mix(easing_from, easing_to, get_y_at(m_playback_position));
            break;

        case LinkToArgumentTypes::Z:
            rotation.z = glm::mix(easing_from, easing_to, get_y_at(m_playback_position));
            break;

        default:
            break;
        }
        break;

    case LinkToTypes::Scale:
        switch (m_link_to_argument)
        {
        case LinkToArgumentTypes::X:
            scale.x = glm::mix(easing_from, easing_to, get_y_at(m_playback_position));
            break;

        case LinkToArgumentTypes::Y:
            scale.y = glm::mix(easing_from, easing_to, get_y_at(m_playback_position));
            break;

        case LinkToArgumentTypes::Z:
            scale.z = glm::mix(easing_from, easing_to, get_y_at(m_playback_position));
            break;

        default:
            break;
        }
        break;

    default:
        break;
    }

    entity->transform->set_local_position(position);
    entity->transform->set_euler_angles(rotation);
    entity->transform->set_local_scale(scale);
}
