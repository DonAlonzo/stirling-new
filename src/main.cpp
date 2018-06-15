#include "deleter.hpp"
#include "file.hpp"
#include "vulkan.hpp"
#include "window.hpp"

#include <vulkan/vulkan.h>

#include <GLFW/glfw3.h>

#include <algorithm>
#include <iostream>
#include <vector>

int main() {
    try {
        const uint32_t width = 800;
        const uint32_t height = 600;

        // Create window
        const Window window{width, height, "Stirling Engine"};

        // Set enabled extensions
        const auto enabled_extensions = window.get_required_instance_extensions();
        //enabled_extensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);    

        // Create instance
        const auto instance = vulkan_create_instance({
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,

            // Application info
            .pApplicationInfo = Wrapper<VkApplicationInfo>{{
                .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                .pNext              = nullptr,
                .pApplicationName   = "Stirling Engine Demo",
                .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
                .pEngineName        = "Stirling Engine",
                .engineVersion      = VK_MAKE_VERSION(1, 0, 0),
                .apiVersion         = VK_API_VERSION_1_0
            }},

            // Enabled layers
            .enabledLayerCount       = 0,
            .ppEnabledLayerNames     = nullptr,

            // Enabled extensions
            .enabledExtensionCount   = enabled_extensions.size(),
            .ppEnabledExtensionNames = enabled_extensions.data()
        });

        // Create window surface
        const auto surface = vulkan_create_surface(instance, window);

        // Get physical devices
        const auto physical_devices = vulkan_get_physical_devices(instance);

        // Just pick the first physical device god damn it
        const auto physical_device = physical_devices[0];

        // Get queue families on physical device
        const auto queue_families = vulkan_get_queue_families(physical_device);

        // Find queue family indices
        uint32_t graphics_queue_family_index = -1;
        uint32_t present_queue_family_index = -1;
        for (uint32_t i = 0; i < queue_families.size(); ++i) {
            if (queue_families[i].queueCount > 0) {
                // Check if graphics queue
                if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                    graphics_queue_family_index = i;
                }

                // Check if present queue
                if (vulkan_get_surface_present_support(physical_device, i, surface)) {
                    present_queue_family_index = i;
                }
            }
        }
        if (graphics_queue_family_index == -1 || present_queue_family_index == -1) {
            throw "Failed to find all queue families.";
        }

        // Prioritize queue families
        const float queue_priorities[] = { 1.0f };

        // Create device
        const auto device = vulkan_create_device({
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,

            // Queues
            .queueCreateInfoCount    = 2,
            .pQueueCreateInfos       = std::vector<VkDeviceQueueCreateInfo> {
                // Graphics queue
                {
                    .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                    .pNext            = nullptr,
                    .flags            = 0,
                    .queueFamilyIndex = graphics_queue_family_index,
                    .queueCount       = 1,
                    .pQueuePriorities = queue_priorities
                },
                // Present queue
                {
                    .sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                    .pNext            = nullptr,
                    .flags            = 0,
                    .queueFamilyIndex = present_queue_family_index,
                    .queueCount       = 1,
                    .pQueuePriorities = queue_priorities
                }
            }.data(),

            // Enabled layers
            .enabledLayerCount   = 0,
            .ppEnabledLayerNames = nullptr,

            // Enabled extensions
            .enabledExtensionCount   = 0,
            .ppEnabledExtensionNames = nullptr,

            // Enabled features
            .pEnabledFeatures        = std::vector<VkPhysicalDeviceFeatures> {{
            }}.data()
        }, physical_device);

        // Retrieve queue handles
        const auto graphics_queue = vulkan_get_queue(device, graphics_queue_family_index, 0);
        const auto present_queue = vulkan_get_queue(device, present_queue_family_index, 0);
        
        // Get supported surface formats
        const auto surface_formats = vulkan_get_surface_formats(physical_device, surface);

        // Get supported presentation modes
        const auto present_modes = vulkan_get_surface_present_modes(physical_device, surface);

        // Get surface capabilities
        const auto surface_capabilities = vulkan_get_surface_capabilities(physical_device, surface);

        // Get swap images count
        const uint32_t swap_image_count = surface_capabilities.maxImageCount > 0
            ? std::min(surface_capabilities.minImageCount + 1, surface_capabilities.maxImageCount)
            : surface_capabilities.minImageCount + 1;

        // Get swap surface format
        const auto surface_format = [&]() -> VkSurfaceFormatKHR {
            if (surface_formats.size() == 1 && surface_formats[0].format == VK_FORMAT_UNDEFINED) {
                return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
            }

            for (const auto& surface_format : surface_formats) {
                if (surface_format.format == VK_FORMAT_B8G8R8A8_UNORM &&
                    surface_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                    return surface_format;
                }
            }

            return surface_formats[0];
        }();

        // Get swap surface extent
        const auto surface_extent = vulkan_get_surface_extent(surface_capabilities, width, height);

        // Create swapchain
        const uint32_t queue_family_indices[] = { graphics_queue_family_index, present_queue_family_index };
        const bool concurrent = graphics_queue_family_index != present_queue_family_index;
        const auto swapchain = vulkan_create_swapchain({
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,

            .surface               = surface,
            .minImageCount         = swap_image_count,
            .imageFormat           = surface_format.format,
            .imageColorSpace       = surface_format.colorSpace,
            .imageExtent           = surface_extent,
            .imageArrayLayers      = 1,
            .imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .imageSharingMode      = concurrent ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = concurrent ? 2 : 0,
            .pQueueFamilyIndices   = concurrent ? queue_family_indices : nullptr,
        }, device);

        // Get swapchain images
        const auto swapchain_images = vulkan_get_swapchain_images(device, swapchain);

        // Create swapchain image views
        std::vector<Deleter<VkImageView>> swapchain_image_views{swapchain_images.size()};
        for (size_t i = 0; i < swapchain_images.size(); ++i) {
            swapchain_image_views[i] = vulkan_create_image_view({
                .sType      = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .pNext      = nullptr,
                .flags      = 0,

                .image      = swapchain_images[i],
                .viewType   = VK_IMAGE_VIEW_TYPE_2D,
                .format     = surface_format.format,
                .components = {
                    .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .a = VK_COMPONENT_SWIZZLE_IDENTITY
                },
                .subresourceRange = {
                    .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel   = 0,
                    .levelCount     = 1,
                    .baseArrayLayer = 0,
                    .layerCount     = 1
                }
            }, device);
        }

        // Create render pass
        const auto render_pass = vulkan_create_render_pass({
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,

            // Attachments
            .attachmentCount = 1,
            .pAttachments    = std::vector<VkAttachmentDescription>{{
                .flags = 0,

                .format         = surface_format.format,
                .samples        = VK_SAMPLE_COUNT_1_BIT,
                .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
            }}.data(),

            // Subpasses
            .subpassCount    = 1,
            .pSubpasses      = std::vector<VkSubpassDescription>{{
                .flags             = 0,

                // Pipeline bind point
                .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,

                // Input attachments
                .inputAttachmentCount = 0,
                .pInputAttachments    = nullptr,

                // Color attachments
                .colorAttachmentCount = 1,
                .pColorAttachments    = std::vector<VkAttachmentReference>{{
                    .attachment = 0,
                    .layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
                }}.data(),

                // Resolve attachments
                .pResolveAttachments = nullptr,

                // Depth stencil attachments
                .pDepthStencilAttachment = nullptr,

                // Preserve attachments
                .preserveAttachmentCount = 0,
                .pPreserveAttachments    = nullptr
            }}.data(),

            // Dependencies
            .dependencyCount = 0,
            .pDependencies   = nullptr
        }, device);

        // Create pipeline layout
        const auto pipeline_layout = vulkan_create_pipeline_layout({
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,

            // Set layouts
            .setLayoutCount = 0,
            .pSetLayouts    = nullptr,

            // Push constant ranges
            .pushConstantRangeCount = 0,
            .pPushConstantRanges    = nullptr
        }, device);

        // Create pipelines
        const auto pipeline = vulkan_create_pipelines({
            // Pipeline 1
            {
                .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,

                // Stages
                .stageCount = 2,
                .pStages    = std::vector<VkPipelineShaderStageCreateInfo>{
                    // Vertex shader stage
                    {
                        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                        .pNext = nullptr,
                        .flags = 0,

                        .stage               = VK_SHADER_STAGE_VERTEX_BIT,
                        .module              = vulkan_create_shader_module("vert.spv", device),
                        .pName               = "main",
                        .pSpecializationInfo = nullptr
                    },
                    // Fragment shader stage
                    {
                        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                        .pNext = nullptr,
                        .flags=  0,

                        .stage               = VK_SHADER_STAGE_FRAGMENT_BIT,
                        .module              = vulkan_create_shader_module("frag.spv", device),
                        .pName               = "main",
                        .pSpecializationInfo = nullptr
                    }
                }.data(),

                // Vertex input
                .pVertexInputState = Wrapper<VkPipelineVertexInputStateCreateInfo>{{
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = 0,

                    // Vertex binding descriptions
                    .vertexBindingDescriptionCount   = 0,
                    .pVertexBindingDescriptions      = nullptr,

                    // Vertex attribute
                    .vertexAttributeDescriptionCount = 0,
                    .pVertexAttributeDescriptions    = nullptr
                }},

                // Input assembly
                .pInputAssemblyState = Wrapper<VkPipelineInputAssemblyStateCreateInfo>{{
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = 0,

                    .topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                    .primitiveRestartEnable = VK_FALSE
                }},

                // Tessellation
                .pTessellationState = nullptr,

                // Viewport
                .pViewportState = Wrapper<VkPipelineViewportStateCreateInfo>{{
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = 0,

                    // Viewports
                    .viewportCount = 1,
                    .pViewports    = std::vector<VkViewport>{
                        {
                            .x        = 0.0f,
                            .y        = 0.0f,
                            .width    = static_cast<float>(surface_extent.width),
                            .height   = static_cast<float>(surface_extent.height),
                            .minDepth = 0.0f,
                            .maxDepth = 1.0f
                        }
                    }.data(),

                    // Scissors
                    .scissorCount = 1,
                    .pScissors    = std::vector<VkRect2D>{
                        {
                            .offset = { 0, 0 },
                            .extent = surface_extent
                        }
                    }.data()
                }},

                // Rasterization
                .pRasterizationState = Wrapper<VkPipelineRasterizationStateCreateInfo>{{
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = 0,

                    .depthClampEnable        = VK_FALSE,
                    .rasterizerDiscardEnable = VK_FALSE,
                    .polygonMode             = VK_POLYGON_MODE_FILL,
                    .cullMode                = VK_CULL_MODE_BACK_BIT,
                    .frontFace               = VK_FRONT_FACE_CLOCKWISE,
                    .depthBiasEnable         = VK_FALSE,
                    .depthBiasConstantFactor = 0.0f,
                    .depthBiasClamp          = 0.0f,
                    .depthBiasSlopeFactor    = 0.0f,
                    .lineWidth               = 1.0f
                }},

                // Multisampler
                .pMultisampleState = Wrapper<VkPipelineMultisampleStateCreateInfo>{{
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = 0,

                    .rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT,
                    .sampleShadingEnable   = VK_FALSE,
                    .minSampleShading      = 1.0f,
                    .pSampleMask           = nullptr,
                    .alphaToCoverageEnable = VK_FALSE,
                    .alphaToOneEnable      = VK_FALSE
                }},

                // Depth stencil
                .pDepthStencilState = nullptr,

                // Color blending
                .pColorBlendState = Wrapper<VkPipelineColorBlendStateCreateInfo>{{
                    .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = 0,

                    .logicOpEnable = VK_FALSE,
                    .logicOp       = VK_LOGIC_OP_COPY,

                    // Attachments
                    .attachmentCount = 1,
                    .pAttachments    = std::vector<VkPipelineColorBlendAttachmentState>{{
                        .blendEnable         = VK_FALSE,
                        .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
                        .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
                        .colorBlendOp        = VK_BLEND_OP_ADD,
                        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
                        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
                        .alphaBlendOp        = VK_BLEND_OP_ADD,
                        .colorWriteMask      = VK_COLOR_COMPONENT_R_BIT
                                             | VK_COLOR_COMPONENT_G_BIT
                                             | VK_COLOR_COMPONENT_B_BIT
                                             | VK_COLOR_COMPONENT_A_BIT
                    }}.data(),

                    // Blend constants
                    .blendConstants = { 0.0f, 0.0f, 0.0f, 0.0f }
                }},

                // Dynamic state
                .pDynamicState = nullptr,

                .layout              = pipeline_layout,
                .renderPass          = render_pass,
                .subpass             = 0,
                .basePipelineHandle  = VK_NULL_HANDLE,
                .basePipelineIndex   = -1
            }
        }, VK_NULL_HANDLE, device);

        // Drawing > Framebuffers

        // Loop
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    } catch (const char* message) {
        std::cout << message << '\n';
    }
    return 0;
}