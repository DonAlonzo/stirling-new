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

VkBool32 debug_callback(
    const auto flags, 
    const auto object_type, 
    const auto object, 
    const auto location, 
    const auto code, 
    const auto layer_prefix, 
    const auto message, 
    const auto user_data) {

    std::cerr << "\033[1;31m[stirling]\033[0m " << message << '\n';
    return VK_FALSE;
}

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
            .application_info = {
                .application_name    = "Stirling Engine Demo",
                .application_version = VK_MAKE_VERSION(1, 0, 0),
                .engine_name         = "Stirling Engine",
                .engine_version      = VK_MAKE_VERSION(1, 0, 0),
                .api_version         = VK_API_VERSION_1_0
            },
            .enabled_layers = {
                "VK_LAYER_LUNARG_standard_validation"
            },
            .enabled_extensions = enabled_extensions
        });

        // Create debug report callback
        const auto debug_callback_handle = vulkan_create_debug_report_callback({
            .flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT,
            .callback = debug_callback,
            .user_data = nullptr
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
            .queues = [&queue_families]() {
                std::vector<VulkanDeviceQueueCreateInfo> create_infos;
                // Only one queue per unique queue family index
                for (const auto queue_family : std::set<uint32_t>{
                    queue_families.graphics_queue,
                    queue_families.present_queue
                }) {
                    create_infos.push_back({
                        .queue_family_index = queue_families.graphics_queue,
                        .queue_priorities   = { 1.0f }
                    });
                }
                return create_infos;
            }(),
            .enabled_layers = {},
            .enabled_extensions = {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME    
            },
            .enabled_features = {}
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
        const bool concurrent = queue_families.graphics_queue != queue_families.present_queue;
        const auto swapchain = vulkan_create_swapchain({
            .surface              = surface,
            .min_image_count      = swap_image_count,
            .image_format         = surface_format.format,
            .image_color_space    = surface_format.colorSpace,
            .image_extent         = surface_extent,
            .image_array_layers   = 1,
            .image_usage          = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .image_sharing_mode   =
                concurrent ?
                    VK_SHARING_MODE_CONCURRENT :
                    VK_SHARING_MODE_EXCLUSIVE,
            .queue_family_indices =
                concurrent ?
                    std::vector<uint32_t>{
                        queue_families.graphics_queue,
                        queue_families.present_queue
                    } :
                    std::vector<uint32_t>{},
            .pre_transform        = surface_capabilities.currentTransform,
            .composite_alpha      = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .present_mode         = present_mode,
            .clipped              = VK_TRUE,
            .old_swapchain        = VK_NULL_HANDLE
        }, device);

        // Create swapchain image views
        const auto swapchain_image_views = [&]() {
            // Get swapchain images
            const auto swapchain_images = vulkan_get_swapchain_images(device, swapchain);
            
            std::vector<Deleter<VkImageView>> swapchain_image_views{swapchain_images.size()};
            for (size_t i = 0; i < swapchain_images.size(); ++i) {
                swapchain_image_views[i] = vulkan_create_image_view({
                    .image      = swapchain_images[i],
                    .view_type  = VK_IMAGE_VIEW_TYPE_2D,
                    .format     = surface_format.format,
                    .components = {
                        .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                        .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                        .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                        .a = VK_COMPONENT_SWIZZLE_IDENTITY
                    },
                    .subresource_range = {
                        .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                        .baseMipLevel   = 0,
                        .levelCount     = 1,
                        .baseArrayLayer = 0,
                        .layerCount     = 1
                    }
                }, device);
            }
            return swapchain_image_views;
        }();

        // Create render pass
        const auto render_pass = vulkan_create_render_pass({
            .attachments = {
                {
                    .flags = 0,
                    .format         = surface_format.format,
                    .samples        = VK_SAMPLE_COUNT_1_BIT,
                    .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
                    .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
                    .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                    .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                    .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
                    .finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
                }
            },
            .subpasses = {
                {
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
                }
            },
            .dependencies = {
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
            }
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
            .pVertexInputState = Wrapper<VkPipelineVertexInputStateCreateInfo>{{
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

                // Logical operation
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

            // Layout
            .layout = vulkan_create_pipeline_layout({
                .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,

                // Set layouts
                .setLayoutCount = 0,
                .pSetLayouts    = nullptr,

                // Push constant ranges
                .pushConstantRangeCount = 0,
                .pPushConstantRanges    = nullptr
            }, device),

            // Render pass
            .renderPass = render_pass,

            // Subpass
            .subpass = 0,

            // Base pipeline
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex  = -1
        }, VK_NULL_HANDLE, device);

        // Create framebuffers
        std::vector<Deleter<VkFramebuffer>> swapchain_framebuffers{swapchain_image_views.size()};
        for (size_t i = 0; i < swapchain_framebuffers.size(); ++i) {
            swapchain_framebuffers[i] = vulkan_create_framebuffer({
                .render_pass = render_pass,

                .attachments = {
                    swapchain_image_views[i]
                },

                .width  = surface_extent.width,
                .height = surface_extent.height,
                .layers = 1
            }, device);
        }

        // Create command pool
        const auto command_pool = vulkan_create_command_pool({
            .flags              = 0,
            .queue_family_index = queue_families.graphics_queue
        }, device);

        // Allocate command buffers
        const auto command_buffers = vulkan_allocate_command_buffers({
            .command_pool         = command_pool,
            .level                = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .command_buffer_count = swapchain_framebuffers.size()
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
                .pClearValues    = Wrapper<VkClearValue>{
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
                    .wait_semaphores = {
                        image_available_semaphores[current_frame]
                    },

                    .wait_dst_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,

                    .command_buffers = {
                        command_buffers[image_index]
                    },

                    .signal_semaphores = {
                        render_finished_semaphores[current_frame]
                    }
                }
            }, graphics_queue, in_flight_fences[current_frame]);

            // Present images
            vulkan_queue_present({
                .wait_semaphores = {
                    render_finished_semaphores[current_frame]
                },

                .swapchains = {
                    swapchain
                },

                .image_indices = {
                    image_index
                }
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