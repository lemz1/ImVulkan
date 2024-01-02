#pragma once

#if !defined(IMVK_PLATFORM_WINDOWS) && !defined(IMVK_PLATFORM_LINUX)
#error Only supports Windows and Linux!
#endif

#if !defined(IMVK_PLATFORM_GLFW) && !defined(IMVK_PLATFORM_WINDOWS)
#error Only supports GLFW and Win32!
#endif

#include <memory>
#include <iostream>
#include <functional>

#define IMVK_INFO(message) std::cout << "\033[32m[" << __FILE__ << ":" << __LINE__ << "]: " << message << "\033[0m" << std::endl;
#define IMVK_WARN(message) std::cout << "\033[33m[" << __FILE__ << ":" << __LINE__ << "]: " << message << "\033[0m" << std::endl;
#define IMVK_ERROR(message) std::cout << "\033[31m[" << __FILE__ << ":" << __LINE__ << "]: " << message << "\033[0m" << std::endl;

#define IMVK_ASSERT(condition, message) \
	if (condition) \
	{ \
		IMVK_ERROR("Assertion failed: (" #condition "), function " << __func__ << "\nMessage: " << message); \
		std::terminate(); \
	}

#define IMVK_ASSERT_INVERSE(condition, message) IMVK_ASSERT(!(condition), message)

#define Flag(s) (1 << s)

#define BindFunction(funcPtr, objPtr) std::bind(funcPtr, objPtr, std::placeholders::_1)

template <typename T>
decltype(auto) Move(T && arg)
{
	return std::move(arg);
}

// ---------------- smart pointers --------------------------------
template<typename T>
using Shared = std::shared_ptr<T>;

template<typename T, typename... Args>
constexpr Shared<T> MakeShared(Args&&... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

template<typename T>
using Unique = std::unique_ptr<T>;

template<typename T, typename... Args>
constexpr Unique<T> MakeUnique(Args&&... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T>
using Weak = std::weak_ptr<T>;
// ----------------------------------------------------------------
