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

        if (m_is_smoothe)
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
            generate_smoothe_points(m_smoothe_precision);
        }

        if (m_is_smoothe)
        {
            generate_smoothe_points(m_smoothe_precision);
        }
    }

    ImPlot::EndPlot();

    ImGui::Checkbox("Smoothe", &m_is_smoothe);

    if (ImGui::InputInt("Smoothe precistion", &m_smoothe_precision))
    {
        generate_smoothe_points(m_smoothe_precision);
    }

    if (ImGui::Button("Add point"))
    {
        if (points.size() == 0)
        {
            points.push_back({0.1f, 0.5f});
            generate_smoothe_points(m_smoothe_precision);
        }
        else
        {
            points.push_back({points.back().x + 0.1f, points.back().y});

            points.back().x = glm::clamp(points.back().x, 0.0f, 1.0f);
            generate_smoothe_points(m_smoothe_precision);
        }
    }

    for (u32 i = 0; i < points.size(); i++)
    {
        if (ImGuiEx::InputFloat2(("Position##" + std::to_string(i)).c_str(), glm::value_ptr(points[i])))
        {
            generate_smoothe_points(m_smoothe_precision);
        }

        ImGui::SameLine();

        ImGui::BeginDisabled(i == 0 || i == points.size() - 1);
        if (ImGui::Button(("Remove point##" + std::to_string(i)).c_str()))
        {
            points.erase(points.begin() + i);
            generate_smoothe_points(m_smoothe_precision);
        }
        ImGui::EndDisabled();
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

    if (m_is_smoothe)
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

    if (m_is_smoothe)
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
