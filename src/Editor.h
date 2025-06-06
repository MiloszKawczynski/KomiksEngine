#pragma once

#include "AK/Badge.h"
#include "AK/Types.h"
#include "Scene.h"
#include "Transform.h"

#include <array>

class DebugDrawing;
class Camera;

namespace Editor
{

enum class GuizmoOperationType
{
    Translate,
    Rotate,
    Scale,
    None
};

enum class EditorWindowType
{
    Custom,
    Debug,
    Content,
    Game,
    Inspector,
    Hierarchy,
};

struct LockData
{
    std::weak_ptr<Entity> selected_entity;
};

class EditorWindow
{
public:
    EditorWindow(i32& last_id, i32 const flags, EditorWindowType const type) : type(type)
    {
        m_id = last_id + 1;
        last_id = m_id;

        this->flags |= flags;

        switch (type)
        {
        case EditorWindowType::Debug:
            m_name = "Debug##" + std::to_string(m_id);
            break;
        case EditorWindowType::Content:
            m_name = "Content##" + std::to_string(m_id);
            break;
        case EditorWindowType::Hierarchy:
            m_name = "Hierarchy##" + std::to_string(m_id);
            break;
        case EditorWindowType::Game:
            m_name = "Game##" + std::to_string(m_id);
            break;
        case EditorWindowType::Inspector:
            m_name = "Inspector##" + std::to_string(m_id);
            break;
        case EditorWindowType::Custom:
            m_name = "Custom##" + std::to_string(m_id);
            break;
        }
    }

    i32 flags = 0;
    EditorWindowType type = EditorWindowType::Custom;

    [[nodiscard]] i32 get_id() const
    {
        return m_id;
    }

    void set_name(std::string const& name)
    {
        m_name = name + "##" + std::to_string(m_id);
    }

    [[nodiscard]] std::string get_name() const
    {
        return m_name;
    }

    void set_is_locked(bool const locked, LockData const& data)
    {
        m_is_locked = locked;

        if (locked)
        {
            m_selected_entity = data.selected_entity;
        }
        else
        {
            m_selected_entity.reset();
        }
    }

    [[nodiscard]] bool is_locked() const
    {
        return m_is_locked;
    }

    [[nodiscard]] std::weak_ptr<Entity> get_locked_entity() const
    {
        return m_selected_entity;
    }

private:
    i32 m_id = 0;
    std::string m_name = {};
    bool m_is_locked = false;

    std::weak_ptr<Entity> m_selected_entity = {};
};

enum class AssetType
{
    Unknown,
    Model,
    Scene,
    Prefab,
    Texture,
    Audio
};

class Asset
{
public:
    Asset(std::string const& path, AssetType const type) : path(path), type(type)
    {
    }

    std::string path;
    AssetType type;
};

class Editor
{
#if EDITOR
public:
    static std::shared_ptr<Editor> create();

    explicit Editor(AK::Badge<Editor>);
    ~Editor();

    void draw();
    void set_scene(std::shared_ptr<Scene> const& scene);
    void handle_input();
    void set_docking_space() const;
    bool load_scene() const;
    void save_scene() const;
    bool load_scene_name(std::string const& name) const;
    void save_scene_as(std::string const& name) const;
    glm::vec2 get_game_size() const;
    glm::vec2 get_game_position() const;
    bool is_rendering_to_editor() const;

    void register_debug_drawing(std::shared_ptr<DebugDrawing> const& debug_drawing);
    void unregister_debug_drawing(std::shared_ptr<DebugDrawing> const& debug_drawing);

    [[nodiscard]] bool are_debug_drawings_enabled() const;

    static std::shared_ptr<Editor> get_instance()
    {
        return m_instance;
    }

private:
    void switch_rendering_to_editor();

    void add_debug_window();
    void add_content_browser();
    void add_game();
    void add_inspector();
    void add_scene_hierarchy();
    void remove_window(std::shared_ptr<EditorWindow> const& window);

