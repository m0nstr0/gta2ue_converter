#include "dff.h"
#include "car.h"

#include <filesystem>
#include <iostream>
#include <map>

gta_to_ue::Vector3f convert_vector_xyz(const ConvertingOptions& converting_options, float x, float y, float z, float multiplicator, bool negate_y = false)
{
	if (converting_options.is_car) {
		return gta_to_ue::Vector3f(y * multiplicator, (negate_y ? (-x) : x) * multiplicator, z * multiplicator);
	}

	return gta_to_ue::Vector3f(x * multiplicator, y * multiplicator, z * multiplicator);
}

void parse_rw_skin_data(const rw::Geometry* geometry, int32_t geometry_id, gta_to_ue::Mesh& mesh_data)
{
    const rw::Skin* skin = rw::Skin::get(geometry);
    if (!skin) {
        return;
    }

    mesh_data.geometries[geometry_id].has_skeleton = true;
    mesh_data.has_skeleton = true;
    mesh_data.same_skeleton = false;

    auto& skeleton = mesh_data.geometries[geometry_id].skeleton;

    skeleton.num_bones = skin->numBones;
    skeleton.num_used_bones = skin->numUsedBones;

    skeleton.bone_ids.reserve(skin->numUsedBones);
    for (int32_t i = 0; i < skin->numUsedBones; i++) {
        skeleton.bone_ids.push_back(skin->usedBones[i]);
    }

    skeleton.inverse_matrices.reserve(skin->numBones);
    rw::Matrix* matrices = reinterpret_cast<rw::Matrix*>(skin->inverseMatrices);
    for (int32_t i = 0; i < skin->numBones; i++) {
        rw::Matrix* mat = (rw::Matrix*)&skin->inverseMatrices[i * 16];
        rw::Matrix inv = *mat;

        skeleton.inverse_matrices.emplace_back(
            gta_to_ue::Vector3f(inv.right.x, inv.right.y, inv.right.z),
            gta_to_ue::Vector3f(inv.at.x, inv.at.y, inv.at.z),
            gta_to_ue::Vector3f(inv.up.x, inv.up.y, inv.up.z),
            gta_to_ue::Vector3f(inv.pos.x, inv.pos.y, inv.pos.z)
        );
    }

    skeleton.weights.reserve(geometry->numVertices);
    skeleton.bone_indices.reserve(geometry->numVertices);
    const gta_to_ue::VertexWeight* weights = reinterpret_cast<gta_to_ue::VertexWeight*>(skin->weights);
    const gta_to_ue::BoneIndex* bone_indices = reinterpret_cast<gta_to_ue::BoneIndex*>(skin->indices);
    for (int32_t i = 0; i < geometry->numVertices; i++) {
        skeleton.weights.push_back(weights[i]);
        skeleton.bone_indices.push_back(bone_indices[i]);
    }
}

