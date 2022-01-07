#pragma once

#include <string>
#include <vector>
#include <rw.h>
#include <rwgta.h>
#include <sstream>

namespace gta_to_ue {

    struct Vector2f
    {
        float x;
        float y;

        Vector2f(float in_x, float in_y);
    };

    using TexCoordinateSet = std::vector<Vector2f>;

    struct Vector3f
    {
        float x;
        float y;
        float z;

        Vector3f(float in_x, float in_y, float in_z);
    };

    struct Triangle
    {
        float vertex1;
        float vertex2;
        float vertex3;
        int32_t material_id;

        Triangle(float in_vertex1, float in_vertex2, float in_vertex3, int32_t in_material_id);
    };

    struct Color
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;

        Color(uint8_t in_r, uint8_t in_g, uint8_t in_b, uint8_t in_a);
        Color(const Color& in_color);
        std::string to_string() const;
    };

    struct Material
    {
        std::string material_name;
        std::string diffuse_texture;
        std::string mask_texture;
        Color color;
        size_t hash;
        int32_t index;

        Material(std::string in_material_name, std::string in_diffuse_texture, std::string in_mask_texture, const Color& in_color, size_t in_hash, int32_t in_index);
    };

    struct MorphTarget
    {
        std::vector<Vector3f> vertices;
        std::vector<Vector3f> normals;

        MorphTarget(int32_t num_vertices_to_reserve = 0);
    };

    struct Geometry
    {
        std::vector<Triangle> triangles;
        std::vector<TexCoordinateSet> tex_coordinate_sets;
        std::vector<MorphTarget> morph_targets;
        int32_t num_vertices;
        int32_t num_materials;

        Geometry(int32_t num_triangles_to_reserve, int32_t num_tex_coordinates_sets_to_reserve, int32_t num_morph_targets_to_reserve, int32_t in_num_vertices, int32_t in_num_materials);
    };

    class MaterialArray: public std::vector<Material>
    {
    public:
        bool has_material_with_hash(size_t hash)
        {
            auto cmp_lambda = [hash](const Material& material) { return material.hash == hash; };

            if (const auto result = std::ranges::find_if(*this, cmp_lambda); result == std::end(*this)) {
                return false;
            }

            return true;
        }

        int32_t get_material_id_with_hash(size_t hash)
        {
            auto cmp_lambda = [hash](const gta_to_ue::Material& material) { return material.hash == hash; };
            if (const auto result = std::ranges::find_if(*this, cmp_lambda); result != std::end(*this)) {
                return result->index;
            }

            return 0;
        }
    };

    struct Mesh
    {
        std::vector<Geometry> geometries;
        MaterialArray materials;
    };
}
