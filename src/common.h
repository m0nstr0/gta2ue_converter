#pragma once

#include <string>
#include <vector>
#include <rw.h>
#include <rwgta.h>
#include <sstream>

struct ConvertingOptions
{
	bool is_car{ false };
    std::string wheels_dff{ "" };
    float wheel_scale{ 1.f };
    int32_t wheel_id{ 237 };
};

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

        Vector3f operator + (const Vector3f& in_vector) const
		{
			return Vector3f(x + in_vector.x, y + (-in_vector.y), z + in_vector.z);
		}
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

    struct BoneTransform
    {
        Vector3f x_axis;
        Vector3f y_axis;
        Vector3f z_axis;
        Vector3f pos;

        BoneTransform(Vector3f in_x_axis, Vector3f in_y_axis, Vector3f in_z_axis, Vector3f in_pos):
            x_axis(in_x_axis), y_axis(in_y_axis), z_axis(in_z_axis), pos(in_pos) {}
    };

    struct BoneHierarchy
    {
        int32_t frame_id;
        int32_t parent_id;
        int32_t max_frame_size;
        BoneHierarchy(int32_t in_frame_id = -1, int32_t in_parent_id = -1) : frame_id(in_frame_id), parent_id(in_parent_id), max_frame_size(0) {}
    };

    struct BoneIndex
    {
        uint8_t bone1;
        uint8_t bone2;
        uint8_t bone3;
        uint8_t bone4;
    };

    struct VertexWeight
    {
        float weight1;
        float weight2;
        float weight3;
        float weight4;
    };

    struct Skeleton
    {
        int32_t num_bones;
        int32_t num_used_bones;
        std::vector<uint8_t> bone_ids;
        std::vector<BoneIndex> bone_indices;
        std::vector<VertexWeight> weights;
        std::vector<BoneTransform> inverse_matrices;
    };

    struct Geometry
    {
        MaterialArray materials;
        std::vector<Triangle> triangles;
        std::vector<TexCoordinateSet> tex_coordinate_sets;
        std::vector<Vector3f> vertices;
        std::vector<Vector3f> normals;
        bool has_skeleton;
        int32_t frame_id;
        Skeleton skeleton;

        Geometry(int32_t num_triangles_to_reserve, int32_t num_tex_coordinates_sets_to_reserve, int32_t in_num_vertices, int32_t in_num_materials, int32_t in_frame_id);
    };

    struct Mesh
    {
        bool has_skeleton {false};
        bool same_skeleton {false};
        std::vector<Geometry> geometries;
        MaterialArray materials;
        std::vector<BoneHierarchy> bone_hierarchy;
        std::vector<Frame> frames;

        Mesh();
    };
}