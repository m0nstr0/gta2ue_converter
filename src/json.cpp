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
    writer.Int(mesh_data.geometries.size());
    writer.Key("NumMaterials");
    writer.Int(mesh_data.materials.size());
    writer.Key("Version");
    writer.Int(1);
    writer.EndObject();
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

void export_atomic_geometry(JsonWriter& writer, const gta_to_ue::Mesh& mesh_data)
{
    for (auto& geometry : mesh_data.geometries) 
    {
        writer.StartObject();
        writer.Key("Geometry");
        writer.StartObject();
        export_geometry_info(writer, geometry);
        export_geometry_triangles(writer, geometry);
        export_geometry_tex_coordinate_sets(writer, geometry);
        export_geometry_morph_target(writer, geometry);
        writer.EndObject();
        writer.EndObject();
    }
}

void export_object_atomic(JsonWriter& writer, const gta_to_ue::Mesh& mesh_data)
{
    writer.Key("Atomic");
    writer.StartArray();
    export_atomic_geometry(writer, mesh_data);
    writer.EndArray();
}

void export_object(JsonWriter& writer, const gta_to_ue::Mesh& mesh_data)
{
    writer.StartObject();
    export_object_info(writer, mesh_data);
    export_object_materials(writer, mesh_data);
    export_object_atomic(writer, mesh_data);
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