    void draw_debug_window(std::shared_ptr<EditorWindow> const& window);
    void draw_content_browser(std::shared_ptr<EditorWindow> const& window);
    void draw_game(std::shared_ptr<EditorWindow> const& window);
    void draw_inspector(std::shared_ptr<EditorWindow> const& window);
    void draw_scene_hierarchy(std::shared_ptr<EditorWindow> const& window);
    void draw_scene_save();

    void draw_entity_recursively(std::shared_ptr<Transform> const& transform);
    static void entity_drag(std::shared_ptr<Entity> const& entity);
    bool draw_entity_popup(std::shared_ptr<Entity> const& entity);

    void draw_window_menu_bar(std::shared_ptr<EditorWindow> const& window);

    void load_assets();
    void set_style() const;

    void camera_input() const;
    void non_camera_input();

    void reset_camera();

    void switch_gizmo_snapping();

    void delete_selected_entity() const;
    void copy_selected_entity() const;
    void paste_entity() const;
    void add_child_entity() const;
    void save_entity_as_prefab();
    [[nodiscard]] bool load_prefab(std::string const& name) const;

    void mouse_callback(double const x, double const y);

    glm::vec3 update_locked_value(glm::vec3 new_value, glm::vec3 const old_value) const;

    std::vector<std::shared_ptr<DebugDrawing>> m_debug_drawings = {};
    bool m_debug_drawings_enabled = true;

    std::string m_copied_entity_path = "./.editor/copied_entity.txt";

    glm::dvec2 m_last_mouse_position = glm::dvec2(1280.0 / 2.0, 720.0 / 2.0);
    float m_yaw = 0.0f;
    float m_pitch = 10.0f;
    bool m_mouse_just_entered = true;
    double m_sensitivity = 0.1;
    float m_camera_speed = 12.5f;

    glm::vec3 const m_camera_default_position = {0.0f, 17.0f, 14.65f};
    glm::vec3 const m_camera_default_rotation = {-50.0f, 0.0f, 0.0f};
    float const m_camera_default_fov = 23.48f;

    std::shared_ptr<Entity> m_camera_entity;
    std::shared_ptr<Camera> m_editor_camera;
    std::shared_ptr<Camera> m_scene_camera;

    std::vector<Asset> m_assets = {};

    glm::vec2 m_game_position = {};
    glm::vec2 m_game_size = {};

    std::weak_ptr<Entity> m_selected_entity;
    std::shared_ptr<Scene> m_open_scene;

    std::vector<std::shared_ptr<EditorWindow>> m_editor_windows = {};
    i32 m_last_window_id = 0;

    bool m_polygon_mode_active = false;
    bool m_always_newest_logs = false;
    i64 m_frame_count = 0;
    double m_current_time = 0.0;
    double m_last_second = 0.0;
    double m_average_ms_per_frame = 0.0;

    bool m_rendering_to_editor = true;
    bool m_gizmo_snapping = false;

    bool m_lock_scale = false;
    glm::vec3 m_lock_ratio = {};
    glm::bvec3 m_disabled_scale = {};

    GuizmoOperationType m_operation_type = GuizmoOperationType::Translate;

    glm::vec3 const m_scale_snap = {0.1f, 0.1f, 0.1f};
    glm::vec3 const m_rotation_snap = {1.0f, 1.0f, 1.0f};
    glm::vec3 const m_position_snap = {0.1f, 0.1f, 0.1f};

    std::string m_content_path = "./res/";
    std::string m_scene_path = "./res/scenes/";
    std::string m_prefab_path = "./res/prefabs/";
    std::string m_textures_path = "./res/textures/UI";
    std::string m_audio_path = "./res/audio/";
    std::array<std::string, 3> m_known_model_formats = {".gltf", ".obj", ".fbx"};
    std::array<std::string, 3> m_known_audio_formats = {".wav"};
    std::array<std::string, 1> m_known_scene_formats = {".txt"};
    std::array<std::string, 1> m_known_textures_formats = {".png"};

    std::string m_search_filter = {};
    std::string m_content_search_filter = {};
    bool m_is_camera_options_locked = false;

    bool m_append_scene = false;

    inline static std::shared_ptr<Editor> m_instance = nullptr;

#endif
};

}