void parse_rw_frames(const rw::FrameList_& frame_list, const ConvertingOptions& converting_options, gta_to_ue::Mesh& mesh_data)
{
    mesh_data.frames.reserve(frame_list.numFrames);
    int32_t hierarchy_id = -1;
    std::map<int32_t, int32_t> bone_id_to_frame_id;

    for (uint32_t i = 0; i < frame_list.numFrames; i++) {
        if (!converting_options.is_car) {
            mesh_data.frames.emplace_back(
                gta_to_ue::Vector3f(frame_list.frames[i]->matrix.right.x, frame_list.frames[i]->matrix.right.y, frame_list.frames[i]->matrix.right.z),
                gta_to_ue::Vector3f(frame_list.frames[i]->matrix.at.x, frame_list.frames[i]->matrix.at.y, frame_list.frames[i]->matrix.at.z),
                gta_to_ue::Vector3f(frame_list.frames[i]->matrix.up.x, frame_list.frames[i]->matrix.up.y, frame_list.frames[i]->matrix.up.z),
				convert_vector_xyz(converting_options, frame_list.frames[i]->matrix.pos.x, frame_list.frames[i]->matrix.pos.y, frame_list.frames[i]->matrix.pos.z, 100.f),
                rw::findPointer(frame_list.frames[i]->getParent(), reinterpret_cast<void**>(frame_list.frames), frame_list.numFrames),
                gta::getNodeName(frame_list.frames[i])
            );
        } else {
            mesh_data.frames.emplace_back(
                gta_to_ue::Vector3f(1.f, 0.f, 0.f),
                gta_to_ue::Vector3f(0.f, 1.f, 0.f),
                gta_to_ue::Vector3f(0.f, 0.f, 1.f),
                convert_vector_xyz(converting_options, frame_list.frames[i]->matrix.pos.x, frame_list.frames[i]->matrix.pos.y, frame_list.frames[i]->matrix.pos.z, 100.f),
                rw::findPointer(frame_list.frames[i]->getParent(), reinterpret_cast<void**>(frame_list.frames), frame_list.numFrames),
                gta::getNodeName(frame_list.frames[i])
            );
        }

        if (const rw::HAnimData* h_anim_data = rw::HAnimData::get(frame_list.frames[i]); h_anim_data->id >= 0) {
            bone_id_to_frame_id[h_anim_data->id] = i;
            if (h_anim_data->hierarchy) {
                hierarchy_id = i;
            }
        }
    }

    if (hierarchy_id == -1) {
        return;
    }

    const rw::HAnimData* h_anim_data = rw::HAnimData::get(frame_list.frames[hierarchy_id]);
    std::vector<gta_to_ue::BoneHierarchy> bones(h_anim_data->hierarchy->numNodes);

    int32_t stack_pointer = 0;
    int32_t stack[32];
    int32_t parent_id = 0;
    bones[0].parent_id = -1;
    bones[0].frame_id = bone_id_to_frame_id[h_anim_data->hierarchy->nodeInfo[0].id];
    bones[0].max_frame_size = h_anim_data->hierarchy->interpolator->maxInterpKeyFrameSize;
    stack[stack_pointer++] = parent_id;
    for (int32_t i = 1; i != h_anim_data->hierarchy->numNodes; i++) {
        bones[i].parent_id = parent_id;
        bones[i].frame_id = bone_id_to_frame_id[h_anim_data->hierarchy->nodeInfo[i].id];
        bones[i].max_frame_size = h_anim_data->hierarchy->interpolator->maxInterpKeyFrameSize;
        if (h_anim_data->hierarchy->nodeInfo[i].flags & rw::HAnimHierarchy::PUSH) {
            stack[stack_pointer++] = parent_id;
        }
        parent_id = i;
        if (h_anim_data->hierarchy->nodeInfo[i].flags & rw::HAnimHierarchy::POP) {
            parent_id = stack[--stack_pointer];
        }
    }

    mesh_data.bone_hierarchy = std::move(bones);
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
            geometry->matList.materials[i]->color.red / 255.f, 
            geometry->matList.materials[i]->color.green / 255.f, 
            geometry->matList.materials[i]->color.blue / 255.f, 
            geometry->matList.materials[i]->color.alpha / 255.f
        );

        if (geometry->matList.materials[i]->texture) {
            std::string diffuse_texture_name(geometry->matList.materials[i]->texture->name, 32);
            std::string mask_texture_name(geometry->matList.materials[i]->texture->mask, 32);
            size_t hash = std::hash<std::string>{}(diffuse_texture_name + mask_texture_name);
            if (mesh_data.materials.has_material_with_hash(hash)) {
                return;
            }
            std::ostringstream s;
            s << filename << "_" << index;
            mesh_data.materials.emplace_back(s.str(), diffuse_texture_name, mask_texture_name, color, hash, index);
        } else {
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

void parse_rw_geometry(const rw::Geometry* geometry, int32_t frame_id, gta_to_ue::Mesh& mesh_data, const ConvertingOptions& converting_options, const std::string& filename)
{
    auto& mesh_geometry_data = mesh_data.geometries.emplace_back(
        geometry->meshHeader->totalIndices / 3,
        geometry->numTexCoordSets,
        geometry->numVertices,
        geometry->matList.numMaterials,
        frame_id
    );
    const rw::Mesh* meshes = geometry->meshHeader->getMeshes();

    parse_rw_materials(geometry, mesh_data, filename);

    if (!converting_options.is_car) {
        parse_rw_skin_data(geometry, mesh_data.geometries.size() - 1, mesh_data);
    }

    if (geometry->meshHeader->flags == rw::MeshHeader::TRISTRIP) {
        for (int32_t i = 0; i < geometry->meshHeader->numMeshes; i++) {
            for (int32_t j = 0; j < meshes[i].numIndices; j++) {
                if (j > 2 && j % 2 == 0 || j == 2)
                    mesh_geometry_data.triangles.emplace_back(meshes[i].indices[j], meshes[i].indices[j - 2], meshes[i].indices[j - 1], get_material_id(meshes[i].material, mesh_data));
                if (j > 2 && j % 2 != 0)
                    mesh_geometry_data.triangles.emplace_back(meshes[i].indices[j], meshes[i].indices[j - 1], meshes[i].indices[j - 2], get_material_id(meshes[i].material, mesh_data));
            }
        }
    } else {
        for (int32_t i = 0; i < geometry->meshHeader->numMeshes; i++) {
            for (int32_t j = 0; j < meshes[i].numIndices; j += 3) {
                mesh_geometry_data.triangles.emplace_back(meshes[i].indices[j + 2], meshes[i].indices[j], meshes[i].indices[j + 1], get_material_id(meshes[i].material, mesh_data));
            }
        }
    }

    std::vector<gta_to_ue::Triangle> NewTriangles;
    for (auto& triangle : mesh_geometry_data.triangles) {
        if (triangle.vertex1 == triangle.vertex2 || triangle.vertex2 == triangle.vertex3 || triangle.vertex1 == triangle.vertex3)
            continue;
        NewTriangles.push_back(triangle);
    }
    mesh_geometry_data.triangles = std::move(NewTriangles);

    for (int32_t i = 0; i < geometry->numTexCoordSets; i++) {
        auto& tex_coords = mesh_geometry_data.tex_coordinate_sets.emplace_back();
        tex_coords.reserve(geometry->numVertices);
        for (int32_t j = 0; j < geometry->numVertices; j++) {
            tex_coords.emplace_back(geometry->texCoords[i][j].u, geometry->texCoords[i][j].v);
        }
    }

    if (geometry->numMorphTargets == 0) {
        //todo: error
        return;
    }

    if (geometry->numMorphTargets > 1) {
        //todo: warning
    }

    const rw::MorphTarget& morph_target = geometry->morphTargets[0];
    for (int32_t j = 0; j < geometry->numVertices; j++) {
        mesh_geometry_data.vertices.emplace_back(convert_vector_xyz(converting_options, morph_target.vertices[j].x, morph_target.vertices[j].y, morph_target.vertices[j].z, 100.f, true));
        if (morph_target.normals) {
            mesh_geometry_data.normals.emplace_back(convert_vector_xyz(converting_options, morph_target.normals[j].x, morph_target.normals[j].y, morph_target.normals[j].z, 1.f));
        } else {
            mesh_geometry_data.normals.emplace_back(convert_vector_xyz(converting_options, 0.f, 0.f, 1.f, 1.f));
        }
    }
}

rw::Clump* read_clump(const std::string& dff_file_name)
{
    rw::StreamFile dff_stream_file;

	if (!dff_stream_file.open(dff_file_name.c_str(), "rb")) {
		std::cout << "file: " << dff_file_name << " is not found" << std::endl;
		return nullptr;
	}

	if (!rw::findChunk(&dff_stream_file, rw::ID_CLUMP, nullptr, nullptr)) {
		std::cout << "file: " << dff_file_name << " is not a clump" << std::endl;
		dff_stream_file.close();
		return nullptr;
	}

	rw::Clump* clump = rw::Clump::streamRead(&dff_stream_file);
	if (!clump) {
		std::cout << "file: " << dff_file_name << " parsing error" << std::endl;
		return nullptr;
	}

	dff_stream_file.close();

    return clump;
}

void parse_dff(rw::Clump* clump, const ConvertingOptions& converting_options, gta_to_ue::Mesh& mesh_data, const std::string& filename)
{
	//frames data
	const rw::FrameList_ frame_list{
		.numFrames = clump->getFrame()->count(),
		.frames = static_cast<rw::Frame**>(rwMalloc(clump->getFrame()->count() * sizeof(rw::Frame*), rw::MEMDUR_FUNCTION | rw::ID_CLUMP))
	};
	rw::makeFrameList(clump->getFrame(), frame_list.frames);

	//frames
	parse_rw_frames(frame_list, converting_options, mesh_data);

	//atomics
	FORLIST(lnk, clump->atomics)
	{
		const rw::Atomic* atomic = rw::Atomic::fromClump(lnk);
		atomic->geometry->correctTristripWinding();
		atomic->geometry->generateTriangles();
		parse_rw_geometry(atomic->geometry, rw::findPointer(atomic->getFrame(), reinterpret_cast<void**>(frame_list.frames), frame_list.numFrames), mesh_data, converting_options, filename);
	}
	rwFree(frame_list.frames);
}

rw::Clump* gta_to_ue::dff::parse(const std::string& dff_file_name, const ConvertingOptions& converting_options, gta_to_ue::Mesh& mesh_data)
{
    const std::filesystem::path path = std::filesystem::path(dff_file_name);
    const std::string ext = path.has_extension() ? path.extension().string() : "";
    std::string filename = path.has_filename() ? path.filename().string() : "";
    if (ext.compare("")) {
        filename = filename.substr(0, filename.length() - ext.length());
    }

    rw::Clump* clump = read_clump(dff_file_name);
    if (!clump) {
        return nullptr;
    }

    parse_dff(clump, converting_options, mesh_data, filename);

    rw::Clump* wheels_clump;
    gta_to_ue::Mesh wheels_mesh_data;
    if (converting_options.is_car) {
        if (converting_options.wheels_dff != "") {
            wheels_clump = read_clump(converting_options.wheels_dff);
            if (wheels_clump) {
				parse_dff(wheels_clump, converting_options, wheels_mesh_data, "wheels");
                gta_to_ue::mixin_car_wheel(converting_options, mesh_data, wheels_mesh_data);
            }
        }

        gta_to_ue::build_car(mesh_data);
    }  

    return clump;
}