#pragma once

#include "ImVulkan/Core/Core.h"
#include <vulkan/vulkan.h>

#define VK_ASSERT(value, message) IMVK_ASSERT(value != VK_SUCCESS, "Vulkan: " << message)

#define ARRAY_COUNT(array) (sizeof(array) / sizeof(array[0]))

#define VK_DEBUG_INFO
