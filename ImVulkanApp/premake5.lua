project "ImVulkanApp"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"

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
        "../ImVulkan/src",
        IncludeDir.glfw,
        IncludeDir.glm,
        IncludeDir.imgui,
        IncludeDir.VulkanSDK,
        IncludeDir.stb
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

    filter {}
