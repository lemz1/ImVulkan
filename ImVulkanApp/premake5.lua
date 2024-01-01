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
