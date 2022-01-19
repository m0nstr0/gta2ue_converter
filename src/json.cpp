#include "json.h"
#include <fstream>
#include <string>
#include <rapidjson/writer.h>

using JsonWriter = rapidjson::Writer<rapidjson::StringBuffer>;

void export_object_info(JsonWriter& writer, const gta_to_ue::Mesh& mesh_data)
{
    writer.Key("Info");
    writer.StartObject();
    writer.Key("Version");
    writer.Int(1);
    writer.Key("HasSkeleton");
    writer.Bool(mesh_data.has_skeleton);
	writer.Key("SameSkeleton");
	writer.Bool(mesh_data.same_skeleton);
    writer.EndObject();
}

void export_object_anim_hierarchies(JsonWriter& writer, const gta_to_ue::Mesh& mesh_data)
{
    writer.Key("BoneHierarchy");
    writer.StartArray();
    for (auto& anim_hierarchy : mesh_data.bone_hierarchy) {
        writer.StartObject();
        writer.Key("FrameID");
        writer.Int(anim_hierarchy.frame_id);
        writer.Key("MaxFrameSize");
        writer.Int(anim_hierarchy.max_frame_size);
        writer.Key("ParentID");
        writer.Int(anim_hierarchy.parent_id);
        writer.EndObject();
    }
    writer.EndArray();
}

void export_object_frames(JsonWriter& writer, const gta_to_ue::Mesh& mesh_data)
{
    writer.Key("Frames");
    writer.StartArray();
    for (auto& frame : mesh_data.frames) {
        writer.StartObject();
        writer.Key("Name");
        writer.String(frame.name.c_str());
        writer.Key("ParentID");
        writer.Int(frame.parent_frame_id);
        writer.Key("Transform");
        writer.StartObject();
        writer.Key("AxisX");
        writer.StartObject();
        writer.Key("X");
        writer.Double(frame.x_axis.x);
        writer.Key("Y");
        writer.Double(frame.x_axis.y);
        writer.Key("Z");
        writer.Double(frame.x_axis.z);
        writer.EndObject();
        writer.Key("AxisY");
        writer.StartObject();
        writer.Key("X");
        writer.Double(frame.y_axis.x);
        writer.Key("Y");
        writer.Double(frame.y_axis.y);
        writer.Key("Z");
        writer.Double(frame.y_axis.z);
        writer.EndObject();
        writer.Key("AxisZ");
        writer.StartObject();
        writer.Key("X");
        writer.Double(frame.z_axis.x);
        writer.Key("Y");
        writer.Double(frame.z_axis.y);
        writer.Key("Z");
        writer.Double(frame.z_axis.z);
        writer.EndObject();
        writer.Key("Position");
        writer.StartObject();
        writer.Key("X");
        writer.Double(frame.pos.x);
        writer.Key("Y");
        writer.Double(frame.pos.y);
        writer.Key("Z");
        writer.Double(frame.pos.z);
        writer.EndObject();
        writer.EndObject();
        writer.EndObject();
    }
    writer.EndArray();
}

void export_object_materials(JsonWriter& writer, const gta_to_ue::Mesh& mesh_data)
{
    writer.Key("Materials");
    writer.StartArray();
    for (auto& material : mesh_data.materials) {
        writer.StartObject();
        writer.Key("ID");
        writer.Int(material.index);
        writer.Key("Name");
        writer.String(material.material_name.c_str());
        writer.Key("DiffuseTexture");
        writer.String(material.diffuse_texture.c_str());
        writer.Key("MaskTexture");
        writer.String(material.mask_texture.c_str());
        writer.Key("Color");
        writer.StartObject();
        writer.Key("R");
        writer.Int(material.color.r);
        writer.Key("G");
        writer.Int(material.color.g);
        writer.Key("B");
        writer.Int(material.color.b);
        writer.Key("A");
        writer.Int(material.color.a);
        writer.EndObject();
        writer.EndObject();
    }
    writer.EndArray();
}

void export_geometry_triangles(JsonWriter& writer, const gta_to_ue::Geometry& geometry)
{
    writer.Key("Triangles");
    writer.StartArray();
    for (auto& triangle : geometry.triangles) 
    {
        writer.StartObject();
        writer.Key("A");
        writer.Int(triangle.vertex1);
        writer.Key("B");
        writer.Int(triangle.vertex2);
        writer.Key("C");
        writer.Int(triangle.vertex3);
        writer.Key("MaterialID");
        writer.Int(triangle.material_id);
        writer.EndObject();
    }
    writer.EndArray();
}

void export_geometry_tex_coordinate_sets(JsonWriter& writer, const gta_to_ue::Geometry& geometry)
{
    writer.Key("TextureCoordinates");
    writer.StartArray();
    for (auto& tex_coordinate_set : geometry.tex_coordinate_sets) 
    {
        //writer.StartArray(); ue4 doesn't support nested tarray
        for (auto& tex_coordinate : tex_coordinate_set) 
        {
            writer.StartObject();
            writer.Key("U");
            writer.Double(tex_coordinate.x);
            writer.Key("V");
            writer.Double(tex_coordinate.y);
            writer.EndObject();
        }
        //writer.EndArray();
    }
    writer.EndArray();
}

