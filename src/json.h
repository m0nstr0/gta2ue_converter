#pragma once

#include <string>
#include "common.h"

namespace gta_to_ue {
    namespace json {
        bool export_to_file(const std::string& file_name, const gta_to_ue::Mesh& mesh_data);
    }
}
