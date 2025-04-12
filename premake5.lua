workspace "rpr"
    architecture "x64"

    configurations {
        "Debug", 
        "Release"
    }

outputdir = "%{cfg.buildcfg}"

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
    }   

    libdirs {
        "%{prj.name}/Libraries/lib"
    }

    links {
        "gdi32",
        "user32",
        "kernel32",
        "shell32",
        "glfw3_mt",
        "opengl32",
        -- "assimp-vc143-mt"
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
        "engine/src"
    }   

    libdirs {
        "bin/"
    }

    links {
        "engine",
        "user32"
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
