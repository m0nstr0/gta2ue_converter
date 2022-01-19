#include "car.h"
#include <map>

const std::vector<std::string> bone_names = {
	"wheel_rf_dummy",
	"wheel_rm_dummy",
	"wheel_rb_dummy",
	"wheel_lf_dummy",
	"wheel_lm_dummy",
	"wheel_lb_dummy",
	"bump_front_dummy",
	"bonnet_dummy",
	"wing_rf_dummy",
	"wing_rr_dummy",
	"wing_lf_dummy",
	"wing_lr_dummy",
	"door_rf_dummy",
	"door_rr_dummy",
	"door_lf_dummy",
	"door_lr_dummy",
	"boot_dummy",
	"bump_rear_dummy",
	"windscreen_dummy",
	"ped_frontseat",
	"ped_backseat",
	"headlights",
	"taillights",
	"exhaust",
	"chassis_dummy"
	"extra1",
	"extra2",
	"extra3",
	"extra4",
	"extra5",
	"extra6"
};

const struct
{
	int32_t id;
	std::string name;
} WheelsIDS[] = {
	{237, "wheel_rim_l0"},
	{238, "wheel_offroad_l0"},
	{239, "wheel_truck_l0"},
	{250, "wheel_sport_l0"},
	{251, "wheel_saloon_l0"},
	{252, "wheel_lightvan_l0"},
	{253, "wheel_classic_l0"},
	{254, "wheel_alloy_l0"},
	{255, "wheel_lighttruck_l0"},
	{256, "wheel_smallcar_l0"}
};


void clear_geometry_and_frames(gta_to_ue::Mesh& mesh)
{
	auto delete_geometry_lambda = [&frames = mesh.frames](const gta_to_ue::Geometry& geometry) {
		return frames[geometry.frame_id].name.ends_with("_lo") || frames[geometry.frame_id].name.ends_with("_vlo");
	};

	mesh.geometries.erase(std::remove_if(mesh.geometries.begin(), mesh.geometries.end(), delete_geometry_lambda), mesh.geometries.end());
}

void build_skeleton(gta_to_ue::Mesh& mesh, std::map<int32_t, int32_t>& frame_to_bone)
{
	std::vector<gta_to_ue::BoneHierarchy> bones(mesh.frames.size());
	mesh.has_skeleton = true;
	mesh.same_skeleton = true;

	for (auto& geometry : mesh.geometries) {
		geometry.has_skeleton = true;

		auto& skeleton = geometry.skeleton;
		skeleton.num_bones = mesh.bone_hierarchy.size();
		skeleton.num_used_bones = mesh.bone_hierarchy.size();

		skeleton.weights.reserve(geometry.vertices.size());
		skeleton.bone_indices.reserve(geometry.vertices.size());
		for (int32_t i = 0; i < geometry.vertices.size(); i++) {
			int32_t frame_id = geometry.frame_id;
			while (true) {
				if (std::find(bone_names.begin(), bone_names.end(), mesh.frames[frame_id].name) == bone_names.end()) {
					frame_id = mesh.frames[frame_id].parent_frame_id;
					if (frame_id == -1) {
						frame_id = 0;
						break;
					}
				} else {
					break;
				}
			}
			
			geometry.vertices[i] = geometry.vertices[i] + mesh.frames[frame_id].pos;
			skeleton.weights.push_back(gta_to_ue::VertexWeight{ 1.f, 0.f, 0.f, 0.f });
			skeleton.bone_indices.push_back(gta_to_ue::BoneIndex{ static_cast<uint8_t>(frame_to_bone[frame_id]), 0, 0, 0});
		}

		skeleton.bone_ids.reserve(mesh.bone_hierarchy.size());
		skeleton.inverse_matrices.reserve(mesh.bone_hierarchy.size());
		for (int32_t i = 0; i < skeleton.num_bones; i++) {
			skeleton.bone_ids.push_back(i);
			skeleton.inverse_matrices.emplace_back(
				gta_to_ue::Vector3f(1.f, 0.f, 0.f),
				gta_to_ue::Vector3f(0.f, 1.f, 0.f),
				gta_to_ue::Vector3f(0.f, 0.f, 1.f),
				gta_to_ue::Vector3f(0.f, 0.f, 1.f)
			);
		}
	}
}

