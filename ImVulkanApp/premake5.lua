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

    -- shaders
    filter "files:**.vert"
    buildmessage 'Compiling %{file.relpath}'
    buildcommands 
    {
        'glslc.exe -fshader-stage=vert "%{file.relpath}" -o "%{file.relpath}.spv"'
    }
    buildoutputs('%{file.relpath}.spv')
    
    filter "files:**.frag"
    buildmessage 'Compiling %{file.relpath}'
    buildcommands 
    {
        'glslc.exe -fshader-stage=frag "%{file.relpath}" -o "%{file.relpath}.spv"'
    }
    buildoutputs('%{file.relpath}.spv')
