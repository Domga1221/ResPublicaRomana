workspace "rpr"
    architecture "x64"

    configurations {
        "Debug", 
        "Release"
    }

-- currentDir = os.getcwd()
outputdir = "%{cfg.buildcfg}"

-- TODO: Think about better project structuring
project "imgui-docking"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    symbols "on"

    -- outputdir defined in parent dir premake file 
    -- currentDir defined in parent dir premake file 
    targetdir ("bin/" .. outputdir)
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
    symbolspath ("bin/" .. outputdir .. "/%{prj.name}.pdb")

    files {
        "engine/Libraries/include/imgui-docking/*.h",
        "engine/Libraries/include/imgui-docking/*.cpp",
    }

    includedirs {
        "engine/Libraries/include"
    }
   
    toolset("clang")
    buildoptions {
        "-Wvarargs", "-Wall", "-Werror",
    }
    linkoptions { "-g" }

project "engine"
    location "engine"
    kind "SharedLib"
    language "C++"
    cppdialect "C++17"
    targetdir ("bin/" .. outputdir)
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
    symbolspath ("bin/" .. outputdir .. "/%{prj.name}.pdb")

    files {
        "%{prj.name}/src/**.hpp",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/src/**.c" -- for glad.c
    }

    includedirs {
        "%{prj.name}/src",
        "%{prj.name}/Libraries/include",
        "%{prj.name}/Libraries/include/imgui-docking"
    }   

    libdirs {
        "bin/*",
        "%{prj.name}/Libraries/lib"
    }

    links {
        "imgui-docking",
        "gdi32",
        "user32",
        "kernel32",
        "shell32",
        "glfw3_mt",
        "opengl32",
        "assimp-vc143-mt"
    }

    defines {
        "DEBUG", "RPR_EXPORT"
    }   

    symbols "On"

    -- makefile 
    buildcommands {
        "make %{cfg.buildcfg}"
    }

    cleancommands {
        "make clean %{cfg.buildcfg}"
    }

    -- toolset
    toolset("clang")
    buildoptions {
        "-Wvarargs", "-Wall", "-Werror",
    }
    linkoptions { "-g" }

project "sandbox"
    location "sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"

    targetdir ("bin/" .. outputdir)
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
    symbolspath ("bin/" .. outputdir .. "/%{prj.name}.pdb")

    files {
        "%{prj.name}/src/**.hpp",
        "%{prj.name}/src/**.cpp"
    }

    includedirs {
        "%{prj.name}/src",
        "engine/src",
        "engine/Libraries/include"
    }   

    libdirs {
        "bin/",
        "engine/Libraries/lib"
    }

    -- Visual studio automatically puts engine in References, meaning when building 
    -- sandbox, all the dependencies from engine are used in sandbox as well 
    -- when using gmake + clang, the dependencies need to be included here in sandbox as well 
    -- https://github.com/premake/premake-core/issues/627
    links {
        "engine",
        "imgui-docking",
        "gdi32",
        "user32",
        "kernel32",
        "shell32",
        "glfw3_mt",
        "opengl32",
        "assimp-vc143-mt"
    }

    defines {
        "RPR_IMPORT"
    }

    symbols "On"

    -- makefile 
    buildcommands {
        "make %{cfg.buildcfg}"
    }

    cleancommands {
        "make clean %{cfg.buildcfg}"
    }

    -- toolset
    toolset("clang")
    buildoptions {
        "-Wvarargs", "-Wall", "-Werror"
    }
    linkoptions { "-g" }
