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
        ImPlot::SetupAxesLimits(0.0f, 1.0f, 0.0f, 1.0f, ImGuiCond_Always);
        ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 2.0f);
        ImPlot::SetupLegend(ImPlotFlags_NoLegend);

        std::vector<float> xs, ys;
        for (auto const& p : points)
        {
            xs.push_back(p.x);
            ys.push_back(p.y);
        }

        ImPlot::PlotLine("##Line", xs.data(), ys.data(), points.size());

        for (u32 i = 0; i < points.size(); i++)
        {
            double px = xs[i];
            double py = ys[i];
            if (ImPlot::DragPoint(i, &px, &py, ImVec4(0, 0.9f, 0, 1), 4))
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

                points[i] = glm::clamp(points[i], 0.0f, 1.0f);
            }
        }

        if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsMouseClicked(0))
        {
            ImPlotPoint mouse_position = ImPlot::GetPlotMousePos();
            bool is_point_being_removed = false;

            for (u32 i = 1; i < points.size() - 1; i++)
            {
                ImPlotPoint point = ImPlotPoint(points[i].x, points[i].y);

                if (glm::epsilonEqual(mouse_position.x, point.x, 0.01) && glm::epsilonEqual(mouse_position.y, point.y, 0.01))
                {
                    is_point_being_removed = true;
                    points.erase(points.begin() + i);
                }
            }

            if (!is_point_being_removed)
            {
                glm::vec2 new_point_position = {glm::clamp(mouse_position.x, 0.0, 1.0), mouse_position.y};

                u32 point_after = get_point_index_after(new_point_position.x);

                points.insert(points.begin() + point_after, new_point_position);
            }
        }
    }

    ImPlot::EndPlot();

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
        if (ImGui::Button(("Remove point##" + std::to_string(i)).c_str()))
        {
            points.erase(points.begin() + i);
        }
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
    if (points.empty())
    {
        return {0.0f, 0.0f};
    }

    if (points.size() == 1)
    {
        return points[0];
    }

    x = glm::clamp(x, 0.0f, 1.0f);

    float const path_length = length();
    float const desired_length = path_length * x;
    float distance = 0.0f;

    for (u32 i = 0; i < points.size() - 1; i++)
    {
        float const segment_length = glm::distance(points[i], points[i + 1]);

        if (distance + segment_length >= desired_length)
        {
            float const segment_ratio = (desired_length - distance) / segment_length;
            return glm::mix(points[i], points[i + 1], segment_ratio);
        }

        distance += segment_length;
    }

    return points.back();
}

float Curve::get_y_at(float x) const
{
    if (points.empty())
    {
        return 0.0f;
    }

    x = glm::clamp(x, 0.0f, 1.0f);

    for (u32 i = 0; i < points.size() - 1; i++)
    {
        if (x >= points[i].x && x <= points[i + 1].x)
        {
            float const x0 = points[i].x;
            float const y0 = points[i].y;
            float const x1 = points[i + 1].x;
            float const y1 = points[i + 1].y;

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
