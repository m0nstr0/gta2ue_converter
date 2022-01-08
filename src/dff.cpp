#include "dff.h"

#include <filesystem>
#include <iostream>

void parse_rw_frames(const rw::FrameList_& frame_list, gta_to_ue::Mesh& mesh_data)
{
    mesh_data.frames.reserve(frame_list.numFrames);
    for (uint32_t i = 0; i < frame_list.numFrames; i++) {
        mesh_data.frames.emplace_back(
            gta_to_ue::Vector3f(frame_list.frames[i]->matrix.right.x, frame_list.frames[i]->matrix.right.y, frame_list.frames[i]->matrix.right.z), 
            gta_to_ue::Vector3f(frame_list.frames[i]->matrix.at.x, frame_list.frames[i]->matrix.at.y, frame_list.frames[i]->matrix.at.z), 
            gta_to_ue::Vector3f(frame_list.frames[i]->matrix.up.x, frame_list.frames[i]->matrix.up.y, frame_list.frames[i]->matrix.up.z), 
            gta_to_ue::Vector3f(frame_list.frames[i]->matrix.pos.x, frame_list.frames[i]->matrix.pos.y, frame_list.frames[i]->matrix.pos.z),
            rw::findPointer(frame_list.frames[i]->getParent(), reinterpret_cast<void**>(frame_list.frames), frame_list.numFrames),
            gta::getNodeName(frame_list.frames[i])
        );
    }
}

size_t get_hash_for_material(const rw::Material* material)
{
    if (!material) {
        return 0;
    }

    if (material->texture) {
        const std::string diffuse_texture_name(material->texture->name, 32);
        const std::string mask_texture_name(material->texture->mask, 32);
        return std::hash<std::string>{}(diffuse_texture_name + mask_texture_name);
    }

    const gta_to_ue::Color color(material->color.red, material->color.green, material->color.blue, material->color.alpha);
    return  std::hash<std::string>{}(color.to_string());
}

int32_t get_material_id(const rw::Material* material, gta_to_ue::Mesh& mesh_data)
{
    return mesh_data.materials.get_material_id_with_hash(get_hash_for_material(material));
}

void parse_rw_materials(const rw::Geometry* geometry, gta_to_ue::Mesh& mesh_data, const std::string& filename)
{
    for (int32_t i = 0; i < geometry->matList.numMaterials; i++)
    {
        std::string material_name;
        const int32_t index = mesh_data.materials.size();
        gta_to_ue::Color color(
            geometry->matList.materials[i]->color.red, 
            geometry->matList.materials[i]->color.green, 
            geometry->matList.materials[i]->color.blue, 
            geometry->matList.materials[i]->color.alpha
        );

        if (geometry->matList.materials[i]->texture)
        {
            std::string diffuse_texture_name(geometry->matList.materials[i]->texture->name, 32);
            std::string mask_texture_name(geometry->matList.materials[i]->texture->mask, 32);
            size_t hash = std::hash<std::string>{}(diffuse_texture_name + mask_texture_name);

            if (mesh_data.materials.has_material_with_hash(hash)) {
                return;
            }

            std::ostringstream s;
            s << filename << "_" << index;
            mesh_data.materials.emplace_back(s.str(), diffuse_texture_name, mask_texture_name, color, hash, index);
        }
        else
        {
            const size_t hash = std::hash<std::string>{}(color.to_string());
            if (mesh_data.materials.has_material_with_hash(hash)) {
                return;
            }

            std::ostringstream s;
            s << filename << "_" << index;
            mesh_data.materials.emplace_back(s.str(), "", "", color, hash, index);
        }
    }
}

