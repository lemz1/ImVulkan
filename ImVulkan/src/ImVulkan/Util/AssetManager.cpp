#include "imvkpch.h"
#include "ImVulkan/Util/AssetManager.h"

#include "ImVulkan/Core/Core.h"

namespace ImVulkan
{
    std::string AssetManager::GetFileContent(const char* filePath)
    {
        std::ifstream file(filePath, std::ios::binary);

        IMVK_ASSERT(!file.is_open(), "Couldn't find file!");

        std::string content;
        file.seekg(0, std::ios::end);
        content.resize(file.tellg());
        file.seekg(0, std::ios::beg);
        file.read(&content[0], content.size());
        file.close();

        return content;
    }
}
