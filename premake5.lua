workspace "ImVulkan"
    architecture "x64"
    startproject "ImVulkanApp"
    configurations { "Debug", "Release" }

    targetdir("%{wks.location}/bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}")
    objdir("%{wks.location}/bin-int/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}")

    filter "system:linux"
        defines 
        { 
            "IMVK_PLATFORM_LINUX", 
            "IMVK_PLATFORM_GLFW"
        }

    filter "system:windows"
        defines 
        { 
            "IMVK_PLATFORM_WINDOWS", 
            "IMVK_PLATFORM_GLFW"
        }

    filter {}

    --[[ uncomment this if you want to use headless
        defines { "IMVK_HEADLESS" }
    
        removedefines { "IMVK_PLATFORM_GLFW" }
    ]]

include "ImVulkanExternal.lua"

include "ImVulkanApp"
