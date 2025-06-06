#pragma once

#include "ConstantBufferTypes.h"
#include "DirectionalLight.h"
#include "Drawable.h"
#include "EngineDefines.h"
#include "Font.h"
#include "Light.h"
#include "Mesh.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Texture.h"
#include "Vertex.h"

#include <set>

#include <glm/mat4x4.hpp>

#if EDITOR
#include <imgui.h>
#endif

class Camera;

class Renderer
{
public:
    Renderer(Renderer const&) = delete;
    void operator=(Renderer const&) = delete;

    void initialize();
    void uninitialize();

    void register_shader(std::shared_ptr<Shader> const& shader);
    void unregister_shader(std::shared_ptr<Shader> const& shader);

    bool is_drawable_registered(std::shared_ptr<Drawable> const& drawable) const;
    void register_drawable(std::shared_ptr<Drawable> const& drawable);
    void unregister_drawable(std::shared_ptr<Drawable> const& drawable);

    void register_material(std::shared_ptr<Material> const& material);
    void unregister_material(std::shared_ptr<Material> const& material);

    bool is_light_registered(std::shared_ptr<Light> const& light) const;
    void register_light(std::shared_ptr<Light> const& light);
    void unregister_light(std::shared_ptr<Light> const& light);

    void register_camera(std::shared_ptr<Camera> const& camera);
    void unregister_camera(std::shared_ptr<Camera> const& camera);

    void choose_main_camera(std::shared_ptr<Camera> const& exclude = nullptr) const;

    void virtual begin_frame() const;
    void render() const;
    void virtual end_frame() const;
    void virtual present() const;

    void switch_rendering_to_texture();
    void set_rendering_to_texture(bool const render_to_texture);
    void reload_shaders() const;

    void set_vsync(bool const enabled);

    virtual void set_rasterizer_draw_type(RasterizerDrawType const rasterizer_draw_type) = 0;
    virtual void restore_default_rasterizer_draw_type() = 0;

    static std::shared_ptr<Renderer> get_instance()
    {
        return m_instance;
    }

    enum class RendererApi
    {
        OpenGL,
        DirectX11,
    };

    inline static RendererApi renderer_api = RendererApi::DirectX11;

    bool wireframe_mode_active = false;

#if EDITOR
    inline static ImVec4 clear_color = ImVec4(0.2f, 0.2f, 0.2f, 1.00f);
#endif

    inline static glm::vec4 clear_color_glm = {0.2f, 0.2f, 0.2f, 1.0f};

    inline static i32 screen_width = 1280;
    inline static i32 screen_height = 720;

    inline static constexpr i32 transparent_render_order = 1000;
    inline static constexpr i32 aa_render_order = 2000;
    inline static constexpr i32 ui_render_order = 3000;

    inline static std::vector<Font> loaded_fonts = {};

protected:
    Renderer() = default;
    virtual ~Renderer() = default;

    static void set_instance(std::shared_ptr<Renderer> const& renderer)
    {
        m_instance = renderer;
    }

    void virtual update_shader(std::shared_ptr<Shader> const& shader, glm::mat4 const& projection_view,
                               glm::mat4 const& projection_view_no_translation) const = 0;
    void virtual update_material(std::shared_ptr<Material> const& material) const = 0;
    void virtual update_object(std::shared_ptr<Drawable> const& drawable, std::shared_ptr<Material> const& material,
                               glm::mat4 const& projection_view) const = 0;

    void virtual unbind_material(std::shared_ptr<Material> const& material) const = 0;

    void virtual initialize_global_renderer_settings() = 0;
    void virtual initialize_buffers(size_t const max_size) = 0;
    void virtual perform_frustum_culling(std::shared_ptr<Material> const& material) const = 0;
    virtual void render_shadow_maps() const = 0;
    void render_single_shadow_map(glm::mat4 const& projection_view) const;

    virtual void render_lighting_pass() const;
    virtual void render_geometry_pass(glm::mat4 const& projection_view) const;
    virtual void render_forward_pass(glm::mat4 const& projection_view, glm::mat4 const& projection_view_no_translation) const;
    virtual void render_ssao() const;
    virtual void render_aa() const;
    virtual void render_custom_render_order_before_aa(glm::mat4 const& projection_view,
                                                      glm::mat4 const& projection_view_no_translation) const;
    virtual void render_custom_render_order_after_aa(glm::mat4 const& projection_view,
                                                     glm::mat4 const& projection_view_no_translation) const;

    virtual void bind_universal_resources() const;
    virtual void bind_for_render_frame() const;

    inline static std::shared_ptr<Renderer> m_instance;

    bool vsync_enabled = false;
    bool m_render_to_texture = true;

    inline static std::vector<std::shared_ptr<PointLight>> m_point_lights = {};
    inline static std::vector<std::shared_ptr<SpotLight>> m_spot_lights = {};
    inline static std::shared_ptr<DirectionalLight> m_directional_light = {};

    // TODO: Retrieve this information from the shader
    // NOTE: This has to be the same value as the variable in a shader to work in all cases.
    i32 m_max_point_lights = 4;
    i32 m_max_spot_lights = 4;

    void draw(std::shared_ptr<Material> const& material, glm::mat4 const& projection_view) const;
    void draw_instanced(std::shared_ptr<Material> const& material, glm::mat4 const& projection_view,
                        glm::mat4 const& projection_view_no_translation) const;

    std::vector<std::shared_ptr<Shader>> m_shaders = {};
    std::vector<std::shared_ptr<Light>> m_lights = {};
    std::vector<std::shared_ptr<Material>> m_instanced_materials = {};
    std::shared_ptr<Shader> m_shadow_shader = nullptr;
    std::shared_ptr<Shader> m_point_shadow_shader = nullptr;
    std::shared_ptr<Shader> m_blur_shader = nullptr;
    std::shared_ptr<Shader> m_lighting_pass_shader = nullptr;
    std::shared_ptr<Shader> m_fxaa_shader = nullptr;

private:
    void draw_transparent(glm::mat4 const& projection_view, glm::mat4 const& projection_view_no_translation) const;
    static void load_fonts();
    static void unload_fonts();

    struct MaterialWithOrder
    {
        i32 render_order;
        std::shared_ptr<Material> material;

        bool operator<(MaterialWithOrder const& b) const
        {
            return render_order < b.render_order;
        }
    };

    std::multiset<MaterialWithOrder> m_custom_render_order_materials_before_aa = {};
    std::multiset<MaterialWithOrder> m_custom_render_order_materials_after_aa = {};
    std::vector<std::shared_ptr<Material>> m_transparent_materials = {};

    std::vector<std::shared_ptr<Camera>> m_cameras = {};

    inline static std::string m_font_path = "./res/fonts/";
};