void export_geometry_vertex_data(JsonWriter& writer, const gta_to_ue::Geometry& geometry)
{
    writer.Key("Vertices");
    writer.StartArray();
    for (auto& vertex : geometry.vertices) {
        writer.StartObject();
        writer.Key("X");
        writer.Double(vertex.x);
        writer.Key("Y");
        writer.Double(vertex.y);
        writer.Key("Z");
        writer.Double(vertex.z);
        writer.EndObject();
    }
    writer.EndArray();

    writer.Key("Normals");
    writer.StartArray();
    for (auto& normal : geometry.normals) {
        writer.StartObject();
        writer.Key("X");
        writer.Double(normal.x);
        writer.Key("Y");
        writer.Double(normal.y);
        writer.Key("Z");
        writer.Double(normal.z);
        writer.EndObject();
    }
    writer.EndArray();
}

void export_geometry_skeleton(JsonWriter& writer, const gta_to_ue::Geometry& geometry)
{
    writer.Key("Skeleton");
    writer.StartObject();
    writer.Key("NumBones");
    writer.Int(geometry.skeleton.num_bones);
    writer.Key("NumUsedBones");
    writer.Int(geometry.skeleton.num_used_bones);

    writer.Key("Weights");
    writer.StartArray();
    for (auto& weight : geometry.skeleton.weights) {
        writer.StartObject();
        writer.Key("WeightOne");
        writer.Double(weight.weight1);
        writer.Key("WeightTwo");
        writer.Double(weight.weight2);
        writer.Key("WeightThree");
        writer.Double(weight.weight3);
        writer.Key("WeightFour");
        writer.Double(weight.weight4);
        writer.EndObject();
    }
    writer.EndArray();

    writer.Key("Indices");
    writer.StartArray();
    for (auto& index : geometry.skeleton.bone_indices) {
        writer.StartObject();
        writer.Key("BoneOne");
        writer.Double(index.bone1);
        writer.Key("BoneTwo");
        writer.Double(index.bone2);
        writer.Key("BoneThree");
        writer.Double(index.bone3);
        writer.Key("BoneFour");
        writer.Double(index.bone4);
        writer.EndObject();
    }
    writer.EndArray();

    writer.Key("Ids");
    writer.StartArray();
    for (auto& id : geometry.skeleton.bone_ids) {
        writer.Int(id);
    }
    writer.EndArray();

    writer.Key("Transform");
    writer.StartArray();
    for (auto& transform : geometry.skeleton.inverse_matrices) {
        writer.StartObject();
        writer.Key("AxisX");
        writer.StartObject();
        writer.Key("X");
        writer.Double(transform.x_axis.x);
        writer.Key("Y");
        writer.Double(transform.x_axis.y);
        writer.Key("Z");
        writer.Double(transform.x_axis.z);
        writer.EndObject();
        writer.Key("AxisY");
        writer.StartObject();
        writer.Key("X");
        writer.Double(transform.y_axis.x);
        writer.Key("Y");
        writer.Double(transform.y_axis.y);
        writer.Key("Z");
        writer.Double(transform.y_axis.z);
        writer.EndObject();
        writer.Key("AxisZ");
        writer.StartObject();
        writer.Key("X");
        writer.Double(transform.z_axis.x);
        writer.Key("Y");
        writer.Double(transform.z_axis.y);
        writer.Key("Z");
        writer.Double(transform.z_axis.z);
        writer.EndObject();
        writer.Key("Position");
        writer.StartObject();
        writer.Key("X");
        writer.Double(transform.pos.x);
        writer.Key("Y");
        writer.Double(transform.pos.y);
        writer.Key("Z");
        writer.Double(transform.pos.z);
        writer.EndObject();
        writer.EndObject();
    }
    writer.EndArray();

    writer.EndObject();
}

void export_geometries(JsonWriter& writer, const gta_to_ue::Mesh& mesh_data)
{
    writer.Key("Geometries");
    writer.StartArray();
    for (auto& geometry : mesh_data.geometries) 
    {
        writer.StartObject();
        writer.Key("FrameID");
        writer.Int(geometry.frame_id);
        writer.Key("HasSkeleton");
        writer.Bool(geometry.has_skeleton);
        export_geometry_skeleton(writer, geometry);
        export_geometry_triangles(writer, geometry);
        export_geometry_tex_coordinate_sets(writer, geometry);
        export_geometry_vertex_data(writer, geometry);
        writer.EndObject();
    }
    writer.EndArray();
}

void export_object(JsonWriter& writer, const gta_to_ue::Mesh& mesh_data)
{
    writer.StartObject();
    export_object_info(writer, mesh_data);
    export_object_frames(writer, mesh_data);
    export_object_anim_hierarchies(writer, mesh_data);
    export_object_materials(writer, mesh_data);
    export_geometries(writer, mesh_data);
    writer.EndObject();
}

bool gta_to_ue::json::export_to_file(const std::string& file_name, const gta_to_ue::Mesh& mesh_data)
{
    std::ofstream ofs;
    ofs.open(file_name);

    if (!ofs.is_open()) {
        return false;
    }

    rapidjson::StringBuffer s;
    JsonWriter writer(s);
    export_object(writer, mesh_data);

    ofs << s.GetString();
    ofs.close();

    return true;
}
