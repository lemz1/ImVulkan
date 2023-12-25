workspace "Vulkin"
    architecture "x64"
    startproject "VulkinApp"
    configurations { "Debug", "Release" }

    filter "system:linux"
        defines { "VULKIN_PLATFORM_LINUX" }

    filter "system:windows"
        defines { "VULKIN_PLATFORM_WINDOWS" }

    targetdir("%{wks.location}/bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}")
    objdir("%{wks.location}/bin-int/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/%{prj.name}")

group "Dependencies"

group ""

group "Core"
    include "VulkinCore"
group ""

include "VulkinApp"
