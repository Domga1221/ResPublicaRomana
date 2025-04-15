project "imgui"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"

    -- outputdir defined in parent dir premake file 
    -- currentDir defined in parent dir premake file 
    targetdir (currentDir .. "/bin/" .. outputdir)
    objdir (currentDir .. "/bin-int/" .. outputdir .. "/%{prj.name}")
    symbolspath (currentDir .. "/bin/" .. outputdir .. "/%{prj.name}.pdb")

    files {
        "*.h",
        "*.cpp",
    }

    includedirs {
        "%{currentDir}/engine/Libraries/include"
    }
   

