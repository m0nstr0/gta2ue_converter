#include "json.h"
#include <fstream>
#include <string>
#include <rapidjson/writer.h>

using JsonWriter = rapidjson::Writer<rapidjson::StringBuffer>;

void export_object_info(JsonWriter& writer, const gta_to_ue::Mesh& mesh_data)
{
    writer.Key("Info");
    writer.StartObject();
    writer.Key("NumAtomics");
    writer.Int(mesh_data.atomics.size());
    writer.Key("NumGeometries");
    writer.Int(mesh_data.geometries.size());
    writer.Key("NumMaterials");
    writer.Int(mesh_data.materials.size());
    writer.Key("NumFrames");
    writer.Int(mesh_data.frames.size());
    writer.Key("NumAnimHierarchies"); 
    writer.Int(mesh_data.anim_hierarchy.size());
    writer.Key("NumSkins");
    writer.Int(mesh_data.skins.size());
    writer.Key("Version");
    writer.Int(1);
    writer.EndObject();
}

void export_object_skins(JsonWriter& writer, const gta_to_ue::Mesh& mesh_data)
{
    writer.Key("Skins");
    writer.StartArray();
    for (auto& skin : mesh_data.skins) 
    {
        writer.StartObject();
        writer.Key("GeometryID");
        writer.Int(skin.geometry_id);
        writer.Key("NumBones");
        writer.Int(skin.num_bones);
        writer.Key("NumUsedBones");
        writer.Int(skin.num_used_bones);
        writer.Key("MaxWeightsPerVertex");
        writer.Int(skin.max_weights_per_vertex);
        writer.Key("UsedBones");
        writer.StartArray();
        for (auto& bone : skin.bone_ids) 
        {
            writer.StartObject();
            writer.Key("BoneID");
            writer.Int(bone);
            writer.EndObject();
        }
        writer.EndArray();
        writer.Key("Indices");
        writer.StartArray();
        for (auto& index : skin.bone_indices) 
        {
            writer.StartObject();
            writer.Key("VertexID");
            writer.Int(index.vertex_id); //TODO: annd vertex id
            writer.Key("BoneOne");
            writer.Int(index.bone1);
            writer.Key("BoneTwo");
            writer.Int(index.bone2);
            writer.Key("BoneThree");
            writer.Int(index.bone3);
            writer.Key("BoneFour");
            writer.Int(index.bone4);
            writer.EndObject();
        }
        writer.EndArray();
        writer.Key("Weights");
        writer.StartArray();
        for (auto& weight : skin.weights) 
        {
            writer.StartObject();
            writer.Key("VertexID");
            writer.Int(weight.vertex_id); //TODO: annd vertex id
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
        writer.Key("Transform");
        writer.StartArray();
        for (auto& transform : skin.inverse_matrices) 
        {
            writer.StartObject();
            writer.Key("XAxis");
            writer.StartObject();
            writer.Key("X");
            writer.Double(transform.x_axis.x);
            writer.Key("Y");
            writer.Double(transform.x_axis.y);
            writer.Key("Z");
            writer.Double(transform.x_axis.z);
            writer.EndObject();
            writer.Key("YAxis");
            writer.StartObject();
            writer.Key("X");
            writer.Double(transform.y_axis.x);
            writer.Key("Y");
            writer.Double(transform.y_axis.y);
            writer.Key("Z");
            writer.Double(transform.y_axis.z);
            writer.EndObject();
            writer.Key("ZAxis");
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
    writer.EndArray();
}

void export_object_anim_hierarchies(JsonWriter& writer, const gta_to_ue::Mesh& mesh_data)
{
    writer.Key("AnimHierarchies");
    writer.StartArray();
    for (auto& anim_hierarchy : mesh_data.anim_hierarchy) {
        writer.StartObject();
        writer.Key("ID");
        writer.Int(anim_hierarchy.id);
        writer.Key("FrameID");
        writer.Int(anim_hierarchy.frame_id);
        writer.Key("Flags");
        writer.Int(anim_hierarchy.flags);
        writer.Key("MaxInterpKeySize");
        writer.Int(anim_hierarchy.interp_key_size);
        writer.Key("NumNodes");
        writer.Int(anim_hierarchy.nodes.size());
        writer.Key("Nodes");
        writer.StartArray();
        for (auto& node : anim_hierarchy.nodes) {
            writer.StartObject();
            writer.Key("ID");
            writer.Int(node.id);
            writer.Key("Index");
            writer.Int(node.index);
            writer.Key("Flags");
            writer.Int(node.flags);
            writer.EndObject();
        }
        writer.EndArray();
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
        writer.Key("FrameName");
        writer.String(frame.name.c_str());
        writer.Key("ParentID");
        writer.Int(frame.parent_frame_id);
        writer.Key("Transform");
        writer.StartObject();
        writer.Key("XAxis");
        writer.StartObject();
        writer.Key("X");
        writer.Double(frame.x_axis.x);
        writer.Key("Y");
        writer.Double(frame.x_axis.y);
        writer.Key("Z");
        writer.Double(frame.x_axis.z);
        writer.EndObject();
        writer.Key("YAxis");
        writer.StartObject();
        writer.Key("X");
        writer.Double(frame.y_axis.x);
        writer.Key("Y");
        writer.Double(frame.y_axis.y);
        writer.Key("Z");
        writer.Double(frame.y_axis.z);
        writer.EndObject();
        writer.Key("ZAxis");
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
        writer.Key("MaterialName");
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
        writer.Key("VertexOne");
        writer.Int(triangle.vertex1);
        writer.Key("VertexTwo");
        writer.Int(triangle.vertex2);
        writer.Key("VertexThree");
        writer.Int(triangle.vertex3);
        writer.Key("MaterialID");
        writer.Int(triangle.material_id);
        writer.EndObject();
    }
    writer.EndArray();
}

void export_geometry_info(JsonWriter& writer, const gta_to_ue::Geometry& geometry)
{
    writer.Key("Info");
    writer.StartObject();
    writer.Key("NumTriangles");
    writer.Int(geometry.triangles.size());
    writer.Key("NumTexCoordSets");
    writer.Int(geometry.tex_coordinate_sets.size());
    writer.Key("NumVertices");
    writer.Int(geometry.num_vertices);
    writer.Key("NumMorphTargets");
    writer.Int(geometry.morph_targets.size());
    writer.Key("NumMaterials");
    writer.Int(geometry.num_materials);
    writer.EndObject();
}

void export_geometry_tex_coordinate_sets(JsonWriter& writer, const gta_to_ue::Geometry& geometry)
{
    writer.Key("TexCoordSets");
    writer.StartArray();
    for (auto& tex_coordinate_set : geometry.tex_coordinate_sets) 
    {
        writer.StartArray();
        for (auto& tex_coordinate : tex_coordinate_set) 
        {
            writer.StartObject();
            writer.Key("U");
            writer.Double(tex_coordinate.x);
            writer.Key("V");
            writer.Double(tex_coordinate.y);
            writer.EndObject();
        }
        writer.EndArray();
    }
    writer.EndArray();
}

void export_morph_target_vertices(JsonWriter& writer, const gta_to_ue::MorphTarget& morph_target)
{
    writer.Key("Vertices");
    writer.StartArray();
    for (auto& vertex : morph_target.vertices) 
    {
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
}

void export_morph_target_normals(JsonWriter& writer, const gta_to_ue::MorphTarget& morph_target)
{
    writer.Key("Normals");
    writer.StartArray();
    for (auto& normal : morph_target.normals) 
    {
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

void export_geometry_morph_target(JsonWriter& writer, const gta_to_ue::Geometry& geometry)
{
    writer.Key("MorphTarget");
    writer.StartArray();
    for (auto& morph_target : geometry.morph_targets) 
    {
        writer.StartObject();
        export_morph_target_vertices(writer, morph_target);
        export_morph_target_normals(writer, morph_target);
        writer.EndObject();
    }
    writer.EndArray();
}

void export_geometries(JsonWriter& writer, const gta_to_ue::Mesh& mesh_data)
{
    writer.Key("Geometry");
    writer.StartArray();
    for (auto& geometry : mesh_data.geometries) 
    {
        writer.StartObject();
        export_geometry_info(writer, geometry);
        export_geometry_triangles(writer, geometry);
        export_geometry_tex_coordinate_sets(writer, geometry);
        export_geometry_morph_target(writer, geometry);
        writer.EndObject();
    }
    writer.EndArray();
}

void export_atomics(JsonWriter& writer, const gta_to_ue::Mesh& mesh_data)
{
    writer.Key("Atomic");
    writer.StartArray();
    for (auto& atomic : mesh_data.atomics)
    {
        writer.StartObject();
        writer.Key("FrameID");
        writer.Int(atomic.frame_id);
        writer.Key("GeometryID");
        writer.Int(atomic.geometry_id);
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
    export_object_skins(writer, mesh_data);
    export_object_materials(writer, mesh_data);
    export_geometries(writer, mesh_data);
    export_atomics(writer, mesh_data);
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
