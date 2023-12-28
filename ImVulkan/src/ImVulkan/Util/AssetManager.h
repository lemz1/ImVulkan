#pragma once

#include <string>

namespace ImVulkan
{
	class AssetManager
	{
	public:
		static std::string GetFileContent(const char* filePath);
	};
}
