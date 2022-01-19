#include <iostream>
#include <filesystem>
#include <fstream>
#include <cxxopts.hpp>
#include "common.h"
#include "dff.h"
#include "json.h"

bool init_rw()
{
    rw::platform = rw::PLATFORM_GL3;
    if (!rw::Engine::init()) {
        return false;
    }

    gta::attachPlugins();

    if (!rw::Engine::open(nil)) {
        return false;
    }

    if (!rw::Engine::start()) {
        return false;
    }

    rw::Texture::setLoadTextures(false);
    rw::Texture::setCreateDummies(true);

    return true;
}

int main(int argc, char** argv)
{
    cxxopts::Options options("gta2ue_converter", "GTA 3 & GTA VC DFF files converter");

    options.custom_help("[-h|--help] [-d|--dff <dff file> [--car [--wheels <wheels file> [--wheel-id <wheel-id>] [--wheel-scale <float>]] -o|--output <output file>]");

    std::string input_dff_file;
    std::string input_wheels_file;
    std::string output_file;
    float wheel_scale;
    int32_t wheel_id;
    ConvertingOptions converting_options;

    options.add_options()
        ("h,help", "print usage")
        ("d,dff", "input *.dff file", cxxopts::value(input_dff_file))
        ("o,output", "output *.dffjson file", cxxopts::value(output_file))
        ("wheels", "DFF file with wheels", cxxopts::value(input_wheels_file))
        ("wheel-id", "wheel id", cxxopts::value(wheel_id))
        ("wheel-scale", "wheel scale", cxxopts::value(wheel_scale))
        ("car", "DFF is a car");

    options.allow_unrecognised_options();

    const auto result = options.parse(argc, argv);

	if (result.count("car")) {
        converting_options.is_car = true;
	}

	if (result.count("wheels")) {
		converting_options.wheels_dff = input_wheels_file;
	}

    if (result.count("wheel-id")) {
		converting_options.wheel_id = wheel_id;
	}

	if (result.count("wheel-scale")) {
        converting_options.wheel_scale = wheel_scale;
	}

    if (result.count("help")) {
        std::cout << options.help() << std::endl;
        return 0;
    }

    if (input_dff_file.empty()) {
        std::cout << "must specify input file, use -h to print usage" << std::endl;
        return 1;
    }

    if (output_file.empty()) {
        const std::filesystem::path path = std::filesystem::path(input_dff_file);
        const std::string ext = path.has_extension() ? path.extension().string() : "";
        if (!ext.empty()) {
            output_file = input_dff_file.substr(0, input_dff_file.length() - ext.length()) + ".dffjson";
        }
    }

    std::cout << "input: " << input_dff_file << std::endl;
    std::cout << "output: " << output_file << std::endl;

    if (!init_rw()) {
        std::cout << "rw engine initialization error" << std::endl;
        return 1;
    }
    
    gta_to_ue::Mesh mesh;
    if (rw::Clump* clump = gta_to_ue::dff::parse(input_dff_file, converting_options, mesh)) {
        if (!gta_to_ue::json::export_to_file(output_file, mesh)) {
            std::cout << "saving error" << std::endl;
            return 1;
        }
        return 0;
    }

    std::cout << "parsing error" << std::endl;
    return 1;
}
