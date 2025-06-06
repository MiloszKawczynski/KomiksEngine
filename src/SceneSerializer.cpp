#include "SceneSerializer.h"

#include "AssetPreloader.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <unordered_set>

#include <yaml-cpp/yaml.h>

#include "AK/ScopeGuard.h"
#include "Button.h"
#include "Camera.h"
#include "Collider2D.h"
#include "Cube.h"
#include "Curve.h"
#include "DebugDrawing.h"
#include "DebugInputController.h"
#include "DialoguePromptController.h"
#include "DirectionalLight.h"
#include "Drawable.h"
#include "Ellipse.h"
#include "Entity.h"
#include "ExampleDynamicText.h"
#include "ExampleUIBar.h"
#include "Floater.h"
#include "FloatersManager.h"
#include "FloeButton.h"
#include "Game/Clock.h"
#include "Game/Credits.h"
#include "Game/Customer.h"
#include "Game/CustomerManager.h"
#include "Game/EndScreen.h"
#include "Game/EndScreenFoliage.h"
#include "Game/Factory.h"
#include "Game/FieldCell.h"
#include "Game/FieldGrid.h"
#include "Game/GameController.h"
#include "Game/HovercraftWithoutKeeper.h"
#include "Game/IceBound.h"
#include "Game/Jeep.h"
#include "Game/LevelController.h"
#include "Game/Lighthouse.h"
#include "Game/LighthouseKeeper.h"
#include "Game/LighthouseLight.h"
#include "Game/Path.h"
#include "Game/Player.h"
#include "Game/Player/PlayerInput.h"
#include "Game/Popup.h"
#include "Game/Port.h"
#include "Game/Ship.h"
#include "Game/ShipEyes.h"
#include "Game/ShipSpawner.h"
#include "Game/Thanks.h"
#include "Game/Truther.h"
#include "Game/WheatOverlay.h"
#include "Komiks/Cow.h"
#include "Komiks/CowManager.h"
#include "Komiks/JeepReflector.h"
#include "Komiks/UFO.h"
#include "Komiks/Wheat.h"
#include "Light.h"
#include "Model.h"
#include "NowPromptTrigger.h"
#include "Panel.h"
#include "Particle.h"
#include "ParticleSystem.h"
#include "PointLight.h"
#include "Quad.h"
#include "ScreenText.h"
#include "ShaderFactory.h"
#include "Sound.h"
#include "SoundListener.h"
#include "Sphere.h"
#include "SpotLight.h"
#include "Sprite.h"
#include "Water.h"
#include "yaml-cpp-extensions.h"
// # Put new header here

SceneSerializer::SceneSerializer(std::shared_ptr<Scene> const& scene) : m_scene(scene)
{
}

std::shared_ptr<SceneSerializer> SceneSerializer::get_instance()
{
    return m_instance;
}

void SceneSerializer::set_instance(std::shared_ptr<SceneSerializer> const& instance)
{
    m_instance = instance;
}

std::shared_ptr<Component> SceneSerializer::get_from_pool(std::string const& guid) const
{
    for (auto const& obj : deserialized_pool)
    {
        if (obj->guid == guid)
            return obj;
    }

    if (m_deserialization_mode == DeserializationMode::Normal)
        return nullptr;

    for (auto const& entity : MainScene::get_instance()->entities)
    {
        for (auto const& component : entity->components)
        {
            if (component->guid == guid)
                return component;
        }
    }

    return nullptr;
}

std::shared_ptr<Entity> SceneSerializer::get_entity_from_pool(std::string const& guid) const
{
    for (auto const& obj : deserialized_entities_pool)
    {
        if (obj->guid == guid)
            return obj;
    }

    if (m_deserialization_mode == DeserializationMode::Normal)
        return nullptr;

    for (auto const& entity : MainScene::get_instance()->entities)
    {
        if (entity->guid == guid)
            return entity;
    }

    return nullptr;
}

