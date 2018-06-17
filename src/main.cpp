#include "deleter.hpp"
#include "file.hpp"
#include "vulkan.hpp"
#include "window.hpp"

#include <vulkan/vulkan.h>

#include <GLFW/glfw3.h>

#include <algorithm>
#include <iostream>
#include <set>
#include <vector>

int main() {
    try {
        const uint32_t width = 800;
        const uint32_t height = 600;

        // Create window
        const Window window{width, height, "Stirling Engine"};

        // Set enabled extensions
        auto enabled_extensions = window.get_required_instance_extensions();
        enabled_extensions.emplace_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

        // Create instance
        const auto instance = vulkan_create_instance({
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,

            // Application info
            .pApplicationInfo = Pointer<VkApplicationInfo>{{
                .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                .pNext              = nullptr,
                .pApplicationName   = "Stirling Engine Demo",
                .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
                .pEngineName        = "Stirling Engine",
                .engineVersion      = VK_MAKE_VERSION(1, 0, 0),
                .apiVersion         = VK_API_VERSION_1_0
            }},

            // Enabled layers
            .enabledLayerCount   = 1,
            .ppEnabledLayerNames = std::vector<const char*>{
                "VK_LAYER_LUNARG_standard_validation"
            }.data(),

            // Enabled extensions
            .enabledExtensionCount   = enabled_extensions.size(),
            .ppEnabledExtensionNames = enabled_extensions.data()
        });

        // Create debug report callback
        const auto debug_callback_handle = vulkan_create_debug_report_callback({
            .sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
            .pNext = nullptr,
            .flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT,

            // Callback
            .pfnCallback = [](
                const auto flags, 
                const auto object_type, 
                const auto object, 
                const auto location, 
                const auto code, 
                const auto layer_prefix, 
                const auto message, 
                const auto user_data) -> VkBool32 {

                std::cerr << "\033[1;31m[stirling]\033[0m " << message << '\n';
                return VK_FALSE;
            },

            // User data
            .pUserData = nullptr
        }, instance);

        // Create window surface
        const auto surface = vulkan_create_surface(instance, window);

        // Pick physical device
        const auto physical_device = [instance]() {
            for (const auto& physical_device : vulkan_get_physical_devices(instance)) {
                const auto properties = vulkan_get_physical_device_properties(physical_device);
                const auto features = vulkan_get_physical_device_features(physical_device);

                return physical_device;
            }
            throw "Failed to find a suitable GPU.";
        }();

        // Get queue families on physical device
        const auto queue_families = vulkan_get_queue_families(physical_device, surface);

        // Create device
        const auto device = vulkan_create_device({
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,

            // Queues
            .queueCreateInfoCount = [&queue_families]() {
                // Only one queue per unique queue family index
                return std::set<uint32_t>{queue_families.graphics_queue, queue_families.present_queue}.size();
            }(),
            .pQueueCreateInfos = [&queue_families, queue_priorities = std::vector<float>{ 1.0f }]() {
                std::vector<VkDeviceQueueCreateInfo> create_infos;
                // Only one queue per unique queue family index
                for (const auto queue_family : std::set<uint32_t>{
                    queue_families.graphics_queue,
                    queue_families.present_queue
                }) {
                    create_infos.push_back({
                        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                        .pNext = nullptr,
                        .flags = 0,

                        // Queue info
                        .queueFamilyIndex = queue_families.graphics_queue,
                        .queueCount       = 1,
                        .pQueuePriorities = queue_priorities.data()
                    });
                }
                return create_infos;
            }().data(),

            // Enabled layers
            .enabledLayerCount   = 0,
            .ppEnabledLayerNames = nullptr,

            // Enabled extensions
            .enabledExtensionCount   = 1,
            .ppEnabledExtensionNames = std::vector<const char*>{
                VK_KHR_SWAPCHAIN_EXTENSION_NAME    
            }.data(),

            // Enabled features
            .pEnabledFeatures = std::vector<VkPhysicalDeviceFeatures>{{
            }}.data()
        }, physical_device);

        // Retrieve queue handles
        const auto graphics_queue = vulkan_get_queue(device, queue_families.graphics_queue, 0);
        const auto present_queue = vulkan_get_queue(device, queue_families.present_queue, 0);
        
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

        // Get swap present mode
        const auto present_mode = [&present_modes]() {
            auto present_mode = VK_PRESENT_MODE_FIFO_KHR;

            for (const auto& available_present_mode : present_modes) {
                if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR) {
                    return available_present_mode;
                } else if (available_present_mode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
                    present_mode = available_present_mode;
                }
            }

            return present_mode;
        }();

        // Get swap surface extent
        const auto surface_extent = vulkan_get_surface_extent(surface_capabilities, width, height);

        // Create swapchain
        const uint32_t queue_family_indices[] = { queue_families.graphics_queue, queue_families.present_queue };
        const bool concurrent = queue_families.graphics_queue != queue_families.present_queue;
        const auto swapchain = vulkan_create_swapchain({
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,

            .surface       = surface,
            .minImageCount = swap_image_count,

            // Image properties
            .imageFormat      = surface_format.format,
            .imageColorSpace  = surface_format.colorSpace,
            .imageExtent      = surface_extent,
            .imageArrayLayers = 1,
            .imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .imageSharingMode = concurrent ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE,

            // Queue families
            .queueFamilyIndexCount = concurrent ? 2 : 0,
            .pQueueFamilyIndices   = concurrent ? queue_family_indices : nullptr,
            
            .preTransform   = surface_capabilities.currentTransform,
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode    = present_mode,
            .clipped        = VK_TRUE,
            .oldSwapchain   = VK_NULL_HANDLE
        }, device);

        // Get swapchain images
        const auto swapchain_images = vulkan_get_swapchain_images(device, swapchain);

        // Create swapchain image views
        std::vector<Deleter<VkImageView>> swapchain_image_views{swapchain_images.size()};
        for (size_t i = 0; i < swapchain_images.size(); ++i) {
            swapchain_image_views[i] = vulkan_create_image_view({
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,

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
            .subpassCount = 1,
            .pSubpasses   = std::vector<VkSubpassDescription>{{
                .flags = 0,

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
            .dependencyCount = 1,
            .pDependencies   = std::vector<VkSubpassDependency>{
                {
                    // Subpasses
                    .srcSubpass = VK_SUBPASS_EXTERNAL,
                    .dstSubpass = 0,

                    // Stage masks
                    .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                    .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,

                    // Access masks
                    .srcAccessMask = 0,
                    .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,

                    // Flags
                    .dependencyFlags = 0
                }
            }.data()
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
        const auto pipeline = vulkan_create_pipeline({
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
                    .flags = 0,

                    .stage               = VK_SHADER_STAGE_FRAGMENT_BIT,
                    .module              = vulkan_create_shader_module("frag.spv", device),
                    .pName               = "main",
                    .pSpecializationInfo = nullptr
                }
            }.data(),

            // Vertex input
            .pVertexInputState = Pointer<VkPipelineVertexInputStateCreateInfo>{{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,

                // Vertex binding descriptions
                .vertexBindingDescriptionCount = 0,
                .pVertexBindingDescriptions    = nullptr,

                // Vertex attribute
                .vertexAttributeDescriptionCount = 0,
                .pVertexAttributeDescriptions    = nullptr
            }},

            // Input assembly
            .pInputAssemblyState = Pointer<VkPipelineInputAssemblyStateCreateInfo>{{
                .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,

                .topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                .primitiveRestartEnable = VK_FALSE
            }},

            // Tessellation
            .pTessellationState = nullptr,

            // Viewport
            .pViewportState = Pointer<VkPipelineViewportStateCreateInfo>{{
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
            .pRasterizationState = Pointer<VkPipelineRasterizationStateCreateInfo>{{
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
            .pMultisampleState = Pointer<VkPipelineMultisampleStateCreateInfo>{{
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
            .pColorBlendState = Pointer<VkPipelineColorBlendStateCreateInfo>{{
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

            .layout             = pipeline_layout,
            .renderPass         = render_pass,
            .subpass            = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex  = -1
        }, VK_NULL_HANDLE, device);

        // Create framebuffers
        std::vector<Deleter<VkFramebuffer>> swapchain_framebuffers{swapchain_image_views.size()};
        for (size_t i = 0; i < swapchain_framebuffers.size(); ++i) {
            swapchain_framebuffers[i] = vulkan_create_framebuffer({
                .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,

                // Render pass
                .renderPass = render_pass,

                // Attachments
                .attachmentCount = 1,
                .pAttachments    = std::vector<VkImageView>{
                    swapchain_image_views[i]
                }.data(),

                // Dimensions
                .width  = surface_extent.width,
                .height = surface_extent.height,
                .layers = 1
            }, device);
        }

        // Create command pool
        const auto command_pool = vulkan_create_command_pool({
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,

            .queueFamilyIndex = queue_families.graphics_queue
        }, device);

        // Allocate command buffers
        const auto command_buffers = vulkan_allocate_command_buffers({
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,

            .commandPool        = command_pool,
            .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = swapchain_framebuffers.size()
        }, device);

        // Record command buffers
        for (size_t i = 0; i < command_buffers.size(); ++i) {
            // Begin command buffer
            vulkan_begin_command_buffer({
                .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                .pNext = nullptr,
                .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT,
                
                .pInheritanceInfo = nullptr
            }, command_buffers[i]);

            // Begin render pass
            vulkan_begin_render_pass({
                .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
                .pNext = nullptr,

                .renderPass  = render_pass,
                .framebuffer = swapchain_framebuffers[i],
                .renderArea = {
                    .offset = { 0, 0 },
                    .extent = surface_extent
                },

                // Clear values
                .clearValueCount = 1,
                .pClearValues    = Pointer<VkClearValue>{
                    { 0.0f, 0.0f, 0.0f, 1.0f }
                }
            }, command_buffers[i], VK_SUBPASS_CONTENTS_INLINE);

            // Bind pipeline
            vkCmdBindPipeline(command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

            // Draw
            vkCmdDraw(command_buffers[i], 3, 1, 0, 0);

            // End render pass
            vkCmdEndRenderPass(command_buffers[i]);

            // End command buffer
            vulkan_assert(vkEndCommandBuffer(command_buffers[i]), "Failed to record command buffer.");
        }

        // Create semaphores
        constexpr auto max_frames_in_flight = 2;
        const auto image_available_semaphores = vulkan_create_semaphores(device, max_frames_in_flight);
        const auto render_finished_semaphores = vulkan_create_semaphores(device, max_frames_in_flight);
        const auto in_flight_fences = vulkan_create_fences(device, max_frames_in_flight, true);

        // Loop
        size_t current_frame = 0;
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();

            /// Wait for frame to be finished and reset fence
            vulkan_wait_for_fence(device, in_flight_fences[current_frame]);
            vulkan_reset_fence(device, in_flight_fences[current_frame]);

            // Get next image from swapchain
            const auto image_index = vulkan_acquire_next_image(device, swapchain, image_available_semaphores[current_frame]);
            
            // Submit command buffer to graphics queue
            vulkan_queue_submit({
                {
                    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                    .pNext = nullptr,

                    // Wait semaphores
                    .waitSemaphoreCount = 1,
                    .pWaitSemaphores    = std::vector<VkSemaphore>{
                        image_available_semaphores[current_frame]
                    }.data(),
                    .pWaitDstStageMask  = std::vector<VkPipelineStageFlags>{
                        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
                    }.data(),

                    // Command buffers
                    .commandBufferCount = 1,
                    .pCommandBuffers    = &command_buffers[image_index],

                    // Signal semaphores
                    .signalSemaphoreCount = 1,
                    .pSignalSemaphores    = std::vector<VkSemaphore>{
                        render_finished_semaphores[current_frame]
                    }.data()
                }
            }, graphics_queue, in_flight_fences[current_frame]);

            // Present images
            vulkan_queue_present({
                .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                .pNext = nullptr,

                // Wait semaphores
                .waitSemaphoreCount = 1,
                .pWaitSemaphores    = std::vector<VkSemaphore>{
                    render_finished_semaphores[current_frame]
                }.data(),

                // Swapchains
                .swapchainCount = 1,
                .pSwapchains    = std::vector<VkSwapchainKHR>{
                    swapchain
                }.data(),

                // Image indices
                .pImageIndices = &image_index,

                // Results
                .pResults = nullptr
            }, present_queue);

            // Wait until queue is idle
            vkQueueWaitIdle(present_queue);

            // Advance to next frame
            current_frame = (current_frame + 1) % max_frames_in_flight;
        }

        // Wait until device is idle
        vkDeviceWaitIdle(device);
    } catch (const char* message) {
        std::cout << message << '\n';
    }
    return 0;
}