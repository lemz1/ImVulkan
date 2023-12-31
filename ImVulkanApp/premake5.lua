project "ImVulkanApp"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"

    files 
    { 
        "**.h", 
        "**.hpp",
        "**.c" ,
        "**.cpp",
        "**.vert",
        "**.frag"
    }

    includedirs 
    {
        "src",
        "../ImVulkan/src",
        "../vendor/glfw/glfw/include",
        "../vendor/glm/glm",
        "../vendor/imgui/imgui"
    }

    links 
    {
        "ImVulkan"
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

    filter "files:**.vert"
        buildmessage 'Compiling %{file.abspath}'
        buildcommands 
        {
            'glslc.exe -fshader-stage=vert "%{file.abspath}" -o "%{file.abspath}.spv"'
        }
        buildoutputs('%{file.abspath}.spv')

    filter "files:**.frag"
        buildmessage 'Compiling %{file.abspath}'
        buildcommands 
        {
            'glslc.exe -fshader-stage=frag "%{file.abspath}" -o "%{file.abspath}.spv"'
        }
        buildoutputs('%{file.abspath}.spv')