void SceneSerializer::auto_serialize_component(YAML::Emitter& out, std::shared_ptr<Component> const& component)
{
    // # Auto serialization start
    if (auto const camera = std::dynamic_pointer_cast<class Camera>(component); camera != nullptr)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ComponentName" << YAML::Value << "CameraComponent";
        out << YAML::Key << "guid" << YAML::Value << camera->guid;
        out << YAML::Key << "custom_name" << YAML::Value << camera->custom_name;
        out << YAML::Key << "width" << YAML::Value << camera->width;
        out << YAML::Key << "height" << YAML::Value << camera->height;
        out << YAML::Key << "fov" << YAML::Value << camera->fov;
        out << YAML::Key << "near_plane" << YAML::Value << camera->near_plane;
        out << YAML::Key << "far_plane" << YAML::Value << camera->far_plane;
        out << YAML::EndMap;
    }
    else
    if (auto const collider2d = std::dynamic_pointer_cast<class Collider2D>(component); collider2d != nullptr)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ComponentName" << YAML::Value << "Collider2DComponent";
        out << YAML::Key << "guid" << YAML::Value << collider2d->guid;
        out << YAML::Key << "custom_name" << YAML::Value << collider2d->custom_name;
        out << YAML::Key << "offset" << YAML::Value << collider2d->offset;
        out << YAML::Key << "is_trigger" << YAML::Value << collider2d->is_trigger;
        out << YAML::Key << "is_static" << YAML::Value << collider2d->is_static;
        out << YAML::Key << "collider_type" << YAML::Value << collider2d->collider_type;
        out << YAML::Key << "width" << YAML::Value << collider2d->width;
        out << YAML::Key << "height" << YAML::Value << collider2d->height;
        out << YAML::Key << "radius" << YAML::Value << collider2d->radius;
        out << YAML::Key << "drag" << YAML::Value << collider2d->drag;
        out << YAML::Key << "velocity" << YAML::Value << collider2d->velocity;
        out << YAML::EndMap;
    }
    else
    if (auto const curve = std::dynamic_pointer_cast<class Curve>(component); curve != nullptr)
    {
        out << YAML::BeginMap;
        // # Put new Curve kid here
        if (auto const path = std::dynamic_pointer_cast<class Path>(component); path != nullptr)
        {
            out << YAML::Key << "ComponentName" << YAML::Value << "PathComponent";
            out << YAML::Key << "guid" << YAML::Value << path->guid;
            out << YAML::Key << "custom_name" << YAML::Value << path->custom_name;
        }
        else
        {
            out << YAML::Key << "ComponentName" << YAML::Value << "CurveComponent";
            out << YAML::Key << "guid" << YAML::Value << curve->guid;
            out << YAML::Key << "custom_name" << YAML::Value << curve->custom_name;
        }
        out << YAML::Key << "points" << YAML::Value << curve->points;
        out << YAML::EndMap;
    }
    else
    if (auto const debuginputcontroller = std::dynamic_pointer_cast<class DebugInputController>(component); debuginputcontroller != nullptr)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ComponentName" << YAML::Value << "DebugInputControllerComponent";
        out << YAML::Key << "guid" << YAML::Value << debuginputcontroller->guid;
        out << YAML::Key << "custom_name" << YAML::Value << debuginputcontroller->custom_name;
        out << YAML::Key << "gamma" << YAML::Value << debuginputcontroller->gamma;
        out << YAML::Key << "exposure" << YAML::Value << debuginputcontroller->exposure;
        out << YAML::EndMap;
    }
    else
    if (auto const dialoguepromptcontroller = std::dynamic_pointer_cast<class DialoguePromptController>(component); dialoguepromptcontroller != nullptr)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ComponentName" << YAML::Value << "DialoguePromptControllerComponent";
        out << YAML::Key << "guid" << YAML::Value << dialoguepromptcontroller->guid;
        out << YAML::Key << "custom_name" << YAML::Value << dialoguepromptcontroller->custom_name;
        out << YAML::Key << "interp_speed" << YAML::Value << dialoguepromptcontroller->interp_speed;
        out << YAML::Key << "dialogue_panel" << YAML::Value << dialoguepromptcontroller->dialogue_panel;
        out << YAML::Key << "panel_parent" << YAML::Value << dialoguepromptcontroller->panel_parent;
        out << YAML::Key << "keeper_sprite" << YAML::Value << dialoguepromptcontroller->keeper_sprite;
        out << YAML::Key << "upper_text" << YAML::Value << dialoguepromptcontroller->upper_text;
        out << YAML::Key << "middle_text" << YAML::Value << dialoguepromptcontroller->middle_text;
        out << YAML::Key << "lower_text" << YAML::Value << dialoguepromptcontroller->lower_text;
        out << YAML::Key << "dialogue_objects" << YAML::Value << dialoguepromptcontroller->dialogue_objects;
        out << YAML::EndMap;
    }
    else
    if (auto const drawable = std::dynamic_pointer_cast<class Drawable>(component); drawable != nullptr)
    {
        out << YAML::BeginMap;
        // # Put new Drawable kid here
        if (auto const screentext = std::dynamic_pointer_cast<class ScreenText>(component); screentext != nullptr)
        {
            out << YAML::Key << "ComponentName" << YAML::Value << "ScreenTextComponent";
            out << YAML::Key << "guid" << YAML::Value << screentext->guid;
            out << YAML::Key << "custom_name" << YAML::Value << screentext->custom_name;
            out << YAML::Key << "text" << YAML::Value << screentext->text;
            out << YAML::Key << "position" << YAML::Value << screentext->position;
            out << YAML::Key << "font_size" << YAML::Value << screentext->font_size;
            out << YAML::Key << "color" << YAML::Value << screentext->color;
            out << YAML::Key << "flags" << YAML::Value << screentext->flags;
            out << YAML::Key << "font_name" << YAML::Value << screentext->font_name;
            out << YAML::Key << "bold" << YAML::Value << screentext->bold;
            out << YAML::Key << "button_ref" << YAML::Value << screentext->button_ref;
        }
        else
        if (auto const quad = std::dynamic_pointer_cast<class Quad>(component); quad != nullptr)
        {
            out << YAML::Key << "ComponentName" << YAML::Value << "QuadComponent";
            out << YAML::Key << "guid" << YAML::Value << quad->guid;
            out << YAML::Key << "custom_name" << YAML::Value << quad->custom_name;
            out << YAML::Key << "path" << YAML::Value << quad->path;
        }
        else
        if (auto const panel = std::dynamic_pointer_cast<class Panel>(component); panel != nullptr)
        {
            out << YAML::Key << "ComponentName" << YAML::Value << "PanelComponent";
            out << YAML::Key << "guid" << YAML::Value << panel->guid;
            out << YAML::Key << "custom_name" << YAML::Value << panel->custom_name;
            out << YAML::Key << "background_path" << YAML::Value << panel->background_path;
        }
        else
        if (auto const model = std::dynamic_pointer_cast<class Model>(component); model != nullptr)
        {
            // # Put new Model kid here
            if (auto const water = std::dynamic_pointer_cast<class Water>(component); water != nullptr)
            {
                out << YAML::Key << "ComponentName" << YAML::Value << "WaterComponent";
                out << YAML::Key << "guid" << YAML::Value << water->guid;
                out << YAML::Key << "custom_name" << YAML::Value << water->custom_name;
                out << YAML::Key << "waves" << YAML::Value << water->waves;
                out << YAML::Key << "m_ps_buffer" << YAML::Value << water->m_ps_buffer;
                out << YAML::Key << "tesselation_level" << YAML::Value << water->tesselation_level;
            }
            else
            if (auto const sprite = std::dynamic_pointer_cast<class Sprite>(component); sprite != nullptr)
            {
                out << YAML::Key << "ComponentName" << YAML::Value << "SpriteComponent";
                out << YAML::Key << "guid" << YAML::Value << sprite->guid;
                out << YAML::Key << "custom_name" << YAML::Value << sprite->custom_name;
                out << YAML::Key << "diffuse_texture_path" << YAML::Value << sprite->diffuse_texture_path;
            }
            else
            if (auto const sphere = std::dynamic_pointer_cast<class Sphere>(component); sphere != nullptr)
            {
                out << YAML::Key << "ComponentName" << YAML::Value << "SphereComponent";
                out << YAML::Key << "guid" << YAML::Value << sphere->guid;
                out << YAML::Key << "custom_name" << YAML::Value << sphere->custom_name;
                out << YAML::Key << "sector_count" << YAML::Value << sphere->sector_count;
                out << YAML::Key << "stack_count" << YAML::Value << sphere->stack_count;
                out << YAML::Key << "texture_path" << YAML::Value << sphere->texture_path;
                out << YAML::Key << "radius" << YAML::Value << sphere->radius;
            }
            else
            if (auto const cube = std::dynamic_pointer_cast<class Cube>(component); cube != nullptr)
            {
                out << YAML::Key << "ComponentName" << YAML::Value << "CubeComponent";
                out << YAML::Key << "guid" << YAML::Value << cube->guid;
                out << YAML::Key << "custom_name" << YAML::Value << cube->custom_name;
                out << YAML::Key << "diffuse_texture_path" << YAML::Value << cube->diffuse_texture_path;
                out << YAML::Key << "specular_texture_path" << YAML::Value << cube->specular_texture_path;
            }
            else
            {
                out << YAML::Key << "ComponentName" << YAML::Value << "ModelComponent";
                out << YAML::Key << "guid" << YAML::Value << model->guid;
                out << YAML::Key << "custom_name" << YAML::Value << model->custom_name;
            }
            out << YAML::Key << "model_path" << YAML::Value << model->model_path;
        }
        else
        if (auto const button = std::dynamic_pointer_cast<class Button>(component); button != nullptr)
        {
            out << YAML::Key << "ComponentName" << YAML::Value << "ButtonComponent";
            out << YAML::Key << "guid" << YAML::Value << button->guid;
            out << YAML::Key << "custom_name" << YAML::Value << button->custom_name;
            out << YAML::Key << "path_default" << YAML::Value << button->path_default;
            out << YAML::Key << "path_hovered" << YAML::Value << button->path_hovered;
            out << YAML::Key << "path_pressed" << YAML::Value << button->path_pressed;
            out << YAML::Key << "top_left_corner" << YAML::Value << button->top_left_corner;
            out << YAML::Key << "top_right_corner" << YAML::Value << button->top_right_corner;
            out << YAML::Key << "bottom_left_corner" << YAML::Value << button->bottom_left_corner;
            out << YAML::Key << "bottom_right_corner" << YAML::Value << button->bottom_right_corner;
        }
        else
        {
            out << YAML::Key << "ComponentName" << YAML::Value << "DrawableComponent";
            out << YAML::Key << "guid" << YAML::Value << drawable->guid;
            out << YAML::Key << "custom_name" << YAML::Value << drawable->custom_name;
        }
        out << YAML::Key << "material" << YAML::Value << drawable->material;
        out << YAML::EndMap;
    }
    else
    if (auto const exampledynamictext = std::dynamic_pointer_cast<class ExampleDynamicText>(component); exampledynamictext != nullptr)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ComponentName" << YAML::Value << "ExampleDynamicTextComponent";
        out << YAML::Key << "guid" << YAML::Value << exampledynamictext->guid;
        out << YAML::Key << "custom_name" << YAML::Value << exampledynamictext->custom_name;
        out << YAML::EndMap;
    }
    else
    if (auto const exampleuibar = std::dynamic_pointer_cast<class ExampleUIBar>(component); exampleuibar != nullptr)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ComponentName" << YAML::Value << "ExampleUIBarComponent";
        out << YAML::Key << "guid" << YAML::Value << exampleuibar->guid;
        out << YAML::Key << "custom_name" << YAML::Value << exampleuibar->custom_name;
        out << YAML::Key << "value" << YAML::Value << exampleuibar->value;
        out << YAML::EndMap;
    }
    else
    if (auto const floater = std::dynamic_pointer_cast<class Floater>(component); floater != nullptr)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ComponentName" << YAML::Value << "FloaterComponent";
        out << YAML::Key << "guid" << YAML::Value << floater->guid;
        out << YAML::Key << "custom_name" << YAML::Value << floater->custom_name;
        out << YAML::Key << "sink" << YAML::Value << floater->sink;
        out << YAML::Key << "side_floaters_offset" << YAML::Value << floater->side_floaters_offset;
        out << YAML::Key << "side_roation_strength" << YAML::Value << floater->side_roation_strength;
        out << YAML::Key << "forward_rotation_strength" << YAML::Value << floater->forward_rotation_strength;
        out << YAML::Key << "forward_floaters_offest" << YAML::Value << floater->forward_floaters_offest;
        out << YAML::Key << "water" << YAML::Value << floater->water;
        out << YAML::EndMap;
    }
    else
    if (auto const floatersmanager = std::dynamic_pointer_cast<class FloatersManager>(component); floatersmanager != nullptr)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ComponentName" << YAML::Value << "FloatersManagerComponent";
        out << YAML::Key << "guid" << YAML::Value << floatersmanager->guid;
        out << YAML::Key << "custom_name" << YAML::Value << floatersmanager->custom_name;
        out << YAML::Key << "big_boat_settings" << YAML::Value << floatersmanager->big_boat_settings;
        out << YAML::Key << "small_boat_settings" << YAML::Value << floatersmanager->small_boat_settings;
        out << YAML::Key << "medium_boat_settings" << YAML::Value << floatersmanager->medium_boat_settings;
        out << YAML::Key << "tool_boat_settings" << YAML::Value << floatersmanager->tool_boat_settings;
        out << YAML::Key << "pirate_boat_settings" << YAML::Value << floatersmanager->pirate_boat_settings;
        out << YAML::Key << "water" << YAML::Value << floatersmanager->water;
        out << YAML::EndMap;
    }
    else
    if (auto const floebutton = std::dynamic_pointer_cast<class FloeButton>(component); floebutton != nullptr)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ComponentName" << YAML::Value << "FloeButtonComponent";
        out << YAML::Key << "guid" << YAML::Value << floebutton->guid;
        out << YAML::Key << "custom_name" << YAML::Value << floebutton->custom_name;
        out << YAML::Key << "floe_button_type" << YAML::Value << floebutton->floe_button_type;
        out << YAML::EndMap;
    }
    else
    if (auto const light = std::dynamic_pointer_cast<class Light>(component); light != nullptr)
    {
        out << YAML::BeginMap;
        // # Put new Light kid here
        if (auto const spotlight = std::dynamic_pointer_cast<class SpotLight>(component); spotlight != nullptr)
        {
            out << YAML::Key << "ComponentName" << YAML::Value << "SpotLightComponent";
            out << YAML::Key << "guid" << YAML::Value << spotlight->guid;
            out << YAML::Key << "custom_name" << YAML::Value << spotlight->custom_name;
            out << YAML::Key << "constant" << YAML::Value << spotlight->constant;
            out << YAML::Key << "linear" << YAML::Value << spotlight->linear;
            out << YAML::Key << "quadratic" << YAML::Value << spotlight->quadratic;
            out << YAML::Key << "scattering_factor" << YAML::Value << spotlight->scattering_factor;
            out << YAML::Key << "cut_off" << YAML::Value << spotlight->cut_off;
            out << YAML::Key << "outer_cut_off" << YAML::Value << spotlight->outer_cut_off;
        }
        else
        if (auto const pointlight = std::dynamic_pointer_cast<class PointLight>(component); pointlight != nullptr)
        {
            out << YAML::Key << "ComponentName" << YAML::Value << "PointLightComponent";
            out << YAML::Key << "guid" << YAML::Value << pointlight->guid;
            out << YAML::Key << "custom_name" << YAML::Value << pointlight->custom_name;
            out << YAML::Key << "constant" << YAML::Value << pointlight->constant;
            out << YAML::Key << "linear" << YAML::Value << pointlight->linear;
            out << YAML::Key << "quadratic" << YAML::Value << pointlight->quadratic;
        }
        else
        if (auto const directionallight = std::dynamic_pointer_cast<class DirectionalLight>(component); directionallight != nullptr)
        {
            out << YAML::Key << "ComponentName" << YAML::Value << "DirectionalLightComponent";
            out << YAML::Key << "guid" << YAML::Value << directionallight->guid;
            out << YAML::Key << "custom_name" << YAML::Value << directionallight->custom_name;
        }
        else
        {
            out << YAML::Key << "ComponentName" << YAML::Value << "LightComponent";
            out << YAML::Key << "guid" << YAML::Value << light->guid;
            out << YAML::Key << "custom_name" << YAML::Value << light->custom_name;
        }
        out << YAML::Key << "ambient" << YAML::Value << light->ambient;
        out << YAML::Key << "diffuse" << YAML::Value << light->diffuse;
        out << YAML::Key << "specular" << YAML::Value << light->specular;
        out << YAML::Key << "m_near_plane" << YAML::Value << light->m_near_plane;
        out << YAML::Key << "m_far_plane" << YAML::Value << light->m_far_plane;
        out << YAML::Key << "m_blocker_search_num_samples" << YAML::Value << light->m_blocker_search_num_samples;
        out << YAML::Key << "m_pcf_num_samples" << YAML::Value << light->m_pcf_num_samples;
        out << YAML::Key << "m_light_world_size" << YAML::Value << light->m_light_world_size;
        out << YAML::Key << "m_light_frustum_width" << YAML::Value << light->m_light_frustum_width;
        out << YAML::EndMap;
    }
    else
    if (auto const nowprompttrigger = std::dynamic_pointer_cast<class NowPromptTrigger>(component); nowprompttrigger != nullptr)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ComponentName" << YAML::Value << "NowPromptTriggerComponent";
        out << YAML::Key << "guid" << YAML::Value << nowprompttrigger->guid;
        out << YAML::Key << "custom_name" << YAML::Value << nowprompttrigger->custom_name;
        out << YAML::EndMap;
    }
    else
    if (auto const particlesystem = std::dynamic_pointer_cast<class ParticleSystem>(component); particlesystem != nullptr)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ComponentName" << YAML::Value << "ParticleSystemComponent";
        out << YAML::Key << "guid" << YAML::Value << particlesystem->guid;
        out << YAML::Key << "custom_name" << YAML::Value << particlesystem->custom_name;
        out << YAML::Key << "particle_type" << YAML::Value << particlesystem->particle_type;
        out << YAML::Key << "play_once" << YAML::Value << particlesystem->play_once;
        out << YAML::Key << "rotate_particles" << YAML::Value << particlesystem->rotate_particles;
        out << YAML::Key << "spawn_instantly" << YAML::Value << particlesystem->spawn_instantly;
        out << YAML::Key << "sprite_path" << YAML::Value << particlesystem->sprite_path;
        out << YAML::Key << "min_spawn_interval" << YAML::Value << particlesystem->min_spawn_interval;
        out << YAML::Key << "max_spawn_interval" << YAML::Value << particlesystem->max_spawn_interval;
        out << YAML::Key << "start_velocity_1" << YAML::Value << particlesystem->start_velocity_1;
        out << YAML::Key << "start_velocity_2" << YAML::Value << particlesystem->start_velocity_2;
        out << YAML::Key << "min_spawn_alpha" << YAML::Value << particlesystem->min_spawn_alpha;
        out << YAML::Key << "max_spawn_alpha" << YAML::Value << particlesystem->max_spawn_alpha;
        out << YAML::Key << "start_min_particle_size" << YAML::Value << particlesystem->start_min_particle_size;
        out << YAML::Key << "start_max_particle_size" << YAML::Value << particlesystem->start_max_particle_size;
        out << YAML::Key << "emitter_bounds" << YAML::Value << particlesystem->emitter_bounds;
        out << YAML::Key << "min_spawn_count" << YAML::Value << particlesystem->min_spawn_count;
        out << YAML::Key << "max_spawn_count" << YAML::Value << particlesystem->max_spawn_count;
        out << YAML::Key << "start_color_1" << YAML::Value << particlesystem->start_color_1;
        out << YAML::Key << "end_color_1" << YAML::Value << particlesystem->end_color_1;
        out << YAML::Key << "lifetime_1" << YAML::Value << particlesystem->lifetime_1;
        out << YAML::Key << "lifetime_2" << YAML::Value << particlesystem->lifetime_2;
        out << YAML::Key << "m_simulate_in_world_space" << YAML::Value << particlesystem->m_simulate_in_world_space;
        out << YAML::EndMap;
    }
    else
    if (auto const sound = std::dynamic_pointer_cast<class Sound>(component); sound != nullptr)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ComponentName" << YAML::Value << "SoundComponent";
        out << YAML::Key << "guid" << YAML::Value << sound->guid;
        out << YAML::Key << "custom_name" << YAML::Value << sound->custom_name;
        out << YAML::Key << "path" << YAML::Value << sound->path;
        out << YAML::Key << "volume" << YAML::Value << sound->volume;
        out << YAML::Key << "play_on_awake" << YAML::Value << sound->play_on_awake;
        out << YAML::Key << "is_positional" << YAML::Value << sound->is_positional;
        out << YAML::EndMap;
    }
    else
    if (auto const soundlistener = std::dynamic_pointer_cast<class SoundListener>(component); soundlistener != nullptr)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ComponentName" << YAML::Value << "SoundListenerComponent";
        out << YAML::Key << "guid" << YAML::Value << soundlistener->guid;
        out << YAML::Key << "custom_name" << YAML::Value << soundlistener->custom_name;
        out << YAML::EndMap;
    }
    else
    if (auto const clock = std::dynamic_pointer_cast<class Clock>(component); clock != nullptr)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ComponentName" << YAML::Value << "ClockComponent";
        out << YAML::Key << "guid" << YAML::Value << clock->guid;
        out << YAML::Key << "custom_name" << YAML::Value << clock->custom_name;
        out << YAML::EndMap;
    }
    else
    if (auto const credits = std::dynamic_pointer_cast<class Credits>(component); credits != nullptr)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ComponentName" << YAML::Value << "CreditsComponent";
        out << YAML::Key << "guid" << YAML::Value << credits->guid;
        out << YAML::Key << "custom_name" << YAML::Value << credits->custom_name;
        out << YAML::Key << "back_to_menu_button" << YAML::Value << credits->back_to_menu_button;
        out << YAML::EndMap;
    }
    else
    if (auto const customer = std::dynamic_pointer_cast<class Customer>(component); customer != nullptr)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ComponentName" << YAML::Value << "CustomerComponent";
        out << YAML::Key << "guid" << YAML::Value << customer->guid;
        out << YAML::Key << "custom_name" << YAML::Value << customer->custom_name;
        out << YAML::Key << "collider" << YAML::Value << customer->collider;
        out << YAML::Key << "left_hand" << YAML::Value << customer->left_hand;
        out << YAML::Key << "right_hand" << YAML::Value << customer->right_hand;
        out << YAML::EndMap;
    }
    else
    if (auto const customermanager = std::dynamic_pointer_cast<class CustomerManager>(component); customermanager != nullptr)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ComponentName" << YAML::Value << "CustomerManagerComponent";
        out << YAML::Key << "guid" << YAML::Value << customermanager->guid;
        out << YAML::Key << "custom_name" << YAML::Value << customermanager->custom_name;
        out << YAML::Key << "destinations_after_feeding" << YAML::Value << customermanager->destinations_after_feeding;
        out << YAML::Key << "destination_curve" << YAML::Value << customermanager->destination_curve;
        out << YAML::Key << "customer_prefab" << YAML::Value << customermanager->customer_prefab;
        out << YAML::EndMap;
    }
    else
    if (auto const factory = std::dynamic_pointer_cast<class Factory>(component); factory != nullptr)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ComponentName" << YAML::Value << "FactoryComponent";
        out << YAML::Key << "guid" << YAML::Value << factory->guid;
        out << YAML::Key << "custom_name" << YAML::Value << factory->custom_name;
        out << YAML::Key << "type" << YAML::Value << factory->type;
        out << YAML::Key << "lights" << YAML::Value << factory->lights;
        out << YAML::Key << "factory_light" << YAML::Value << factory->factory_light;
        out << YAML::EndMap;
    }
    else
    if (auto const fieldcell = std::dynamic_pointer_cast<class FieldCell>(component); fieldcell != nullptr)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ComponentName" << YAML::Value << "FieldCellComponent";
        out << YAML::Key << "guid" << YAML::Value << fieldcell->guid;
        out << YAML::Key << "custom_name" << YAML::Value << fieldcell->custom_name;
        out << YAML::EndMap;
    }
    else
    if (auto const fieldgrid = std::dynamic_pointer_cast<class FieldGrid>(component); fieldgrid != nullptr)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ComponentName" << YAML::Value << "FieldGridComponent";
        out << YAML::Key << "guid" << YAML::Value << fieldgrid->guid;
        out << YAML::Key << "custom_name" << YAML::Value << fieldgrid->custom_name;
        out << YAML::Key << "rows_number" << YAML::Value << fieldgrid->rows_number;
        out << YAML::Key << "punishment_multiplier" << YAML::Value << fieldgrid->punishment_multiplier;
        out << YAML::Key << "punishment_power" << YAML::Value << fieldgrid->punishment_power;
        out << YAML::EndMap;
    }
    else
    if (auto const gamecontroller = std::dynamic_pointer_cast<class GameController>(component); gamecontroller != nullptr)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ComponentName" << YAML::Value << "GameControllerComponent";
        out << YAML::Key << "guid" << YAML::Value << gamecontroller->guid;
        out << YAML::Key << "custom_name" << YAML::Value << gamecontroller->custom_name;
        out << YAML::Key << "current_scene" << YAML::Value << gamecontroller->current_scene;
        out << YAML::Key << "next_scene" << YAML::Value << gamecontroller->next_scene;
        out << YAML::Key << "dialog_manager" << YAML::Value << gamecontroller->dialog_manager;
        out << YAML::EndMap;
    }
    else
    if (auto const hovercraftwithoutkeeper = std::dynamic_pointer_cast<class HovercraftWithoutKeeper>(component); hovercraftwithoutkeeper != nullptr)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ComponentName" << YAML::Value << "HovercraftWithoutKeeperComponent";
        out << YAML::Key << "guid" << YAML::Value << hovercraftwithoutkeeper->guid;
        out << YAML::Key << "custom_name" << YAML::Value << hovercraftwithoutkeeper->custom_name;
        out << YAML::EndMap;
    }
    else
    if (auto const icebound = std::dynamic_pointer_cast<class IceBound>(component); icebound != nullptr)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ComponentName" << YAML::Value << "IceBoundComponent";
        out << YAML::Key << "guid" << YAML::Value << icebound->guid;
        out << YAML::Key << "custom_name" << YAML::Value << icebound->custom_name;
        out << YAML::EndMap;
    }
    else
    if (auto const jeep = std::dynamic_pointer_cast<class Jeep>(component); jeep != nullptr)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ComponentName" << YAML::Value << "JeepComponent";
        out << YAML::Key << "guid" << YAML::Value << jeep->guid;
        out << YAML::Key << "custom_name" << YAML::Value << jeep->custom_name;
        out << YAML::Key << "maximum_speed" << YAML::Value << jeep->maximum_speed;
        out << YAML::Key << "acceleration" << YAML::Value << jeep->acceleration;
        out << YAML::Key << "deceleration" << YAML::Value << jeep->deceleration;
        out << YAML::Key << "is_active" << YAML::Value << jeep->is_active;
        out << YAML::Key << "player" << YAML::Value << jeep->player;
        out << YAML::EndMap;
    }
    else
    if (auto const levelcontroller = std::dynamic_pointer_cast<class LevelController>(component); levelcontroller != nullptr)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ComponentName" << YAML::Value << "LevelControllerComponent";
        out << YAML::Key << "guid" << YAML::Value << levelcontroller->guid;
        out << YAML::Key << "custom_name" << YAML::Value << levelcontroller->custom_name;
        out << YAML::Key << "map_time" << YAML::Value << levelcontroller->map_time;
        out << YAML::Key << "map_food" << YAML::Value << levelcontroller->map_food;
        out << YAML::Key << "maximum_lighthouse_level" << YAML::Value << levelcontroller->maximum_lighthouse_level;
        out << YAML::Key << "factories" << YAML::Value << levelcontroller->factories;
        out << YAML::Key << "port" << YAML::Value << levelcontroller->port;
        out << YAML::Key << "lighthouse" << YAML::Value << levelcontroller->lighthouse;
        out << YAML::Key << "customer_manager" << YAML::Value << levelcontroller->customer_manager;
        out << YAML::Key << "playfield_width" << YAML::Value << levelcontroller->playfield_width;
        out << YAML::Key << "playfield_additional_width" << YAML::Value << levelcontroller->playfield_additional_width;
        out << YAML::Key << "playfield_height" << YAML::Value << levelcontroller->playfield_height;
        out << YAML::Key << "playfield_y_shift" << YAML::Value << levelcontroller->playfield_y_shift;
        out << YAML::Key << "ships_limit_curve" << YAML::Value << levelcontroller->ships_limit_curve;
        out << YAML::Key << "ships_limit" << YAML::Value << levelcontroller->ships_limit;
        out << YAML::Key << "ships_speed_curve" << YAML::Value << levelcontroller->ships_speed_curve;
        out << YAML::Key << "ships_speed" << YAML::Value << levelcontroller->ships_speed;
        out << YAML::Key << "ships_range_curve" << YAML::Value << levelcontroller->ships_range_curve;
        out << YAML::Key << "ships_turn_curve" << YAML::Value << levelcontroller->ships_turn_curve;
        out << YAML::Key << "ships_additional_speed_curve" << YAML::Value << levelcontroller->ships_additional_speed_curve;
        out << YAML::Key << "pirates_in_control_curve" << YAML::Value << levelcontroller->pirates_in_control_curve;
        out << YAML::Key << "is_tutorial" << YAML::Value << levelcontroller->is_tutorial;
        out << YAML::Key << "starting_packages" << YAML::Value << levelcontroller->starting_packages;
        out << YAML::Key << "tutorial_level" << YAML::Value << levelcontroller->tutorial_level;
        out << YAML::EndMap;
    }
    else
    if (auto const lighthouse = std::dynamic_pointer_cast<class Lighthouse>(component); lighthouse != nullptr)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ComponentName" << YAML::Value << "LighthouseComponent";
        out << YAML::Key << "guid" << YAML::Value << lighthouse->guid;
        out << YAML::Key << "custom_name" << YAML::Value << lighthouse->custom_name;
        out << YAML::Key << "light" << YAML::Value << lighthouse->light;
        out << YAML::Key << "water" << YAML::Value << lighthouse->water;
        out << YAML::Key << "spawn_position" << YAML::Value << lighthouse->spawn_position;
        out << YAML::EndMap;
    }
    else
    if (auto const lighthousekeeper = std::dynamic_pointer_cast<class LighthouseKeeper>(component); lighthousekeeper != nullptr)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ComponentName" << YAML::Value << "LighthouseKeeperComponent";
        out << YAML::Key << "guid" << YAML::Value << lighthousekeeper->guid;
        out << YAML::Key << "custom_name" << YAML::Value << lighthousekeeper->custom_name;
        out << YAML::Key << "maximum_speed" << YAML::Value << lighthousekeeper->maximum_speed;
        out << YAML::Key << "acceleration" << YAML::Value << lighthousekeeper->acceleration;
        out << YAML::Key << "deceleration" << YAML::Value << lighthousekeeper->deceleration;
        out << YAML::Key << "lighthouse" << YAML::Value << lighthousekeeper->lighthouse;
        out << YAML::Key << "port" << YAML::Value << lighthousekeeper->port;
        out << YAML::Key << "keeper_dust" << YAML::Value << lighthousekeeper->keeper_dust;
        out << YAML::Key << "keeper_splash" << YAML::Value << lighthousekeeper->keeper_splash;
        out << YAML::Key << "packages" << YAML::Value << lighthousekeeper->packages;
        out << YAML::EndMap;
    }
    else
    if (auto const lighthouselight = std::dynamic_pointer_cast<class LighthouseLight>(component); lighthouselight != nullptr)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ComponentName" << YAML::Value << "LighthouseLightComponent";
        out << YAML::Key << "guid" << YAML::Value << lighthouselight->guid;
        out << YAML::Key << "custom_name" << YAML::Value << lighthouselight->custom_name;
        out << YAML::Key << "spotlight" << YAML::Value << lighthouselight->spotlight;
        out << YAML::Key << "spotlight_beam_width" << YAML::Value << lighthouselight->spotlight_beam_width;
        out << YAML::EndMap;
    }
    else
    if (auto const player = std::dynamic_pointer_cast<class Player>(component); player != nullptr)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ComponentName" << YAML::Value << "PlayerComponent";
        out << YAML::Key << "guid" << YAML::Value << player->guid;
        out << YAML::Key << "custom_name" << YAML::Value << player->custom_name;
        out << YAML::Key << "packages_text" << YAML::Value << player->packages_text;
        out << YAML::Key << "flashes_text" << YAML::Value << player->flashes_text;
        out << YAML::Key << "level_text" << YAML::Value << player->level_text;
        out << YAML::Key << "clock_text" << YAML::Value << player->clock_text;
        out << YAML::EndMap;
    }
    else
    if (auto const popup = std::dynamic_pointer_cast<class Popup>(component); popup != nullptr)
    {
        out << YAML::BeginMap;
        // # Put new Popup kid here
        if (auto const endscreenfoliage = std::dynamic_pointer_cast<class EndScreenFoliage>(component); endscreenfoliage != nullptr)
        {
            out << YAML::Key << "ComponentName" << YAML::Value << "EndScreenFoliageComponent";
            out << YAML::Key << "guid" << YAML::Value << endscreenfoliage->guid;
            out << YAML::Key << "custom_name" << YAML::Value << endscreenfoliage->custom_name;
            out << YAML::Key << "next_level_button" << YAML::Value << endscreenfoliage->next_level_button;
            out << YAML::Key << "percentage" << YAML::Value << endscreenfoliage->percentage;
            out << YAML::Key << "percentage_gained" << YAML::Value << endscreenfoliage->percentage_gained;
            out << YAML::Key << "percentage_text" << YAML::Value << endscreenfoliage->percentage_text;
            out << YAML::Key << "percentage_bar" << YAML::Value << endscreenfoliage->percentage_bar;
        }
        else
        if (auto const endscreen = std::dynamic_pointer_cast<class EndScreen>(component); endscreen != nullptr)
        {
            out << YAML::Key << "ComponentName" << YAML::Value << "EndScreenComponent";
            out << YAML::Key << "guid" << YAML::Value << endscreen->guid;
            out << YAML::Key << "custom_name" << YAML::Value << endscreen->custom_name;
            out << YAML::Key << "is_failed" << YAML::Value << endscreen->is_failed;
            out << YAML::Key << "number_of_stars" << YAML::Value << endscreen->number_of_stars;
            out << YAML::Key << "stars" << YAML::Value << endscreen->stars;
            out << YAML::Key << "star_scale" << YAML::Value << endscreen->star_scale;
            out << YAML::Key << "next_level_button" << YAML::Value << endscreen->next_level_button;
            out << YAML::Key << "restart_button" << YAML::Value << endscreen->restart_button;
            out << YAML::Key << "menu_button" << YAML::Value << endscreen->menu_button;
        }
        else
        {
            out << YAML::Key << "ComponentName" << YAML::Value << "PopupComponent";
            out << YAML::Key << "guid" << YAML::Value << popup->guid;
            out << YAML::Key << "custom_name" << YAML::Value << popup->custom_name;
        }
        out << YAML::EndMap;
    }
    else
    if (auto const port = std::dynamic_pointer_cast<class Port>(component); port != nullptr)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ComponentName" << YAML::Value << "PortComponent";
        out << YAML::Key << "guid" << YAML::Value << port->guid;
        out << YAML::Key << "custom_name" << YAML::Value << port->custom_name;
        out << YAML::Key << "lights" << YAML::Value << port->lights;
        out << YAML::EndMap;
    }
    else
    if (auto const ship = std::dynamic_pointer_cast<class Ship>(component); ship != nullptr)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ComponentName" << YAML::Value << "ShipComponent";
        out << YAML::Key << "guid" << YAML::Value << ship->guid;
        out << YAML::Key << "custom_name" << YAML::Value << ship->custom_name;
        out << YAML::Key << "type" << YAML::Value << ship->type;
        out << YAML::Key << "light" << YAML::Value << ship->light;
        out << YAML::Key << "spawner" << YAML::Value << ship->spawner;
        out << YAML::Key << "eyes" << YAML::Value << ship->eyes;
        out << YAML::Key << "my_light" << YAML::Value << ship->my_light;
        out << YAML::EndMap;
    }
    else
    if (auto const shipeyes = std::dynamic_pointer_cast<class ShipEyes>(component); shipeyes != nullptr)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ComponentName" << YAML::Value << "ShipEyesComponent";
        out << YAML::Key << "guid" << YAML::Value << shipeyes->guid;
        out << YAML::Key << "custom_name" << YAML::Value << shipeyes->custom_name;
        out << YAML::EndMap;
    }
    else
    if (auto const shipspawner = std::dynamic_pointer_cast<class ShipSpawner>(component); shipspawner != nullptr)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ComponentName" << YAML::Value << "ShipSpawnerComponent";
        out << YAML::Key << "guid" << YAML::Value << shipspawner->guid;
        out << YAML::Key << "custom_name" << YAML::Value << shipspawner->custom_name;
        out << YAML::Key << "paths" << YAML::Value << shipspawner->paths;
        out << YAML::Key << "floaters_manager" << YAML::Value << shipspawner->floaters_manager;
        out << YAML::Key << "light" << YAML::Value << shipspawner->light;
        out << YAML::Key << "last_chance_food_threshold" << YAML::Value << shipspawner->last_chance_food_threshold;
        out << YAML::Key << "last_chance_time_threshold" << YAML::Value << shipspawner->last_chance_time_threshold;
        out << YAML::Key << "main_event_spawn" << YAML::Value << shipspawner->main_event_spawn;
        out << YAML::Key << "backup_spawn" << YAML::Value << shipspawner->backup_spawn;
        out << YAML::EndMap;
    }
    else
    if (auto const thanks = std::dynamic_pointer_cast<class Thanks>(component); thanks != nullptr)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ComponentName" << YAML::Value << "ThanksComponent";
        out << YAML::Key << "guid" << YAML::Value << thanks->guid;
        out << YAML::Key << "custom_name" << YAML::Value << thanks->custom_name;
        out << YAML::Key << "back_to_menu_button" << YAML::Value << thanks->back_to_menu_button;
        out << YAML::EndMap;
    }
    else
    if (auto const truther = std::dynamic_pointer_cast<class Truther>(component); truther != nullptr)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ComponentName" << YAML::Value << "TrutherComponent";
        out << YAML::Key << "guid" << YAML::Value << truther->guid;
        out << YAML::Key << "custom_name" << YAML::Value << truther->custom_name;
        out << YAML::Key << "truther_bends" << YAML::Value << truther->truther_bends;
        out << YAML::Key << "maximum_speed" << YAML::Value << truther->maximum_speed;
        out << YAML::Key << "acceleration" << YAML::Value << truther->acceleration;
        out << YAML::Key << "deceleration" << YAML::Value << truther->deceleration;
        out << YAML::Key << "jump_power" << YAML::Value << truther->jump_power;
        out << YAML::Key << "jump_horizontal_power" << YAML::Value << truther->jump_horizontal_power;
        out << YAML::Key << "gravitation" << YAML::Value << truther->gravitation;
        out << YAML::Key << "suck_power" << YAML::Value << truther->suck_power;
        out << YAML::Key << "is_sucked" << YAML::Value << truther->is_sucked;
        out << YAML::Key << "wheat_overlay" << YAML::Value << truther->wheat_overlay;
        out << YAML::Key << "model_entity" << YAML::Value << truther->model_entity;
        out << YAML::EndMap;
    }
    else
    if (auto const wheatoverlay = std::dynamic_pointer_cast<class WheatOverlay>(component); wheatoverlay != nullptr)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ComponentName" << YAML::Value << "WheatOverlayComponent";
        out << YAML::Key << "guid" << YAML::Value << wheatoverlay->guid;
        out << YAML::Key << "custom_name" << YAML::Value << wheatoverlay->custom_name;
        out << YAML::EndMap;
    }
    else
    if (auto const playerinput = std::dynamic_pointer_cast<class PlayerInput>(component); playerinput != nullptr)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ComponentName" << YAML::Value << "PlayerInputComponent";
        out << YAML::Key << "guid" << YAML::Value << playerinput->guid;
        out << YAML::Key << "custom_name" << YAML::Value << playerinput->custom_name;
        out << YAML::Key << "player_speed" << YAML::Value << playerinput->player_speed;
        out << YAML::Key << "camera_speed" << YAML::Value << playerinput->camera_speed;
        out << YAML::EndMap;
    }
    else
    if (auto const cow = std::dynamic_pointer_cast<class Cow>(component); cow != nullptr)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ComponentName" << YAML::Value << "CowComponent";
        out << YAML::Key << "guid" << YAML::Value << cow->guid;
        out << YAML::Key << "custom_name" << YAML::Value << cow->custom_name;
        out << YAML::Key << "cow_manager" << YAML::Value << cow->cow_manager;
        out << YAML::Key << "is_sucked" << YAML::Value << cow->is_sucked;
        out << YAML::EndMap;
    }
    else
    if (auto const cowmanager = std::dynamic_pointer_cast<class CowManager>(component); cowmanager != nullptr)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ComponentName" << YAML::Value << "CowManagerComponent";
        out << YAML::Key << "guid" << YAML::Value << cowmanager->guid;
        out << YAML::Key << "custom_name" << YAML::Value << cowmanager->custom_name;
        out << YAML::Key << "dialogue_prompt_controller" << YAML::Value << cowmanager->dialogue_prompt_controller;
        out << YAML::Key << "paths" << YAML::Value << cowmanager->paths;
        out << YAML::Key << "cows" << YAML::Value << cowmanager->cows;
        out << YAML::Key << "truther" << YAML::Value << cowmanager->truther;
        out << YAML::Key << "ufo" << YAML::Value << cowmanager->ufo;
        out << YAML::Key << "jeep" << YAML::Value << cowmanager->jeep;
        out << YAML::Key << "wheat_overlay" << YAML::Value << cowmanager->wheat_overlay;
        out << YAML::Key << "clock_text_ref" << YAML::Value << cowmanager->clock_text_ref;
        out << YAML::Key << "friel_grid" << YAML::Value << cowmanager->friel_grid;
        out << YAML::Key << "event_timer" << YAML::Value << cowmanager->event_timer;
        out << YAML::Key << "does_level_ended" << YAML::Value << cowmanager->does_level_ended;
        out << YAML::EndMap;
    }
    else
    if (auto const jeepreflector = std::dynamic_pointer_cast<class JeepReflector>(component); jeepreflector != nullptr)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ComponentName" << YAML::Value << "JeepReflectorComponent";
        out << YAML::Key << "guid" << YAML::Value << jeepreflector->guid;
        out << YAML::Key << "custom_name" << YAML::Value << jeepreflector->custom_name;
        out << YAML::Key << "jeep" << YAML::Value << jeepreflector->jeep;
        out << YAML::EndMap;
    }
    else
    if (auto const ufo = std::dynamic_pointer_cast<class UFO>(component); ufo != nullptr)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ComponentName" << YAML::Value << "UFOComponent";
        out << YAML::Key << "guid" << YAML::Value << ufo->guid;
        out << YAML::Key << "custom_name" << YAML::Value << ufo->custom_name;
        out << YAML::Key << "field_grid" << YAML::Value << ufo->field_grid;
        out << YAML::Key << "truther" << YAML::Value << ufo->truther;
        out << YAML::Key << "attract_bean" << YAML::Value << ufo->attract_bean;
        out << YAML::Key << "cow_manager" << YAML::Value << ufo->cow_manager;
        out << YAML::Key << "particles" << YAML::Value << ufo->particles;
        out << YAML::EndMap;
    }
    else
    if (auto const wheat = std::dynamic_pointer_cast<class Wheat>(component); wheat != nullptr)
    {
        out << YAML::BeginMap;
        out << YAML::Key << "ComponentName" << YAML::Value << "WheatComponent";
        out << YAML::Key << "guid" << YAML::Value << wheat->guid;
        out << YAML::Key << "custom_name" << YAML::Value << wheat->custom_name;
        out << YAML::EndMap;
    }
    else
    {
        // NOTE: This only returns unmangled name while using the MSVC compiler
        std::string const name = typeid(*component).name();
        std::cout << "Error. Serialization of component " << name.substr(6) << " failed." << "\n";
    }
    // # Put new serialization here
}

