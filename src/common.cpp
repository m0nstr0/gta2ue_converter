#include "common.h"

using namespace gta_to_ue;

Vector3f::Vector3f(float in_x, float in_y, float in_z) : x(in_x), y(in_y), z(in_z)
{}

Vector2f::Vector2f(float in_x, float in_y) : x(in_x), y(in_y)
{}

Triangle::Triangle(float in_vertex1, float in_vertex2, float in_vertex3, int32_t in_material_id) :
    vertex1(in_vertex1), vertex2(in_vertex2), vertex3(in_vertex3), material_id(in_material_id)
{}

Color::Color(uint8_t in_r, uint8_t in_g, uint8_t in_b, uint8_t in_a): r(in_r), g(in_g), b(in_b), a(in_a)
{}

Color::Color(const Color& in_color) = default;

std::string Color::to_string() const
{
    std::ostringstream s;
    s << "color" << r << g << b << a;
    return s.str();
}

Material::Material(std::string in_material_name, std::string in_diffuse_texture, std::string in_mask_texture,
                   const Color& in_color, size_t in_hash, int32_t in_index):
    material_name(std::move(in_material_name)), diffuse_texture(std::move(in_diffuse_texture)),
    mask_texture(std::move(in_mask_texture)), color(in_color), hash(in_hash), index(in_index)
{}

MorphTarget::MorphTarget(int32_t num_vertices_to_reserve)
{
    vertices.reserve(num_vertices_to_reserve);
    normals.reserve(num_vertices_to_reserve);
}

Geometry::Geometry(int32_t num_triangles_to_reserve, int32_t num_tex_coordinates_sets_to_reserve, int32_t num_morph_targets_to_reserve, int32_t in_num_vertices, int32_t in_num_materials) :
    num_vertices(in_num_vertices), num_materials(in_num_materials)
{
    triangles.reserve(num_triangles_to_reserve);
    tex_coordinate_sets.reserve(num_tex_coordinates_sets_to_reserve);
    morph_targets.reserve(num_morph_targets_to_reserve);
}