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
        bool has_material_with_hash(size_t hash);

        int32_t get_material_id_with_hash(size_t hash);
    };

    struct Frame
    {
        Vector3f x_axis;
        Vector3f y_axis;
        Vector3f z_axis;
        std::string name;
        Vector3f pos;
        int32_t parent_frame_id;

        Frame(const Vector3f& in_x_axis, const Vector3f& in_y_axis, const Vector3f& in_z_axis, const Vector3f& in_pos, int32_t in_parent_frame_id, std::string in_name);
    };

    struct Atomic
    {
        int32_t geometry_id;
        int32_t frame_id;

        Atomic(int32_t in_geometry_id, int32_t in_frame_id);
    };

    struct AnimHierarchyNode
    {
        int32_t id;
        int32_t index;
        int32_t flags;

        AnimHierarchyNode(int32_t in_id, int32_t in_index, int32_t in_flags);
    };

    struct AnimHierarchy
    {
        int32_t frame_id;
        int32_t id;
        int32_t flags;
        int32_t interp_key_size;

        std::vector<AnimHierarchyNode> nodes;

        AnimHierarchy(int32_t in_frame_id, int32_t in_id, int32_t in_flags, int32_t in_interp_key_size, std::vector<AnimHierarchyNode>& in_nodes);
    };


    struct SkinBoneTransform
    {
        Vector3f x_axis;
        Vector3f y_axis;
        Vector3f z_axis;
        Vector3f pos;

        SkinBoneTransform(Vector3f in_x_axis, Vector3f in_y_axis, Vector3f in_z_axis, Vector3f in_pos):
            x_axis(in_x_axis), y_axis(in_y_axis), z_axis(in_z_axis), pos(in_pos) {}
    };

    struct SkinBoneIndex
    {
        int32_t vertex_id;
        uint8_t bone1;
        uint8_t bone2;
        uint8_t bone3;
        uint8_t bone4;

        SkinBoneIndex(int32_t in_vertex_id, uint8_t in_bone1, uint8_t in_bone2, uint8_t in_bone3, uint8_t in_bone4);
    };

    struct SkinVertexWeight
    {
        int32_t vertex_id;
        float weight1;
        float weight2;
        float weight3;
        float weight4;

        SkinVertexWeight(int32_t in_vertex_id, float in_weight1, float in_weight2, float in_weight3, float in_weight4);
    };

    struct Skin
    {
        int32_t geometry_id;
        int32_t num_bones;
        int32_t num_used_bones;
        int32_t max_weights_per_vertex;
        std::vector<uint8_t> bone_ids;
        std::vector<SkinBoneIndex> bone_indices;
        std::vector<SkinVertexWeight> weights;
        std::vector<SkinBoneTransform> inverse_matrices;
        Skin(int32_t in_geometry_id, int32_t in_num_bones, int32_t in_num_used_bones, int32_t in_max_weights_per_vertex, std::vector<uint8_t> in_bone_ids, std::vector<SkinBoneIndex> in_bone_indices, std::vector<SkinVertexWeight> in_weights, std::vector<SkinBoneTransform> in_inverse_matrices);
    };

    struct Mesh
    {
        std::vector<Geometry> geometries;
        std::vector<Atomic> atomics;
        std::vector<AnimHierarchy> anim_hierarchy;
        std::vector<Skin> skins;
        MaterialArray materials;
        std::vector<Frame> frames;
    };
}