void SceneSerializer::serialize_entity(YAML::Emitter& out, std::shared_ptr<Entity> const& entity)
{
    out << YAML::BeginMap; // Entity
    out << YAML::Key << "Entity" << YAML::Value << entity->name;
    out << YAML::Key << "guid" << YAML::Value << entity->guid;
    out << YAML::Key << "Name" << YAML::Value << entity->name;

    {
        out << YAML::Key << "TransformComponent";
        out << YAML::BeginMap; // TransformComponent

        out << YAML::Key << "Translation" << YAML::Value << entity->transform->get_local_position();
        out << YAML::Key << "Rotation" << YAML::Value << entity->transform->get_euler_angles();
        out << YAML::Key << "Scale" << YAML::Value << entity->transform->get_local_scale();

        if (!entity->transform->parent.expired())
        {
            out << YAML::Key << "Parent";
            out << YAML::BeginMap;
            out << YAML::Key << "guid" << YAML::Value << entity->transform->parent.lock()->entity.lock()->guid;
            out << YAML::EndMap;
        }
        else
        {
            out << YAML::Key << "Parent";
            out << YAML::BeginMap;
            out << YAML::Key << "guid" << YAML::Value << "";
            out << YAML::EndMap;
        }

        out << YAML::EndMap; // TransformComponent
    }

    out << YAML::Key << "Components";
    out << YAML::BeginSeq; // Components
    for (auto const& component : entity->components)
    {
        if (false)
        {
            // Custom serialization here
        }
        else
        {
            auto_serialize_component(out, component);
        }
    }
    out << YAML::EndSeq; // Components

    out << YAML::EndMap; // Entity
}

