project "ImVulkan"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"

    pchheader "imvkpch.h"
    pchsource "src/imvkpch.cpp"

    files 
    { 
        "**.h", 
        "**.hpp",
        "**.c" ,
        "**.cpp"
    }

    includedirs 
    {
        "src",
        IncludeDir.glfw,
        IncludeDir.glm,
        IncludeDir.imgui,
        IncludeDir.VulkanSDK
    }

    links 
    {
        "GLFW",
        "ImGui",
        Library.Vulkan
    }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"