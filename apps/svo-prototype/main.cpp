
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>

#include <string>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <set>

#define VK_CHECK(x) \
    do { \
        VkResult result = (x); \
        if (result != VK_SUCCESS) \
            throw std::runtime_error("Vulkan error: " + std::to_string(result)); \
    } while (0)

int main() {
    // --------------------------------
    // SDL
    // --------------------------------

    if (!SDL_Init(SDL_INIT_VIDEO))
        throw std::runtime_error(SDL_GetError());

    SDL_Window* window = SDL_CreateWindow(
        "Voxel Engine",
        1600,
        900,
        SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE
    );

    if (!window)
        throw std::runtime_error(SDL_GetError());

    // --------------------------------
    // Vulkan instance
    // --------------------------------

    uint32_t extensionCount = 0;

    const char* const* extensions =
        SDL_Vulkan_GetInstanceExtensions(&extensionCount);

    if (!extensions)
        throw std::runtime_error(SDL_GetError());

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Voxel Engine";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Voxel Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo instanceInfo{};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.enabledExtensionCount = extensionCount;
    instanceInfo.ppEnabledExtensionNames = extensions;

    VkInstance instance;

    VK_CHECK(vkCreateInstance(
        &instanceInfo,
        nullptr,
        &instance
    ));

    // --------------------------------
    // Vulkan surface
    // --------------------------------

    VkSurfaceKHR surface;

    if (!SDL_Vulkan_CreateSurface(
        window,
        instance,
        nullptr,
        &surface
    ))
        throw std::runtime_error(SDL_GetError());

    // --------------------------------
    // Physical device
    // --------------------------------

    uint32_t deviceCount = 0;

    VK_CHECK(vkEnumeratePhysicalDevices(
        instance,
        &deviceCount,
        nullptr
    ));

    if (deviceCount == 0)
        throw std::runtime_error("No Vulkan GPUs found.");

    std::vector<VkPhysicalDevice> devices(deviceCount);

    VK_CHECK(vkEnumeratePhysicalDevices(
        instance,
        &deviceCount,
        devices.data()
    ));

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    uint32_t graphicsFamily = UINT32_MAX;
    uint32_t presentFamily = UINT32_MAX;

    for (auto device : devices) {
        uint32_t familyCount = 0;

        vkGetPhysicalDeviceQueueFamilyProperties(
            device,
            &familyCount,
            nullptr
        );

        std::vector<VkQueueFamilyProperties> families(familyCount);

        vkGetPhysicalDeviceQueueFamilyProperties(
            device,
            &familyCount,
            families.data()
        );

        uint32_t graphics = UINT32_MAX;
        uint32_t present = UINT32_MAX;

        for (uint32_t i = 0; i < familyCount; ++i)
        {
            if (families[i].queueCount == 0)
                continue;

            if (families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                graphics = i;

            VkBool32 supportsPresent = false;

            VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(
                device,
                i,
                surface,
                &supportsPresent
            ));

            if (supportsPresent)
                present = i;
        }

        if (graphics != UINT32_MAX &&
            present != UINT32_MAX)
        {
            physicalDevice = device;
            graphicsFamily = graphics;
            presentFamily = present;
            break;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE)
        throw std::runtime_error("No suitable Vulkan GPU found.");

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);

    std::cout << "GPU: " << properties.deviceName << '\n';

    // --------------------------------
    // Logical device
    // --------------------------------

    float priority = 1.0f;

    std::set<uint32_t> uniqueFamilies = {
        graphicsFamily,
        presentFamily
    };

    std::vector<VkDeviceQueueCreateInfo> queues;

    for (uint32_t family : uniqueFamilies)
    {
        VkDeviceQueueCreateInfo queueInfo{};
        queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfo.queueFamilyIndex = family;
        queueInfo.queueCount = 1;
        queueInfo.pQueuePriorities = &priority;

        queues.push_back(queueInfo);
    }

    const char* deviceExtensions[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    VkPhysicalDeviceFeatures features{};

    VkDeviceCreateInfo deviceInfo{};
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.queueCreateInfoCount =
        static_cast<uint32_t>(queues.size());
    deviceInfo.pQueueCreateInfos = queues.data();
    deviceInfo.pEnabledFeatures = &features;
    deviceInfo.enabledExtensionCount = 1;
    deviceInfo.ppEnabledExtensionNames = deviceExtensions;

    VkDevice device;

    VK_CHECK(vkCreateDevice(
        physicalDevice,
        &deviceInfo,
        nullptr,
        &device
    ));

    VkQueue graphicsQueue;
    VkQueue presentQueue;

    vkGetDeviceQueue(device, graphicsFamily, 0, &graphicsQueue);
    vkGetDeviceQueue(device, presentFamily, 0, &presentQueue);

    std::cout << "Vulkan initialized.\n";
    std::cout << "SDL3 initialized.\n";
    std::cout << "Window running.\n";

    // --------------------------------
    // Main loop
    // --------------------------------

    bool running = true;
    SDL_Event event;

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = false;

            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
                running = false;
        }
    }

    // --------------------------------
    // Shutdown
    // --------------------------------

    vkDeviceWaitIdle(device);

    vkDestroyDevice(device, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
