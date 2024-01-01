include "ImVulkanLibraries.lua" -- libraries

startprj = "ImVulkanApp"

workspace "ImVulkan"
    architecture "x64"
    startproject "%{startprj}"
    configurations { "Debug", "Release" }

    targetdir("%{wks.location}/bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}")
    objdir("%{wks.location}/bin-int/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}")

    defines {  }

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


include "ImVulkanExternal.lua"
include "ImVulkanApp"
