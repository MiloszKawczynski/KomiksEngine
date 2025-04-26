#include "Quad.h"

#include "AK/AK.h"
#include "AK/Math.h"
#include "Camera.h"
#include "Entity.h"
#include "Game/CustomerManager.h"
#include "Game/GameController.h"
#include "Globals.h"
#include "RendererDX11.h"
#include "ResourceManager.h"

#include <glm/gtc/type_ptr.inl>

#if EDITOR
#include "imgui_stdlib.h"
#endif

std::shared_ptr<Quad> Quad::create()
{
    auto const particle_shader = ResourceManager::get_instance().load_shader("./res/shaders/particle.hlsl", "./res/shaders/particle.hlsl");
    auto const particle_material = Material::create(particle_shader, 1000, false, false, true);
    particle_material->casts_shadows = false;
    particle_material->needs_forward_rendering = true;

    auto quad = std::make_shared<Quad>(AK::Badge<Quad> {}, "./res/textures/particle.png", particle_material);

    quad->prepare();

    return quad;
}

Quad::Quad(AK::Badge<Quad>, std::string const& sprite_path, std::shared_ptr<Material> const& mat) : Drawable(mat), path(sprite_path)
{
}

void Quad::awake()
{
    set_can_tick(true);
}

void Quad::draw() const
{
    if (m_rasterizer_draw_type == RasterizerDrawType::None)
    {
        return;
    }

    // Either wireframe or solid for individual model
    Renderer::get_instance()->set_rasterizer_draw_type(m_rasterizer_draw_type);

    if (m_mesh != nullptr)
    {
        m_mesh->draw();
    }

    Renderer::get_instance()->restore_default_rasterizer_draw_type();
}

#if EDITOR
void Quad::draw_editor()
{
    Drawable::draw_editor();

    ImGui::ColorEdit4("Color", glm::value_ptr(m_color));
    ImGui::InputText("Sprite Path", &path);
    if (ImGui::IsItemDeactivatedAfterEdit())
    {
        reprepare();
    }

    update_quad();
}
#endif

void Quad::reprepare()
{
    Drawable::reprepare();

    prepare();
}

void Quad::update_quad() const
{
    material->color = m_color;
}

void Quad::update()
{
    update_quad();
}

bool Quad::is_particle() const
{
    return true;
}

void Quad::prepare()
{
    m_mesh = create_sprite();
}

std::shared_ptr<Mesh> Quad::create_sprite() const
{
    std::vector<Vertex> const vertices = {
        {glm::vec3(-1.0f, -1.0f, 0.0f), {}, {0.0f, 0.0f}}, // bottom left
        {glm::vec3(1.0f, -1.0f, 0.0f), {}, {1.0f, 0.0f}}, // bottom right
        {glm::vec3(1.0f, 1.0f, 0.0f), {}, {1.0f, 1.0f}}, // top right
        {glm::vec3(-1.0f, 1.0f, 0.0f), {}, {0.0f, 1.0f}}, // top left
    };

    std::vector<u32> const indices = {0, 1, 2, 0, 2, 3};

    std::vector<std::shared_ptr<Texture>> textures;

    std::vector<std::shared_ptr<Texture>> diffuse_maps = {};
    TextureSettings texture_settings = {};
    texture_settings.wrap_mode_x = TextureWrapMode::ClampToEdge;
    texture_settings.wrap_mode_y = TextureWrapMode::ClampToEdge;

    if (!path.empty())
        diffuse_maps.emplace_back(ResourceManager::get_instance().load_texture(path, TextureType::Diffuse, texture_settings));

    textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());

    return ResourceManager::get_instance().load_mesh(0, path, vertices, indices, textures, DrawType::Triangles, material);
}