void build_hierarchy(gta_to_ue::Mesh& mesh)
{
	std::vector<gta_to_ue::BoneHierarchy> bones;
	bones.reserve(mesh.frames.size());
	std::map<int32_t, int32_t> frame_to_bone;
	
	bones.emplace_back(gta_to_ue::BoneHierarchy(0, -1));
	frame_to_bone[0] = 0;
	int32_t bone_idx = 1;
	for (int32_t i = 1; i < mesh.frames.size(); i++) {
		if (std::find(bone_names.begin(), bone_names.end(), mesh.frames[i].name) != bone_names.end()) {
			bones.emplace_back(gta_to_ue::BoneHierarchy(i, 0));
			frame_to_bone[i] = bone_idx;
			bone_idx++;
		}
	}

	mesh.bone_hierarchy = std::move(bones);

	build_skeleton(mesh, frame_to_bone);
}

void gta_to_ue::mixin_car_wheel(const ConvertingOptions& converting_options, Mesh& mesh, Mesh& wheel_mesh)
{
	int32_t wheel_rf_dummy = -1;
	int32_t	wheel_rb_dummy = -1;
	int32_t wheel_rm_dummy = -1;

	int32_t wheel_lm_dummy = -1;
	int32_t wheel_lf_dummy = -1;
	int32_t wheel_lb_dummy = -1;

	for (int32_t i = 0; i < mesh.frames.size(); i++) {
		if (mesh.frames[i].name == "wheel_rf_dummy") {
			wheel_rf_dummy = i;
		}
		if (mesh.frames[i].name == "wheel_rm_dummy") {
			wheel_rm_dummy = i;
		}
		if (mesh.frames[i].name == "wheel_rb_dummy") {
			wheel_rb_dummy = i;
		}
		if (mesh.frames[i].name == "wheel_lf_dummy") {
			wheel_lf_dummy = i;
		}
		if (mesh.frames[i].name == "wheel_lm_dummy") {
			wheel_lm_dummy = i;
		}
		if (mesh.frames[i].name == "wheel_lb_dummy") {
			wheel_lb_dummy = i;
		}
	}
	std::string wheel_name = "wheel_classic_l0";
	for (int32_t i = 0; i < 10; i++) {
		if (WheelsIDS[i].id == converting_options.wheel_id) {
			wheel_name = WheelsIDS[i].name;
			break;
		}
	}

	for (auto& geometry : wheel_mesh.geometries) {
		if (wheel_mesh.frames[geometry.frame_id].name == wheel_name) {
			//copy materials
			std::map<int32_t, int32_t> trimat_to_global;
			for (auto& triangle : geometry.triangles) {
				if (trimat_to_global.count(triangle.material_id) == 0) {
					mesh.materials.push_back(wheel_mesh.materials[triangle.material_id]);
					mesh.materials.back().index = mesh.materials.size() - 1;
					trimat_to_global[triangle.material_id] = mesh.materials.size() - 1;
				}
				triangle.material_id = trimat_to_global[triangle.material_id];
			}

			gta_to_ue::Geometry r_geometry(geometry);
			gta_to_ue::Geometry l_geometry(geometry);

			for (int32_t i = 0; i < geometry.vertices.size(); i++) {
				r_geometry.vertices[i].x *= converting_options.wheel_scale;
				r_geometry.vertices[i].y *= converting_options.wheel_scale;
				r_geometry.vertices[i].z *= converting_options.wheel_scale;

				l_geometry.vertices[i].x *= converting_options.wheel_scale;
				l_geometry.vertices[i].y *= -converting_options.wheel_scale;
				l_geometry.vertices[i].z *= converting_options.wheel_scale;
			}

			if (wheel_rf_dummy != -1) {
				r_geometry.frame_id = wheel_rf_dummy;
				mesh.geometries.push_back(r_geometry);
			}

			if (wheel_rb_dummy != -1) {
				r_geometry.frame_id = wheel_rb_dummy;
				mesh.geometries.push_back(r_geometry);
			}

			if (wheel_rm_dummy != -1) {
				r_geometry.frame_id = wheel_rm_dummy;
				mesh.geometries.push_back(r_geometry);
			}

			if (wheel_lm_dummy != -1) {
				l_geometry.frame_id = wheel_lm_dummy;
				mesh.geometries.push_back(l_geometry);
			}

			if (wheel_lf_dummy != -1) {
				l_geometry.frame_id = wheel_lf_dummy;
				mesh.geometries.push_back(l_geometry);
			}

			if (wheel_lb_dummy != -1) {
				l_geometry.frame_id = wheel_lb_dummy;
				mesh.geometries.push_back(l_geometry);
			}

			break;
		}
	}
}

void gta_to_ue::build_car(Mesh& mesh)
{
	clear_geometry_and_frames(mesh);
	build_hierarchy(mesh);
}
