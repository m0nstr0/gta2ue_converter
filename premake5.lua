librw = 'vendor/librw'
librwgta = 'vendor/librwgta'
rapidjson = 'vendor/rapidjson'
cxxopts = 'vendor/cxxopts'
glfwdir = 'vendor/glfw'

local function addSrcFiles( prefix )
	return prefix .. "/*cpp", prefix .. "/*.h", prefix .. "/*.c", prefix .. "/*.ico", prefix .. "/*.rc"
end

workspace "gta2ue"
    language "C++"
    configurations {"Debug","Release"}
    platforms {"Win64"}
    startproject "gta2ue"
    location "build"
    symbols "Full"
    staticruntime "off"
    
    libdirs { 
        "lib/%{cfg.system}-%{cfg.architecture}-%{cfg.buildcfg}/librw",
        "lib/%{cfg.system}-%{cfg.architecture}-%{cfg.buildcfg}/librwgta",
        "lib/%{cfg.system}-%{cfg.architecture}-%{cfg.buildcfg}/glfw"
    }

    filter { "platforms:win*" }
        system "Windows"
        architecture "amd64"

	filter "configurations:Debug"
		defines { "DEBUG" }

	filter "configurations:not Debug"
		defines { "NDEBUG" }
		optimize "Speed"

    filter {}

    project "glfw"
        kind "StaticLib"
        targetname "glfw"
        language "C"
        targetdir("lib/%{cfg.system}-%{cfg.architecture}-%{cfg.buildcfg}/glfw")

        files { path.join(glfwdir, "src/context.c") }
        files { path.join(glfwdir, "src/init.c") } 
        files { path.join(glfwdir, "src/input.c") } 
        files { path.join(glfwdir, "src/monitor.c") } 
        files { path.join(glfwdir, "src/vulkan.c") } 
        files { path.join(glfwdir, "src/window.c") }
        files { path.join(glfwdir, "src/internal.h") } 
        files { path.join(glfwdir, "src/mappings.h") }

        includedirs { path.join(glfwdir, "include") }

        filter { "platforms:win*" }
            architecture "amd64"
            staticruntime "off"
            defines { "_GLFW_WIN32" }
            links { "gdi32" }
            files { path.join(glfwdir, "src/win32_platform.h") } 
            files { path.join(glfwdir, "src/win32_joystick.h") }
            files { path.join(glfwdir, "src/wgl_context.h") } 
            files { path.join(glfwdir, "src/egl_context.h") } 
            files { path.join(glfwdir, "src/osmesa_context.h") }
            files { path.join(glfwdir, "src/win32_init.c") } 
            files { path.join(glfwdir, "src/win32_joystick.c") }
            files { path.join(glfwdir, "src/win32_monitor.c") } 
            files { path.join(glfwdir, "src/win32_time.c") } 
            files { path.join(glfwdir, "src/win32_thread.c") } 
            files { path.join(glfwdir, "src/win32_window.c") }
            files { path.join(glfwdir, "src/wgl_context.c") } 
            files { path.join(glfwdir, "src/egl_context.c") } 
            files { path.join(glfwdir, "src/osmesa_context.c") }

        filter {}

    project "librw"
        kind "StaticLib"
        targetname "rw"
        targetdir("lib/%{cfg.system}-%{cfg.architecture}-%{cfg.buildcfg}/librw")
        dependson "glfw"
        defines { "RW_NULL" }

        includedirs { path.join(glfwdir, "include") }

        files { path.join(librw, "src/*.*") }
        files { path.join(librw, "src/*/*.*") }
        files { path.join(librw, "src/gl/*/*.*") }
        
        filter { "platforms:win*" }
            architecture "amd64"
            defines { "_CRT_SECURE_NO_WARNINGS", "_CRT_NONSTDC_NO_DEPRECATE" }
            staticruntime "off"
            buildoptions { "/Zc:sizedDealloc-" }
        
        filter {}
    
    project "librwgta"
        kind "StaticLib"
        targetname "rwgta"
        dependson "librw"
        links { "rw" }
        
        includedirs { librw }
        includedirs { path.join(glfwdir, "include") }

        targetdir("lib/%{cfg.system}-%{cfg.architecture}-%{cfg.buildcfg}/librwgta")
        files { path.join(librwgta, "src/*.*") }

        filter { "platforms:win*" }
            architecture "amd64"

        filter {}

    project "gta2ue"
        kind "ConsoleApp"
        cppdialect "C++20"
        targetextension ".exe"
        targetname "gta2ue"
        targetdir "bin/%{cfg.system}-%{cfg.architecture}-%{cfg.buildcfg}"
        dependson "librwgta"
        staticruntime "off"
        defines { "RWLIBS", "RW_NULL" }
        links { "rw", "librwgta", "opengl32", "glfw" }
        includedirs { librw }
        includedirs { path.join(rapidjson, "include") }
        includedirs { path.join(librwgta, "src") }
        includedirs { path.join(cxxopts, "include") }
        includedirs { path.join(glfwdir, "include") }

        files { addSrcFiles("src") }
     
        filter {}