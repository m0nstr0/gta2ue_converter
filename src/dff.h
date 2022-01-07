#pragma once

#include <string>
#include "common.h"

namespace gta_to_ue {
    namespace dff {
        rw::Clump* parse(const std::string& dff_file_name, gta_to_ue::Mesh& mesh_data);
    }
}