void SceneSerializer::serialize_entity_recursively(YAML::Emitter& out, std::shared_ptr<Entity> const& entity)
{
    if (!entity->is_serialized)
    {
        return;
    }

    serialize_entity(out, entity);

    for (auto const& child : entity->transform->children)
    {
        if (child->entity.expired())
            continue;

        serialize_entity_recursively(out, child->entity.lock());
    }
}

void SceneSerializer::auto_deserialize_component(YAML::Node const& component, std::shared_ptr<Entity> const& deserialized_entity,
                                                 bool const first_pass)
{
    auto component_name = component["ComponentName"].as<std::string>();
    // # Auto deserialization start
    if (component_name == "CameraComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = Camera::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class Camera>(get_from_pool(component["guid"].as<std::string>()));
            if (component["width"].IsDefined())
            {
                deserialized_component->width = component["width"].as<float>();
            }
            if (component["height"].IsDefined())
            {
                deserialized_component->height = component["height"].as<float>();
            }
            if (component["fov"].IsDefined())
            {
                deserialized_component->fov = component["fov"].as<float>();
            }
            if (component["near_plane"].IsDefined())
            {
                deserialized_component->near_plane = component["near_plane"].as<float>();
            }
            if (component["far_plane"].IsDefined())
            {
                deserialized_component->far_plane = component["far_plane"].as<float>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "Collider2DComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = Collider2D::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class Collider2D>(get_from_pool(component["guid"].as<std::string>()));
            if (component["offset"].IsDefined())
            {
                deserialized_component->offset = component["offset"].as<glm::vec2>();
            }
            if (component["is_trigger"].IsDefined())
            {
                deserialized_component->is_trigger = component["is_trigger"].as<bool>();
            }
            if (component["is_static"].IsDefined())
            {
                deserialized_component->is_static = component["is_static"].as<bool>();
            }
            if (component["collider_type"].IsDefined())
            {
                deserialized_component->collider_type = component["collider_type"].as<ColliderType2D>();
            }
            if (component["width"].IsDefined())
            {
                deserialized_component->width = component["width"].as<float>();
            }
            if (component["height"].IsDefined())
            {
                deserialized_component->height = component["height"].as<float>();
            }
            if (component["radius"].IsDefined())
            {
                deserialized_component->radius = component["radius"].as<float>();
            }
            if (component["drag"].IsDefined())
            {
                deserialized_component->drag = component["drag"].as<float>();
            }
            if (component["velocity"].IsDefined())
            {
                deserialized_component->velocity = component["velocity"].as<glm::vec2>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "CurveComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = Curve::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class Curve>(get_from_pool(component["guid"].as<std::string>()));
            if (component["points"].IsDefined())
            {
                deserialized_component->points = component["points"].as<std::vector<glm::vec2>>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "PathComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = Path::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class Path>(get_from_pool(component["guid"].as<std::string>()));
            if (component["points"].IsDefined())
            {
                deserialized_component->points = component["points"].as<std::vector<glm::vec2>>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "DebugInputControllerComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = DebugInputController::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class DebugInputController>(get_from_pool(component["guid"].as<std::string>()));
            if (component["gamma"].IsDefined())
            {
                deserialized_component->gamma = component["gamma"].as<float>();
            }
            if (component["exposure"].IsDefined())
            {
                deserialized_component->exposure = component["exposure"].as<float>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "DialoguePromptControllerComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = DialoguePromptController::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class DialoguePromptController>(get_from_pool(component["guid"].as<std::string>()));
            if (component["interp_speed"].IsDefined())
            {
                deserialized_component->interp_speed = component["interp_speed"].as<float>();
            }
            if (component["dialogue_panel"].IsDefined())
            {
                deserialized_component->dialogue_panel = component["dialogue_panel"].as<std::weak_ptr<Button>>();
            }
            if (component["panel_parent"].IsDefined())
            {
                deserialized_component->panel_parent = component["panel_parent"].as<std::weak_ptr<Entity>>();
            }
            if (component["keeper_sprite"].IsDefined())
            {
                deserialized_component->keeper_sprite = component["keeper_sprite"].as<std::weak_ptr<Entity>>();
            }
            if (component["upper_text"].IsDefined())
            {
                deserialized_component->upper_text = component["upper_text"].as<std::weak_ptr<ScreenText>>();
            }
            if (component["middle_text"].IsDefined())
            {
                deserialized_component->middle_text = component["middle_text"].as<std::weak_ptr<ScreenText>>();
            }
            if (component["lower_text"].IsDefined())
            {
                deserialized_component->lower_text = component["lower_text"].as<std::weak_ptr<ScreenText>>();
            }
            if (component["dialogue_objects"].IsDefined())
            {
                deserialized_component->dialogue_objects = component["dialogue_objects"].as<std::vector<DialogueObject>>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "ButtonComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = Button::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class Button>(get_from_pool(component["guid"].as<std::string>()));
            if (component["path_default"].IsDefined())
            {
                deserialized_component->path_default = component["path_default"].as<std::string>();
            }
            if (component["path_hovered"].IsDefined())
            {
                deserialized_component->path_hovered = component["path_hovered"].as<std::string>();
            }
            if (component["path_pressed"].IsDefined())
            {
                deserialized_component->path_pressed = component["path_pressed"].as<std::string>();
            }
            if (component["top_left_corner"].IsDefined())
            {
                deserialized_component->top_left_corner = component["top_left_corner"].as<glm::vec2>();
            }
            if (component["top_right_corner"].IsDefined())
            {
                deserialized_component->top_right_corner = component["top_right_corner"].as<glm::vec2>();
            }
            if (component["bottom_left_corner"].IsDefined())
            {
                deserialized_component->bottom_left_corner = component["bottom_left_corner"].as<glm::vec2>();
            }
            if (component["bottom_right_corner"].IsDefined())
            {
                deserialized_component->bottom_right_corner = component["bottom_right_corner"].as<glm::vec2>();
            }
            if (component["material"].IsDefined())
            {
                deserialized_component->material = component["material"].as<std::shared_ptr<Material>>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "ModelComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = Model::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class Model>(get_from_pool(component["guid"].as<std::string>()));
            if (component["model_path"].IsDefined())
            {
                deserialized_component->model_path = component["model_path"].as<std::string>();
            }
            if (component["material"].IsDefined())
            {
                deserialized_component->material = component["material"].as<std::shared_ptr<Material>>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "CubeComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = Cube::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class Cube>(get_from_pool(component["guid"].as<std::string>()));
            if (component["diffuse_texture_path"].IsDefined())
            {
                deserialized_component->diffuse_texture_path = component["diffuse_texture_path"].as<std::string>();
            }
            if (component["specular_texture_path"].IsDefined())
            {
                deserialized_component->specular_texture_path = component["specular_texture_path"].as<std::string>();
            }
            if (component["model_path"].IsDefined())
            {
                deserialized_component->model_path = component["model_path"].as<std::string>();
            }
            if (component["material"].IsDefined())
            {
                deserialized_component->material = component["material"].as<std::shared_ptr<Material>>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "SphereComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = Sphere::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class Sphere>(get_from_pool(component["guid"].as<std::string>()));
            if (component["sector_count"].IsDefined())
            {
                deserialized_component->sector_count = component["sector_count"].as<u32>();
            }
            if (component["stack_count"].IsDefined())
            {
                deserialized_component->stack_count = component["stack_count"].as<u32>();
            }
            if (component["texture_path"].IsDefined())
            {
                deserialized_component->texture_path = component["texture_path"].as<std::string>();
            }
            if (component["radius"].IsDefined())
            {
                deserialized_component->radius = component["radius"].as<float>();
            }
            if (component["model_path"].IsDefined())
            {
                deserialized_component->model_path = component["model_path"].as<std::string>();
            }
            if (component["material"].IsDefined())
            {
                deserialized_component->material = component["material"].as<std::shared_ptr<Material>>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "SpriteComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = Sprite::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class Sprite>(get_from_pool(component["guid"].as<std::string>()));
            if (component["diffuse_texture_path"].IsDefined())
            {
                deserialized_component->diffuse_texture_path = component["diffuse_texture_path"].as<std::string>();
            }
            if (component["model_path"].IsDefined())
            {
                deserialized_component->model_path = component["model_path"].as<std::string>();
            }
            if (component["material"].IsDefined())
            {
                deserialized_component->material = component["material"].as<std::shared_ptr<Material>>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "WaterComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = Water::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class Water>(get_from_pool(component["guid"].as<std::string>()));
            if (component["waves"].IsDefined())
            {
                deserialized_component->waves = component["waves"].as<std::vector<DXWave>>();
            }
            if (component["m_ps_buffer"].IsDefined())
            {
                deserialized_component->m_ps_buffer = component["m_ps_buffer"].as<ConstantBufferWater>();
            }
            if (component["tesselation_level"].IsDefined())
            {
                deserialized_component->tesselation_level = component["tesselation_level"].as<u32>();
            }
            if (component["model_path"].IsDefined())
            {
                deserialized_component->model_path = component["model_path"].as<std::string>();
            }
            if (component["material"].IsDefined())
            {
                deserialized_component->material = component["material"].as<std::shared_ptr<Material>>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "PanelComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = Panel::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class Panel>(get_from_pool(component["guid"].as<std::string>()));
            if (component["background_path"].IsDefined())
            {
                deserialized_component->background_path = component["background_path"].as<std::string>();
            }
            if (component["material"].IsDefined())
            {
                deserialized_component->material = component["material"].as<std::shared_ptr<Material>>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "QuadComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = Quad::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class Quad>(get_from_pool(component["guid"].as<std::string>()));
            if (component["path"].IsDefined())
            {
                deserialized_component->path = component["path"].as<std::string>();
            }
            if (component["material"].IsDefined())
            {
                deserialized_component->material = component["material"].as<std::shared_ptr<Material>>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "ScreenTextComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = ScreenText::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class ScreenText>(get_from_pool(component["guid"].as<std::string>()));
            if (component["text"].IsDefined())
            {
                deserialized_component->text = component["text"].as<std::string>();
            }
            if (component["position"].IsDefined())
            {
                deserialized_component->position = component["position"].as<glm::vec2>();
            }
            if (component["font_size"].IsDefined())
            {
                deserialized_component->font_size = component["font_size"].as<float>();
            }
            if (component["color"].IsDefined())
            {
                deserialized_component->color = component["color"].as<u32>();
            }
            if (component["flags"].IsDefined())
            {
                deserialized_component->flags = component["flags"].as<u16>();
            }
            if (component["font_name"].IsDefined())
            {
                deserialized_component->font_name = component["font_name"].as<std::string>();
            }
            if (component["bold"].IsDefined())
            {
                deserialized_component->bold = component["bold"].as<bool>();
            }
            if (component["button_ref"].IsDefined())
            {
                deserialized_component->button_ref = component["button_ref"].as<std::weak_ptr<Button>>();
            }
            if (component["material"].IsDefined())
            {
                deserialized_component->material = component["material"].as<std::shared_ptr<Material>>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "ExampleDynamicTextComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = ExampleDynamicText::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class ExampleDynamicText>(get_from_pool(component["guid"].as<std::string>()));
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "ExampleUIBarComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = ExampleUIBar::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class ExampleUIBar>(get_from_pool(component["guid"].as<std::string>()));
            if (component["value"].IsDefined())
            {
                deserialized_component->value = component["value"].as<float>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "FloaterComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = Floater::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class Floater>(get_from_pool(component["guid"].as<std::string>()));
            if (component["sink"].IsDefined())
            {
                deserialized_component->sink = component["sink"].as<float>();
            }
            if (component["side_floaters_offset"].IsDefined())
            {
                deserialized_component->side_floaters_offset = component["side_floaters_offset"].as<float>();
            }
            if (component["side_roation_strength"].IsDefined())
            {
                deserialized_component->side_roation_strength = component["side_roation_strength"].as<float>();
            }
            if (component["forward_rotation_strength"].IsDefined())
            {
                deserialized_component->forward_rotation_strength = component["forward_rotation_strength"].as<float>();
            }
            if (component["forward_floaters_offest"].IsDefined())
            {
                deserialized_component->forward_floaters_offest = component["forward_floaters_offest"].as<float>();
            }
            if (component["water"].IsDefined())
            {
                deserialized_component->water = component["water"].as<std::weak_ptr<Water>>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "FloatersManagerComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = FloatersManager::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class FloatersManager>(get_from_pool(component["guid"].as<std::string>()));
            if (component["big_boat_settings"].IsDefined())
            {
                deserialized_component->big_boat_settings = component["big_boat_settings"].as<FloaterSettings>();
            }
            if (component["small_boat_settings"].IsDefined())
            {
                deserialized_component->small_boat_settings = component["small_boat_settings"].as<FloaterSettings>();
            }
            if (component["medium_boat_settings"].IsDefined())
            {
                deserialized_component->medium_boat_settings = component["medium_boat_settings"].as<FloaterSettings>();
            }
            if (component["tool_boat_settings"].IsDefined())
            {
                deserialized_component->tool_boat_settings = component["tool_boat_settings"].as<FloaterSettings>();
            }
            if (component["pirate_boat_settings"].IsDefined())
            {
                deserialized_component->pirate_boat_settings = component["pirate_boat_settings"].as<FloaterSettings>();
            }
            if (component["water"].IsDefined())
            {
                deserialized_component->water = component["water"].as<std::weak_ptr<Water>>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "FloeButtonComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = FloeButton::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class FloeButton>(get_from_pool(component["guid"].as<std::string>()));
            if (component["floe_button_type"].IsDefined())
            {
                deserialized_component->floe_button_type = component["floe_button_type"].as<FloeButtonType>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "DirectionalLightComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = DirectionalLight::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class DirectionalLight>(get_from_pool(component["guid"].as<std::string>()));
            if (component["ambient"].IsDefined())
            {
                deserialized_component->ambient = component["ambient"].as<glm::vec3>();
            }
            if (component["diffuse"].IsDefined())
            {
                deserialized_component->diffuse = component["diffuse"].as<glm::vec3>();
            }
            if (component["specular"].IsDefined())
            {
                deserialized_component->specular = component["specular"].as<glm::vec3>();
            }
            if (component["m_near_plane"].IsDefined())
            {
                deserialized_component->m_near_plane = component["m_near_plane"].as<float>();
            }
            if (component["m_far_plane"].IsDefined())
            {
                deserialized_component->m_far_plane = component["m_far_plane"].as<float>();
            }
            if (component["m_blocker_search_num_samples"].IsDefined())
            {
                deserialized_component->m_blocker_search_num_samples = component["m_blocker_search_num_samples"].as<u32>();
            }
            if (component["m_pcf_num_samples"].IsDefined())
            {
                deserialized_component->m_pcf_num_samples = component["m_pcf_num_samples"].as<u32>();
            }
            if (component["m_light_world_size"].IsDefined())
            {
                deserialized_component->m_light_world_size = component["m_light_world_size"].as<float>();
            }
            if (component["m_light_frustum_width"].IsDefined())
            {
                deserialized_component->m_light_frustum_width = component["m_light_frustum_width"].as<float>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "PointLightComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = PointLight::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class PointLight>(get_from_pool(component["guid"].as<std::string>()));
            if (component["constant"].IsDefined())
            {
                deserialized_component->constant = component["constant"].as<float>();
            }
            if (component["linear"].IsDefined())
            {
                deserialized_component->linear = component["linear"].as<float>();
            }
            if (component["quadratic"].IsDefined())
            {
                deserialized_component->quadratic = component["quadratic"].as<float>();
            }
            if (component["ambient"].IsDefined())
            {
                deserialized_component->ambient = component["ambient"].as<glm::vec3>();
            }
            if (component["diffuse"].IsDefined())
            {
                deserialized_component->diffuse = component["diffuse"].as<glm::vec3>();
            }
            if (component["specular"].IsDefined())
            {
                deserialized_component->specular = component["specular"].as<glm::vec3>();
            }
            if (component["m_near_plane"].IsDefined())
            {
                deserialized_component->m_near_plane = component["m_near_plane"].as<float>();
            }
            if (component["m_far_plane"].IsDefined())
            {
                deserialized_component->m_far_plane = component["m_far_plane"].as<float>();
            }
            if (component["m_blocker_search_num_samples"].IsDefined())
            {
                deserialized_component->m_blocker_search_num_samples = component["m_blocker_search_num_samples"].as<u32>();
            }
            if (component["m_pcf_num_samples"].IsDefined())
            {
                deserialized_component->m_pcf_num_samples = component["m_pcf_num_samples"].as<u32>();
            }
            if (component["m_light_world_size"].IsDefined())
            {
                deserialized_component->m_light_world_size = component["m_light_world_size"].as<float>();
            }
            if (component["m_light_frustum_width"].IsDefined())
            {
                deserialized_component->m_light_frustum_width = component["m_light_frustum_width"].as<float>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "SpotLightComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = SpotLight::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class SpotLight>(get_from_pool(component["guid"].as<std::string>()));
            if (component["constant"].IsDefined())
            {
                deserialized_component->constant = component["constant"].as<float>();
            }
            if (component["linear"].IsDefined())
            {
                deserialized_component->linear = component["linear"].as<float>();
            }
            if (component["quadratic"].IsDefined())
            {
                deserialized_component->quadratic = component["quadratic"].as<float>();
            }
            if (component["scattering_factor"].IsDefined())
            {
                deserialized_component->scattering_factor = component["scattering_factor"].as<float>();
            }
            if (component["cut_off"].IsDefined())
            {
                deserialized_component->cut_off = component["cut_off"].as<float>();
            }
            if (component["outer_cut_off"].IsDefined())
            {
                deserialized_component->outer_cut_off = component["outer_cut_off"].as<float>();
            }
            if (component["ambient"].IsDefined())
            {
                deserialized_component->ambient = component["ambient"].as<glm::vec3>();
            }
            if (component["diffuse"].IsDefined())
            {
                deserialized_component->diffuse = component["diffuse"].as<glm::vec3>();
            }
            if (component["specular"].IsDefined())
            {
                deserialized_component->specular = component["specular"].as<glm::vec3>();
            }
            if (component["m_near_plane"].IsDefined())
            {
                deserialized_component->m_near_plane = component["m_near_plane"].as<float>();
            }
            if (component["m_far_plane"].IsDefined())
            {
                deserialized_component->m_far_plane = component["m_far_plane"].as<float>();
            }
            if (component["m_blocker_search_num_samples"].IsDefined())
            {
                deserialized_component->m_blocker_search_num_samples = component["m_blocker_search_num_samples"].as<u32>();
            }
            if (component["m_pcf_num_samples"].IsDefined())
            {
                deserialized_component->m_pcf_num_samples = component["m_pcf_num_samples"].as<u32>();
            }
            if (component["m_light_world_size"].IsDefined())
            {
                deserialized_component->m_light_world_size = component["m_light_world_size"].as<float>();
            }
            if (component["m_light_frustum_width"].IsDefined())
            {
                deserialized_component->m_light_frustum_width = component["m_light_frustum_width"].as<float>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "NowPromptTriggerComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = NowPromptTrigger::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class NowPromptTrigger>(get_from_pool(component["guid"].as<std::string>()));
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "ParticleSystemComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = ParticleSystem::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class ParticleSystem>(get_from_pool(component["guid"].as<std::string>()));
            if (component["particle_type"].IsDefined())
            {
                deserialized_component->particle_type = component["particle_type"].as<ParticleType>();
            }
            if (component["play_once"].IsDefined())
            {
                deserialized_component->play_once = component["play_once"].as<bool>();
            }
            if (component["rotate_particles"].IsDefined())
            {
                deserialized_component->rotate_particles = component["rotate_particles"].as<bool>();
            }
            if (component["spawn_instantly"].IsDefined())
            {
                deserialized_component->spawn_instantly = component["spawn_instantly"].as<bool>();
            }
            if (component["sprite_path"].IsDefined())
            {
                deserialized_component->sprite_path = component["sprite_path"].as<std::string>();
            }
            if (component["min_spawn_interval"].IsDefined())
            {
                deserialized_component->min_spawn_interval = component["min_spawn_interval"].as<float>();
            }
            if (component["max_spawn_interval"].IsDefined())
            {
                deserialized_component->max_spawn_interval = component["max_spawn_interval"].as<float>();
            }
            if (component["start_velocity_1"].IsDefined())
            {
                deserialized_component->start_velocity_1 = component["start_velocity_1"].as<glm::vec3>();
            }
            if (component["start_velocity_2"].IsDefined())
            {
                deserialized_component->start_velocity_2 = component["start_velocity_2"].as<glm::vec3>();
            }
            if (component["min_spawn_alpha"].IsDefined())
            {
                deserialized_component->min_spawn_alpha = component["min_spawn_alpha"].as<float>();
            }
            if (component["max_spawn_alpha"].IsDefined())
            {
                deserialized_component->max_spawn_alpha = component["max_spawn_alpha"].as<float>();
            }
            if (component["start_min_particle_size"].IsDefined())
            {
                deserialized_component->start_min_particle_size = component["start_min_particle_size"].as<glm::vec3>();
            }
            if (component["start_max_particle_size"].IsDefined())
            {
                deserialized_component->start_max_particle_size = component["start_max_particle_size"].as<glm::vec3>();
            }
            if (component["emitter_bounds"].IsDefined())
            {
                deserialized_component->emitter_bounds = component["emitter_bounds"].as<float>();
            }
            if (component["min_spawn_count"].IsDefined())
            {
                deserialized_component->min_spawn_count = component["min_spawn_count"].as<i32>();
            }
            if (component["max_spawn_count"].IsDefined())
            {
                deserialized_component->max_spawn_count = component["max_spawn_count"].as<i32>();
            }
            if (component["start_color_1"].IsDefined())
            {
                deserialized_component->start_color_1 = component["start_color_1"].as<glm::vec4>();
            }
            if (component["end_color_1"].IsDefined())
            {
                deserialized_component->end_color_1 = component["end_color_1"].as<glm::vec4>();
            }
            if (component["lifetime_1"].IsDefined())
            {
                deserialized_component->lifetime_1 = component["lifetime_1"].as<float>();
            }
            if (component["lifetime_2"].IsDefined())
            {
                deserialized_component->lifetime_2 = component["lifetime_2"].as<float>();
            }
            if (component["m_simulate_in_world_space"].IsDefined())
            {
                deserialized_component->m_simulate_in_world_space = component["m_simulate_in_world_space"].as<bool>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "SoundComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = Sound::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class Sound>(get_from_pool(component["guid"].as<std::string>()));
            if (component["path"].IsDefined())
            {
                deserialized_component->path = component["path"].as<std::string>();
            }
            if (component["volume"].IsDefined())
            {
                deserialized_component->volume = component["volume"].as<float>();
            }
            if (component["play_on_awake"].IsDefined())
            {
                deserialized_component->play_on_awake = component["play_on_awake"].as<bool>();
            }
            if (component["is_positional"].IsDefined())
            {
                deserialized_component->is_positional = component["is_positional"].as<bool>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "SoundListenerComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = SoundListener::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class SoundListener>(get_from_pool(component["guid"].as<std::string>()));
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "ClockComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = Clock::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class Clock>(get_from_pool(component["guid"].as<std::string>()));
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "CreditsComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = Credits::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class Credits>(get_from_pool(component["guid"].as<std::string>()));
            if (component["back_to_menu_button"].IsDefined())
            {
                deserialized_component->back_to_menu_button = component["back_to_menu_button"].as<std::weak_ptr<Button>>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "CustomerComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = Customer::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class Customer>(get_from_pool(component["guid"].as<std::string>()));
            if (component["collider"].IsDefined())
            {
                deserialized_component->collider = component["collider"].as<std::weak_ptr<Collider2D>>();
            }
            if (component["left_hand"].IsDefined())
            {
                deserialized_component->left_hand = component["left_hand"].as<std::weak_ptr<Entity>>();
            }
            if (component["right_hand"].IsDefined())
            {
                deserialized_component->right_hand = component["right_hand"].as<std::weak_ptr<Entity>>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "CustomerManagerComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = CustomerManager::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class CustomerManager>(get_from_pool(component["guid"].as<std::string>()));
            if (component["destinations_after_feeding"].IsDefined())
            {
                deserialized_component->destinations_after_feeding = component["destinations_after_feeding"].as<std::vector<std::weak_ptr<Entity>>>();
            }
            if (component["destination_curve"].IsDefined())
            {
                deserialized_component->destination_curve = component["destination_curve"].as<std::weak_ptr<Curve>>();
            }
            if (component["customer_prefab"].IsDefined())
            {
                deserialized_component->customer_prefab = component["customer_prefab"].as<std::string>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "FactoryComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = Factory::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class Factory>(get_from_pool(component["guid"].as<std::string>()));
            if (component["type"].IsDefined())
            {
                deserialized_component->type = component["type"].as<FactoryType>();
            }
            if (component["lights"].IsDefined())
            {
                deserialized_component->lights = component["lights"].as<std::vector<std::weak_ptr<PointLight>>>();
            }
            if (component["factory_light"].IsDefined())
            {
                deserialized_component->factory_light = component["factory_light"].as<std::weak_ptr<PointLight>>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "FieldCellComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = FieldCell::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class FieldCell>(get_from_pool(component["guid"].as<std::string>()));
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "FieldGridComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = FieldGrid::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class FieldGrid>(get_from_pool(component["guid"].as<std::string>()));
            if (component["rows_number"].IsDefined())
            {
                deserialized_component->rows_number = component["rows_number"].as<i32>();
            }
            if (component["punishment_multiplier"].IsDefined())
            {
                deserialized_component->punishment_multiplier = component["punishment_multiplier"].as<float>();
            }
            if (component["punishment_power"].IsDefined())
            {
                deserialized_component->punishment_power = component["punishment_power"].as<float>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "GameControllerComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = GameController::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class GameController>(get_from_pool(component["guid"].as<std::string>()));
            if (component["current_scene"].IsDefined())
            {
                deserialized_component->current_scene = component["current_scene"].as<std::weak_ptr<Entity>>();
            }
            if (component["next_scene"].IsDefined())
            {
                deserialized_component->next_scene = component["next_scene"].as<std::weak_ptr<Entity>>();
            }
            if (component["dialog_manager"].IsDefined())
            {
                deserialized_component->dialog_manager = component["dialog_manager"].as<std::weak_ptr<DialoguePromptController>>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "HovercraftWithoutKeeperComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = HovercraftWithoutKeeper::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class HovercraftWithoutKeeper>(get_from_pool(component["guid"].as<std::string>()));
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "IceBoundComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = IceBound::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class IceBound>(get_from_pool(component["guid"].as<std::string>()));
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "JeepComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = Jeep::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class Jeep>(get_from_pool(component["guid"].as<std::string>()));
            if (component["maximum_speed"].IsDefined())
            {
                deserialized_component->maximum_speed = component["maximum_speed"].as<float>();
            }
            if (component["acceleration"].IsDefined())
            {
                deserialized_component->acceleration = component["acceleration"].as<float>();
            }
            if (component["deceleration"].IsDefined())
            {
                deserialized_component->deceleration = component["deceleration"].as<float>();
            }
            if (component["is_active"].IsDefined())
            {
                deserialized_component->is_active = component["is_active"].as<bool>();
            }
            if (component["player"].IsDefined())
            {
                deserialized_component->player = component["player"].as<std::weak_ptr<Truther>>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "LevelControllerComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = LevelController::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class LevelController>(get_from_pool(component["guid"].as<std::string>()));
            if (component["map_time"].IsDefined())
            {
                deserialized_component->map_time = component["map_time"].as<float>();
            }
            if (component["map_food"].IsDefined())
            {
                deserialized_component->map_food = component["map_food"].as<u32>();
            }
            if (component["maximum_lighthouse_level"].IsDefined())
            {
                deserialized_component->maximum_lighthouse_level = component["maximum_lighthouse_level"].as<i32>();
            }
            if (component["factories"].IsDefined())
            {
                deserialized_component->factories = component["factories"].as<std::vector<std::weak_ptr<Factory>>>();
            }
            if (component["port"].IsDefined())
            {
                deserialized_component->port = component["port"].as<std::weak_ptr<Port>>();
            }
            if (component["lighthouse"].IsDefined())
            {
                deserialized_component->lighthouse = component["lighthouse"].as<std::weak_ptr<Lighthouse>>();
            }
            if (component["customer_manager"].IsDefined())
            {
                deserialized_component->customer_manager = component["customer_manager"].as<std::weak_ptr<CustomerManager>>();
            }
            if (component["playfield_width"].IsDefined())
            {
                deserialized_component->playfield_width = component["playfield_width"].as<float>();
            }
            if (component["playfield_additional_width"].IsDefined())
            {
                deserialized_component->playfield_additional_width = component["playfield_additional_width"].as<float>();
            }
            if (component["playfield_height"].IsDefined())
            {
                deserialized_component->playfield_height = component["playfield_height"].as<float>();
            }
            if (component["playfield_y_shift"].IsDefined())
            {
                deserialized_component->playfield_y_shift = component["playfield_y_shift"].as<float>();
            }
            if (component["ships_limit_curve"].IsDefined())
            {
                deserialized_component->ships_limit_curve = component["ships_limit_curve"].as<std::weak_ptr<Curve>>();
            }
            if (component["ships_limit"].IsDefined())
            {
                deserialized_component->ships_limit = component["ships_limit"].as<u32>();
            }
            if (component["ships_speed_curve"].IsDefined())
            {
                deserialized_component->ships_speed_curve = component["ships_speed_curve"].as<std::weak_ptr<Curve>>();
            }
            if (component["ships_speed"].IsDefined())
            {
                deserialized_component->ships_speed = component["ships_speed"].as<float>();
            }
            if (component["ships_range_curve"].IsDefined())
            {
                deserialized_component->ships_range_curve = component["ships_range_curve"].as<std::weak_ptr<Curve>>();
            }
            if (component["ships_turn_curve"].IsDefined())
            {
                deserialized_component->ships_turn_curve = component["ships_turn_curve"].as<std::weak_ptr<Curve>>();
            }
            if (component["ships_additional_speed_curve"].IsDefined())
            {
                deserialized_component->ships_additional_speed_curve = component["ships_additional_speed_curve"].as<std::weak_ptr<Curve>>();
            }
            if (component["pirates_in_control_curve"].IsDefined())
            {
                deserialized_component->pirates_in_control_curve = component["pirates_in_control_curve"].as<std::weak_ptr<Curve>>();
            }
            if (component["is_tutorial"].IsDefined())
            {
                deserialized_component->is_tutorial = component["is_tutorial"].as<bool>();
            }
            if (component["starting_packages"].IsDefined())
            {
                deserialized_component->starting_packages = component["starting_packages"].as<u32>();
            }
            if (component["tutorial_level"].IsDefined())
            {
                deserialized_component->tutorial_level = component["tutorial_level"].as<u32>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "LighthouseComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = Lighthouse::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class Lighthouse>(get_from_pool(component["guid"].as<std::string>()));
            if (component["light"].IsDefined())
            {
                deserialized_component->light = component["light"].as<std::weak_ptr<LighthouseLight>>();
            }
            if (component["water"].IsDefined())
            {
                deserialized_component->water = component["water"].as<std::weak_ptr<Water>>();
            }
            if (component["spawn_position"].IsDefined())
            {
                deserialized_component->spawn_position = component["spawn_position"].as<std::weak_ptr<Entity>>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "LighthouseKeeperComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = LighthouseKeeper::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class LighthouseKeeper>(get_from_pool(component["guid"].as<std::string>()));
            if (component["maximum_speed"].IsDefined())
            {
                deserialized_component->maximum_speed = component["maximum_speed"].as<float>();
            }
            if (component["acceleration"].IsDefined())
            {
                deserialized_component->acceleration = component["acceleration"].as<float>();
            }
            if (component["deceleration"].IsDefined())
            {
                deserialized_component->deceleration = component["deceleration"].as<float>();
            }
            if (component["lighthouse"].IsDefined())
            {
                deserialized_component->lighthouse = component["lighthouse"].as<std::weak_ptr<Lighthouse>>();
            }
            if (component["port"].IsDefined())
            {
                deserialized_component->port = component["port"].as<std::weak_ptr<Port>>();
            }
            if (component["keeper_dust"].IsDefined())
            {
                deserialized_component->keeper_dust = component["keeper_dust"].as<std::weak_ptr<ParticleSystem>>();
            }
            if (component["keeper_splash"].IsDefined())
            {
                deserialized_component->keeper_splash = component["keeper_splash"].as<std::weak_ptr<ParticleSystem>>();
            }
            if (component["packages"].IsDefined())
            {
                deserialized_component->packages = component["packages"].as<std::vector<std::weak_ptr<Entity>>>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "LighthouseLightComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = LighthouseLight::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class LighthouseLight>(get_from_pool(component["guid"].as<std::string>()));
            if (component["spotlight"].IsDefined())
            {
                deserialized_component->spotlight = component["spotlight"].as<std::weak_ptr<SpotLight>>();
            }
            if (component["spotlight_beam_width"].IsDefined())
            {
                deserialized_component->spotlight_beam_width = component["spotlight_beam_width"].as<float>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "PlayerComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = Player::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class Player>(get_from_pool(component["guid"].as<std::string>()));
            if (component["packages_text"].IsDefined())
            {
                deserialized_component->packages_text = component["packages_text"].as<std::weak_ptr<ScreenText>>();
            }
            if (component["flashes_text"].IsDefined())
            {
                deserialized_component->flashes_text = component["flashes_text"].as<std::weak_ptr<ScreenText>>();
            }
            if (component["level_text"].IsDefined())
            {
                deserialized_component->level_text = component["level_text"].as<std::weak_ptr<ScreenText>>();
            }
            if (component["clock_text"].IsDefined())
            {
                deserialized_component->clock_text = component["clock_text"].as<std::weak_ptr<ScreenText>>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "PopupComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = Popup::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class Popup>(get_from_pool(component["guid"].as<std::string>()));
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "EndScreenComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = EndScreen::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class EndScreen>(get_from_pool(component["guid"].as<std::string>()));
            if (component["is_failed"].IsDefined())
            {
                deserialized_component->is_failed = component["is_failed"].as<bool>();
            }
            if (component["number_of_stars"].IsDefined())
            {
                deserialized_component->number_of_stars = component["number_of_stars"].as<u32>();
            }
            if (component["stars"].IsDefined())
            {
                deserialized_component->stars = component["stars"].as<std::vector<std::weak_ptr<Entity>>>();
            }
            if (component["star_scale"].IsDefined())
            {
                deserialized_component->star_scale = component["star_scale"].as<glm::vec2>();
            }
            if (component["next_level_button"].IsDefined())
            {
                deserialized_component->next_level_button = component["next_level_button"].as<std::weak_ptr<Button>>();
            }
            if (component["restart_button"].IsDefined())
            {
                deserialized_component->restart_button = component["restart_button"].as<std::weak_ptr<Button>>();
            }
            if (component["menu_button"].IsDefined())
            {
                deserialized_component->menu_button = component["menu_button"].as<std::weak_ptr<Button>>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "EndScreenFoliageComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = EndScreenFoliage::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class EndScreenFoliage>(get_from_pool(component["guid"].as<std::string>()));
            if (component["next_level_button"].IsDefined())
            {
                deserialized_component->next_level_button = component["next_level_button"].as<std::weak_ptr<Button>>();
            }
            if (component["percentage"].IsDefined())
            {
                deserialized_component->percentage = component["percentage"].as<float>();
            }
            if (component["percentage_gained"].IsDefined())
            {
                deserialized_component->percentage_gained = component["percentage_gained"].as<float>();
            }
            if (component["percentage_text"].IsDefined())
            {
                deserialized_component->percentage_text = component["percentage_text"].as<std::weak_ptr<ScreenText>>();
            }
            if (component["percentage_bar"].IsDefined())
            {
                deserialized_component->percentage_bar = component["percentage_bar"].as<std::weak_ptr<Panel>>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "PortComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = Port::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class Port>(get_from_pool(component["guid"].as<std::string>()));
            if (component["lights"].IsDefined())
            {
                deserialized_component->lights = component["lights"].as<std::vector<std::weak_ptr<Entity>>>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "ShipComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = Ship::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class Ship>(get_from_pool(component["guid"].as<std::string>()));
            if (component["type"].IsDefined())
            {
                deserialized_component->type = component["type"].as<ShipType>();
            }
            if (component["light"].IsDefined())
            {
                deserialized_component->light = component["light"].as<std::weak_ptr<LighthouseLight>>();
            }
            if (component["spawner"].IsDefined())
            {
                deserialized_component->spawner = component["spawner"].as<std::weak_ptr<ShipSpawner>>();
            }
            if (component["eyes"].IsDefined())
            {
                deserialized_component->eyes = component["eyes"].as<std::weak_ptr<ShipEyes>>();
            }
            if (component["my_light"].IsDefined())
            {
                deserialized_component->my_light = component["my_light"].as<std::weak_ptr<PointLight>>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "ShipEyesComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = ShipEyes::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class ShipEyes>(get_from_pool(component["guid"].as<std::string>()));
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "ShipSpawnerComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = ShipSpawner::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class ShipSpawner>(get_from_pool(component["guid"].as<std::string>()));
            if (component["paths"].IsDefined())
            {
                deserialized_component->paths = component["paths"].as<std::vector<std::weak_ptr<Path>>>();
            }
            if (component["floaters_manager"].IsDefined())
            {
                deserialized_component->floaters_manager = component["floaters_manager"].as<std::weak_ptr<FloatersManager>>();
            }
            if (component["light"].IsDefined())
            {
                deserialized_component->light = component["light"].as<std::weak_ptr<LighthouseLight>>();
            }
            if (component["last_chance_food_threshold"].IsDefined())
            {
                deserialized_component->last_chance_food_threshold = component["last_chance_food_threshold"].as<u32>();
            }
            if (component["last_chance_time_threshold"].IsDefined())
            {
                deserialized_component->last_chance_time_threshold = component["last_chance_time_threshold"].as<float>();
            }
            if (component["main_event_spawn"].IsDefined())
            {
                deserialized_component->main_event_spawn = component["main_event_spawn"].as<std::vector<SpawnEvent>>();
            }
            if (component["backup_spawn"].IsDefined())
            {
                deserialized_component->backup_spawn = component["backup_spawn"].as<std::vector<SpawnEvent>>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "ThanksComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = Thanks::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class Thanks>(get_from_pool(component["guid"].as<std::string>()));
            if (component["back_to_menu_button"].IsDefined())
            {
                deserialized_component->back_to_menu_button = component["back_to_menu_button"].as<std::weak_ptr<Button>>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "TrutherComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = Truther::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class Truther>(get_from_pool(component["guid"].as<std::string>()));
            if (component["truther_bends"].IsDefined())
            {
                deserialized_component->truther_bends = component["truther_bends"].as<bool>();
            }
            if (component["maximum_speed"].IsDefined())
            {
                deserialized_component->maximum_speed = component["maximum_speed"].as<float>();
            }
            if (component["acceleration"].IsDefined())
            {
                deserialized_component->acceleration = component["acceleration"].as<float>();
            }
            if (component["deceleration"].IsDefined())
            {
                deserialized_component->deceleration = component["deceleration"].as<float>();
            }
            if (component["jump_power"].IsDefined())
            {
                deserialized_component->jump_power = component["jump_power"].as<float>();
            }
            if (component["jump_horizontal_power"].IsDefined())
            {
                deserialized_component->jump_horizontal_power = component["jump_horizontal_power"].as<float>();
            }
            if (component["gravitation"].IsDefined())
            {
                deserialized_component->gravitation = component["gravitation"].as<float>();
            }
            if (component["suck_power"].IsDefined())
            {
                deserialized_component->suck_power = component["suck_power"].as<float>();
            }
            if (component["is_sucked"].IsDefined())
            {
                deserialized_component->is_sucked = component["is_sucked"].as<bool>();
            }
            if (component["wheat_overlay"].IsDefined())
            {
                deserialized_component->wheat_overlay = component["wheat_overlay"].as<std::weak_ptr<WheatOverlay>>();
            }
            if (component["model_entity"].IsDefined())
            {
                deserialized_component->model_entity = component["model_entity"].as<std::weak_ptr<Entity>>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "WheatOverlayComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = WheatOverlay::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class WheatOverlay>(get_from_pool(component["guid"].as<std::string>()));
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "PlayerInputComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = PlayerInput::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class PlayerInput>(get_from_pool(component["guid"].as<std::string>()));
            if (component["player_speed"].IsDefined())
            {
                deserialized_component->player_speed = component["player_speed"].as<float>();
            }
            if (component["camera_speed"].IsDefined())
            {
                deserialized_component->camera_speed = component["camera_speed"].as<float>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "CowComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = Cow::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class Cow>(get_from_pool(component["guid"].as<std::string>()));
            if (component["cow_manager"].IsDefined())
            {
                deserialized_component->cow_manager = component["cow_manager"].as<std::weak_ptr<CowManager>>();
            }
            if (component["is_sucked"].IsDefined())
            {
                deserialized_component->is_sucked = component["is_sucked"].as<bool>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "CowManagerComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = CowManager::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class CowManager>(get_from_pool(component["guid"].as<std::string>()));
            if (component["dialogue_prompt_controller"].IsDefined())
            {
                deserialized_component->dialogue_prompt_controller = component["dialogue_prompt_controller"].as<std::weak_ptr<DialoguePromptController>>();
            }
            if (component["paths"].IsDefined())
            {
                deserialized_component->paths = component["paths"].as<std::vector<std::weak_ptr<Path>>>();
            }
            if (component["cows"].IsDefined())
            {
                deserialized_component->cows = component["cows"].as<std::vector<std::weak_ptr<Cow>>>();
            }
            if (component["truther"].IsDefined())
            {
                deserialized_component->truther = component["truther"].as<std::weak_ptr<Truther>>();
            }
            if (component["ufo"].IsDefined())
            {
                deserialized_component->ufo = component["ufo"].as<std::weak_ptr<UFO>>();
            }
            if (component["jeep"].IsDefined())
            {
                deserialized_component->jeep = component["jeep"].as<std::weak_ptr<Jeep>>();
            }
            if (component["wheat_overlay"].IsDefined())
            {
                deserialized_component->wheat_overlay = component["wheat_overlay"].as<std::weak_ptr<WheatOverlay>>();
            }
            if (component["clock_text_ref"].IsDefined())
            {
                deserialized_component->clock_text_ref = component["clock_text_ref"].as<std::weak_ptr<ScreenText>>();
            }
            if (component["friel_grid"].IsDefined())
            {
                deserialized_component->friel_grid = component["friel_grid"].as<std::weak_ptr<FieldGrid>>();
            }
            if (component["event_timer"].IsDefined())
            {
                deserialized_component->event_timer = component["event_timer"].as<float>();
            }
            if (component["does_level_ended"].IsDefined())
            {
                deserialized_component->does_level_ended = component["does_level_ended"].as<bool>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "JeepReflectorComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = JeepReflector::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class JeepReflector>(get_from_pool(component["guid"].as<std::string>()));
            if (component["jeep"].IsDefined())
            {
                deserialized_component->jeep = component["jeep"].as<std::weak_ptr<Jeep>>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "UFOComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = UFO::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class UFO>(get_from_pool(component["guid"].as<std::string>()));
            if (component["field_grid"].IsDefined())
            {
                deserialized_component->field_grid = component["field_grid"].as<std::weak_ptr<FieldGrid>>();
            }
            if (component["truther"].IsDefined())
            {
                deserialized_component->truther = component["truther"].as<std::weak_ptr<Truther>>();
            }
            if (component["attract_bean"].IsDefined())
            {
                deserialized_component->attract_bean = component["attract_bean"].as<std::weak_ptr<SpotLight>>();
            }
            if (component["cow_manager"].IsDefined())
            {
                deserialized_component->cow_manager = component["cow_manager"].as<std::weak_ptr<CowManager>>();
            }
            if (component["particles"].IsDefined())
            {
                deserialized_component->particles = component["particles"].as<std::weak_ptr<ParticleSystem>>();
            }
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    if (component_name == "WheatComponent")
    {
        if (first_pass)
        {
            auto const deserialized_component = Wheat::create();
            deserialized_component->guid = component["guid"].as<std::string>();
            deserialized_component->custom_name = component["custom_name"].as<std::string>();
            deserialized_pool.emplace_back(deserialized_component);
        }
        else
        {
            auto const deserialized_component = std::dynamic_pointer_cast<class Wheat>(get_from_pool(component["guid"].as<std::string>()));
            deserialized_entity->add_component(deserialized_component);
            deserialized_component->reprepare();
        }
    }
        else
    {
        std::cout << "Error. Deserialization of component " << component_name << " failed." << "\n";
    }
    // # Put new deserialization here
}

void SceneSerializer::deserialize_components(YAML::Node const& entity_node, std::shared_ptr<Entity> const& deserialized_entity,
                                             bool const first_pass)
{
    auto const components = entity_node["Components"];

    for (auto it = components.begin(); it != components.end(); ++it)
    {
        YAML::Node const& component = *it;
        auto component_name = component["ComponentName"].as<std::string>();
        if (false)
        {
            // Custom deserialization here
        }
        else
        {
            auto_deserialize_component(component, deserialized_entity, first_pass);
        }
    }
}

std::shared_ptr<Entity> SceneSerializer::deserialize_entity_first_pass(YAML::Node const& entity)
{
    auto const entity_node = entity["guid"];
    if (!entity_node)
    {
        std::cout << "Deserialization of a scene failed. Broken entity. No guid present."
                  << "\n";
        return nullptr;
    }
    auto const guid = entity_node.as<std::string>();

    auto const name_node = entity["Name"];
    if (!name_node)
    {
        std::cout << "Deserialization of a scene failed. Broken entity. No name present."
                  << "\n";
        return nullptr;
    }
    auto const name = name_node.as<std::string>();

    std::shared_ptr<Entity> deserialized_entity = Entity::create(guid, name);
    deserialized_entity->m_is_being_deserialized = true;

    auto const transform = entity["TransformComponent"];
    if (!transform)
    {
        std::cout << "Deserialization of a scene failed. Broken entity. No transform present."
                  << "\n";
        return nullptr;
    }

    deserialized_entity->transform->set_local_position(transform["Translation"].as<glm::vec3>());
    deserialized_entity->transform->set_euler_angles(transform["Rotation"].as<glm::vec3>());
    deserialized_entity->transform->set_local_scale(transform["Scale"].as<glm::vec3>());
    deserialized_entity->m_parent_guid = transform["Parent"]["guid"].as<std::string>();

    deserialize_components(entity, deserialized_entity, true);

    return deserialized_entity;
}

void SceneSerializer::deserialize_entity_second_pass(YAML::Node const& entity, std::shared_ptr<Entity> const& deserialized_entity)
{
    deserialize_components(entity, deserialized_entity, false);

    deserialized_entity->m_is_being_deserialized = false;
}

// Serialize one entity (including its children) to a file.
void SceneSerializer::serialize_this_entity(std::shared_ptr<Entity> const& entity, std::string const& file_path) const
{
    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "Scene" << YAML::Value << "Untitled";
    out << YAML::Key << "Entities";
    out << YAML::Value << YAML::BeginSeq;

    serialize_entity_recursively(out, entity);

    out << YAML::EndSeq;
    out << YAML::EndMap;

    std::filesystem::path const path = file_path;

    if (!path.has_parent_path())
    {
        Debug::log("Path to serialize an entity doesn't have a directory. Aborting.", DebugType::Error);
        return;
    }

    if (!std::filesystem::exists(path.parent_path()))
    {
        create_directory(path.parent_path());
    }

    std::ofstream scene_file(file_path);

    if (!scene_file.is_open())
    {
        std::cout << "Could not serialize an entity to a file: " << file_path << "\n";
        return;
    }

    scene_file << out.c_str();
    scene_file.close();
}

// Deserialize entity (might include its children) from a file.
// Replaces all guids that are not present in the scene with newly generated ones.
std::shared_ptr<Entity> SceneSerializer::deserialize_this_entity(std::string const& file_path)
{
    std::optional<std::string> scene_data = Engine::asset_preloader->get_text_asset(file_path);

    std::stringstream stream;

    if (!scene_data.has_value())
    {
        Debug::log("Preloading failed " + file_path);
        std::ifstream scene_file(file_path);

        if (!scene_file.is_open())
        {
            Debug::log("Could not open a scene file: " + file_path + "\n", DebugType::Error);
            return {};
        }

        stream << scene_file.rdbuf();
        scene_file.close();

        scene_data = stream.str();
    }
    else
    {
        Debug::log("Preloading succ " + file_path);
        stream = std::stringstream(scene_data.value());
    }

    std::unordered_set<std::string> included_guids = {};
    std::string line = {};
    bool next_line_new_guid = false;
    while (std::getline(stream, line))
    {
        size_t component_search = line.find("ComponentName:");
        size_t entity_search = line.find("Entity:");

        if (component_search != std::string::npos || entity_search != std::string::npos)
        {
            next_line_new_guid = true;
            continue;
        }

        if (next_line_new_guid)
        {
            size_t guid_search = line.find("guid:");
            if (guid_search == std::string::npos)
            {
                Debug::log("Copied entity might be corrupted, missing guid after component or entity?", DebugType::Error);
                next_line_new_guid = false;
                continue;
            }

            size_t first_guid_char_offset = guid_search + 6;
            std::string guid = line.substr(first_guid_char_offset);
            included_guids.emplace(guid);

            next_line_new_guid = false;
        }
    }

    stream.clear();
    stream.seekg(0);

    std::stringstream output = {};
    while (std::getline(stream, line))
    {
        size_t guid_search = line.find("guid:");

        if (guid_search == std::string::npos)
        {
            output << line << "\n";
            continue;
        }

        // Replace guid
        size_t first_guid_char_offset = guid_search + 6;
        std::string guid = line.substr(first_guid_char_offset);

        if (guid == "\"\"")
        {
            output << line << "\n";
            continue;
        }

        if (m_replaced_guids_map.contains(guid))
        {
            line.replace(first_guid_char_offset, guid.size(), m_replaced_guids_map.at(guid));
        }
        else if (included_guids.contains(guid))
        {
            std::string new_guid = AK::generate_guid();
            m_replaced_guids_map.emplace(guid, new_guid);
            line.replace(first_guid_char_offset, guid.size(), new_guid);
        }

        output << line << "\n";
    }

    YAML::Node data = YAML::Load(output.str());

    if (!data["Scene"])
        return {};

    DeserializationMode const previous_mode = m_deserialization_mode;
    m_deserialization_mode = DeserializationMode::InjectFromFile;

    auto const scene_name = data["Scene"].as<std::string>();

    std::shared_ptr<Entity> first_entity = {};

    if (auto const entities = data["Entities"])
    {
        std::vector<std::pair<std::shared_ptr<Entity>, YAML::Node>> deserialized_entities = {};
        deserialized_entities.reserve(entities.size());

        // First pass. Create all entities and components.
        for (auto const entity : entities)
        {
            auto const deserialized_entity = deserialize_entity_first_pass(entity);
            if (deserialized_entity == nullptr)
            {
                m_deserialization_mode = previous_mode;
                return {};
            }

            if (first_entity == nullptr)
            {
                first_entity = deserialized_entity;
            }

            deserialized_entities_pool.emplace_back(deserialized_entity);
            deserialized_entities.emplace_back(deserialized_entity, entity);
        }

        // Second pass. Assign components' values including references to other components.
        // Assign appropriate parent for each entity.
        for (auto const& [entity, node] : deserialized_entities)
        {
            deserialize_entity_second_pass(node, entity);

            if (entity->m_parent_guid.empty())
                continue;

            for (auto const& [other_entity, other_node] : deserialized_entities)
            {
                if (entity->m_parent_guid == other_entity->guid)
                {
                    entity->transform->set_parent(other_entity->transform);
                    break;
                }
            }
        }

        if (MainScene::get_instance()->is_running)
        {
            for (auto const& component : deserialized_pool)
            {
                component->awake();
                component->has_been_awaken = true;

                if (component->enabled())
                {
                    component->on_enabled();
                }
            }
        }
    }

    m_deserialization_mode = previous_mode;

    return first_entity;
}

void SceneSerializer::serialize(std::string const& file_path) const
{
    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "Scene" << YAML::Value << "Untitled";
    out << YAML::Key << "Entities";
    out << YAML::Value << YAML::BeginSeq;

    for (auto const& entity : m_scene->entities)
    {
        if (!entity->is_serialized)
        {
            continue;
        }

        serialize_entity(out, entity);
    }

    out << YAML::EndSeq;
    out << YAML::EndMap;

    std::ofstream scene_file(file_path);

    if (!scene_file.is_open())
    {
        std::cout << "Could not create a scene file: " << file_path << "\n";
        return;
    }

    scene_file << out.c_str();
    scene_file.close();
}

bool SceneSerializer::deserialize(std::string const& file_path)
{
    std::optional<std::string> scene_data = Engine::asset_preloader->get_text_asset(file_path);

    if (!scene_data.has_value())
    {
        std::ifstream scene_file(file_path);

        if (!scene_file.is_open())
        {
            std::cout << "Could not open a scene file: " << file_path << "\n";
            return false;
        }

        std::stringstream stream;
        stream << scene_file.rdbuf();
        scene_file.close();

        scene_data = stream.str();
    }

    YAML::Node data = YAML::Load(scene_data.value());

    if (!data["Scene"])
        return false;

    auto const scene_name = data["Scene"].as<std::string>();
    std::cout << "Deserializing scene " << scene_name << "\n";

    if (auto const entities = data["Entities"])
    {
        std::vector<std::pair<std::shared_ptr<Entity>, YAML::Node>> deserialized_entities = {};
        deserialized_entities.reserve(entities.size());

        // First pass. Create all entities and components.
        for (auto const entity : entities)
        {
            auto const deserialized_entity = deserialize_entity_first_pass(entity);
            if (deserialized_entity == nullptr)
                return false;

            deserialized_entities_pool.emplace_back(deserialized_entity);
            deserialized_entities.emplace_back(deserialized_entity, entity);
        }

        // Second pass. Assign components' values including references to other components.
        // Assign appropriate parent for each entity.
        for (auto const& [entity, node] : deserialized_entities)
        {
            deserialize_entity_second_pass(node, entity);

            if (entity->m_parent_guid.empty())
                continue;

            for (auto const& [other_entity, other_node] : deserialized_entities)
            {
                if (entity->m_parent_guid == other_entity->guid)
                {
                    entity->transform->set_parent(other_entity->transform);
                    break;
                }
            }
        }

        if (MainScene::get_instance()->is_running)
        {
            for (auto const& component : deserialized_pool)
            {
                component->awake();
                component->has_been_awaken = true;

                if (component->enabled())
                {
                    component->on_enabled();
                }
            }
        }
    }

    return true;
}

void SceneSerializer::save_prefab(std::shared_ptr<Entity> const& entity, std::string const& prefab_name)
{
    auto const scene_serializer = std::make_shared<SceneSerializer>(MainScene::get_instance());
    scene_serializer->set_instance(scene_serializer);
    ScopeGuard unset_instance = [&] { scene_serializer->set_instance(nullptr); };

    scene_serializer->serialize_this_entity(entity, m_prefab_path + prefab_name + ".txt");
}

// FIXME: We should probably cache entities that are prefabs and are referenced in the scene, just like Unity.
//        So we won't need to read the .txt files here.
std::shared_ptr<Entity> SceneSerializer::load_prefab(std::string const& prefab_name)
{
    auto const scene_serializer = std::make_shared<SceneSerializer>(MainScene::get_instance());
    scene_serializer->set_instance(scene_serializer);
    ScopeGuard unset_instance = [&] { scene_serializer->set_instance(nullptr); };

    std::shared_ptr<Entity> entity = scene_serializer->deserialize_this_entity(m_prefab_path + prefab_name + ".txt");

    return entity;
}
