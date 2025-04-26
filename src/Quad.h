#pragma once

#include "Drawable.h"
#include "GBuffer.h"
#include "ParticleSystem.h"

class Mesh;

class Quad final : public Drawable
{
public:
    static std::shared_ptr<Quad> create();

    explicit Quad(AK::Badge<Quad>, std::string const& sprite_path, std::shared_ptr<Material> const& mat);

    virtual void awake() override;
    virtual void update() override;
    virtual bool is_particle() const override;
    virtual void draw() const override;

#if EDITOR
    virtual void draw_editor() override;
#endif

    virtual void reprepare() override;
    void prepare();

    NON_SERIALIZED
    bool rotate = true;

    NON_SERIALIZED
    ParticleType particle_type = ParticleType::Default;

    std::string path = "./res/textures/particle.png";

private:
    [[nodiscard]] std::shared_ptr<Mesh> create_sprite() const;

    void update_quad() const;

    std::shared_ptr<Material> m_particle_material = {};

    ID3D11Buffer* m_constant_buffer_particle = nullptr;

    std::shared_ptr<Mesh> m_mesh = {};

    glm::vec4 m_color = {1.0f, 1.0f, 1.0f, 1.0f};
};
