include "ImVulkanLibraries.lua" -- libraries

workspace "ImVulkan"
    architecture "x64"
    startproject "ImVulkanApp"
    configurations { "Debug", "Release" }

    defines { "IMVK_PLATFORM_GLFW" }

    filter "system:linux"
        defines { "IMVK_PLATFORM_LINUX" }

    filter "system:windows"
        defines { "IMVK_PLATFORM_WINDOWS" }

    targetdir("%{wks.location}/bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}")
    objdir("%{wks.location}/bin-int/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}")


include "ImVulkanExternal.lua"
include "ImVulkanApp"
