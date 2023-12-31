VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"
IncludeDir["glfw"] = "%{wks.location}/vendor/glfw/glfw/include"
IncludeDir["glm"] = "%{wks.location}/vendor/glm/glm"
IncludeDir["imgui"] = "%{wks.location}/vendor/imgui/imgui"
IncludeDir["stb"] = "%{wks.location}/vendor/stb"

LibraryDir = {}
LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"

Library = {}
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"