void parse_rw_geometry(const rw::Geometry* geometry, gta_to_ue::Mesh& mesh_data, const std::string& filename)
{
    auto& mesh_geometry_data = mesh_data.geometries.emplace_back(
        geometry->meshHeader->totalIndices / 3,
        geometry->numTexCoordSets, geometry->numMorphTargets,
        geometry->numVertices,
        geometry->matList.numMaterials
    );
    const rw::Mesh* meshes = geometry->meshHeader->getMeshes();

    parse_rw_materials(geometry, mesh_data, filename);

    if (geometry->meshHeader->flags == rw::MeshHeader::TRISTRIP) {
        for (int32_t i = 0; i < geometry->meshHeader->numMeshes; i++) {
            for (int32_t j = 0; j < meshes[i].numIndices; j++) {
                if (j == 0)
                    mesh_geometry_data.triangles.emplace_back(meshes[i].indices[j + 2], meshes[i].indices[j + 1], meshes[i].indices[j], get_material_id(meshes[i].material, mesh_data));
                if (j > 2 && j % 2 == 0)
                    mesh_geometry_data.triangles.emplace_back(meshes[i].indices[j - 2], meshes[i].indices[j], meshes[i].indices[j - 1], get_material_id(meshes[i].material, mesh_data));
                if (j > 2 && j % 2 != 0)
                    mesh_geometry_data.triangles.emplace_back(meshes[i].indices[j - 2], meshes[i].indices[j - 1], meshes[i].indices[j], get_material_id(meshes[i].material, mesh_data));
            }
        }
    }
    else {
        for (int32_t i = 0; i < geometry->meshHeader->numMeshes; i++) {
            for (int32_t j = 0; j < meshes[i].numIndices; j += 3) {
                mesh_geometry_data.triangles.emplace_back(meshes[i].indices[j], meshes[i].indices[j + 1], meshes[i].indices[j + 2], get_material_id(meshes[i].material, mesh_data));
            }
        }
    }

    for (int32_t i = 0; i < geometry->numTexCoordSets; i++) {
        auto& tex_coords = mesh_geometry_data.tex_coordinate_sets.emplace_back();
        tex_coords.reserve(geometry->numVertices);
        for (int32_t j = 0; j < geometry->numVertices; j++) {
            tex_coords.emplace_back(geometry->texCoords[i][j].u, geometry->texCoords[i][j].v);
        }
    }

    for (int32_t i = 0; i < geometry->numMorphTargets; i++) {
        auto& morph_target = mesh_geometry_data.morph_targets.emplace_back(geometry->numVertices);
        for (int32_t j = 0; j < geometry->numVertices; j++) {
            morph_target.vertices.emplace_back(geometry->morphTargets[i].vertices[j].x * 100.f, geometry->morphTargets[i].vertices[j].y * -100.f, geometry->morphTargets[i].vertices[j].z * 100.f);
            if (geometry->morphTargets[i].normals) {
                morph_target.normals.emplace_back(geometry->morphTargets[i].normals[j].x, -geometry->morphTargets[i].normals[j].y, geometry->morphTargets[i].normals[j].z);
            }
            else {
                morph_target.normals.emplace_back(0.f, 0.f, 1.f);
            }
        }
    }
}

rw::Clump* gta_to_ue::dff::parse(const std::string& dff_file_name, gta_to_ue::Mesh& mesh_data)
{
    rw::StreamFile stream_file;

    const std::filesystem::path path = std::filesystem::path(dff_file_name);
    const std::string ext = path.has_extension() ? path.extension().string() : "";
    std::string filename = path.has_filename() ? path.filename().string() : "";
    if (ext.compare("")) {
        filename = filename.substr(0, filename.length() - ext.length());
    }

    if (!stream_file.open(dff_file_name.c_str(), "rb")) {
        std::cout << "file: " << dff_file_name << " is not found" << std::endl;
        return nullptr;
    }

    if (!rw::findChunk(&stream_file, rw::ID_CLUMP, nullptr, nullptr)) {
        std::cout << "file: " << dff_file_name << " is not a clump" << std::endl;
        stream_file.close();
        return nullptr;
    }

    rw::Clump* clump = rw::Clump::streamRead(&stream_file);
    if (!clump) {
        std::cout << "file: " << dff_file_name << " parsing error" << std::endl;
    }

    stream_file.close();

    //frames data
    const rw::FrameList_ frame_list{
        .numFrames = clump->getFrame()->count(),
        .frames = static_cast<rw::Frame**>(rwMalloc(clump->getFrame()->count() * sizeof(rw::Frame*), rw::MEMDUR_FUNCTION | rw::ID_CLUMP))
    };
    rw::makeFrameList(clump->getFrame(), frame_list.frames);

    //frames
    parse_rw_frames(frame_list, mesh_data);

    //atomics
    FORLIST(lnk, clump->atomics)
    {
        const rw::Atomic* atomic = rw::Atomic::fromClump(lnk);
        parse_rw_geometry(atomic->geometry, mesh_data, filename);
        mesh_data.atomics.emplace_back(mesh_data.geometries.size() - 1, rw::findPointer(atomic->getFrame(), reinterpret_cast<void**>(frame_list.frames), frame_list.numFrames));
    }
    rwFree(frame_list.frames);

    return clump;
}