#pragma once
#include "common.h"


namespace gta_to_ue {

	void mixin_car_wheel(const ConvertingOptions& converting_options, gta_to_ue::Mesh& mesh, gta_to_ue::Mesh& wheel_mesh);
	void build_car(gta_to_ue::Mesh& mesh);
}