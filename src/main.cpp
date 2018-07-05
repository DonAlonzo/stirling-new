#include "vulkan/buffer.hpp"
#include "vulkan/deleter.hpp"
#include "vulkan/device.hpp"
#include "vulkan/device_memory.hpp"
#include "vulkan/instance.hpp"
#include "vulkan/vulkan.hpp"

#include "file.hpp"
#include "main.hpp"
#include "window.hpp"

#include <vulkan/vulkan.h>

#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <set>
#include <vector>

namespace stirling {

    VkBool32 debug_callback(
        VkDebugReportFlagsEXT      flags,
        VkDebugReportObjectTypeEXT object_type,
        uint64_t                   object,
        size_t                     location,
        int32_t                    message_code,
        const char*                layer_prefix,
        const char*                message,
        void*                      user_data) {

        std::cerr << "\033[1;31m[stirling]\033[0m " << message << '\n';
        return VK_FALSE;
    }

    StirlingInstance::StirlingInstance() {
        const uint32_t width = 1024;
        const uint32_t height = 768;

        // Create window
        const Window window{width, height, "Stirling Engine"};

        // Set enabled extensions
        auto enabled_extensions = window.get_required_instance_extensions();
        enabled_extensions.emplace_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

        // Create instance
        const vulkan::Instance instance{{{
            .application_info = {{
                .application_name    = "Stirling Engine Demo",
                .application_version = VK_MAKE_VERSION(1, 0, 0),
                .engine_name         = "Stirling Engine",
                .engine_version      = VK_MAKE_VERSION(1, 0, 0),
                .api_version         = VK_API_VERSION_1_0
            }},
            .enabled_layers = {
                "VK_LAYER_LUNARG_standard_validation"
            },
            .enabled_extensions = enabled_extensions
        }}};

        // Create debug report callback
        const auto debug_callback_handle = instance.create_debug_report_callback({{
            .flags     = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT,
            .callback  = debug_callback,
        }});

        // Create window surface
        const auto surface = instance.create_surface(window);

        // Pick physical device
        const auto physical_device = [physical_devices = instance.get_physical_devices()]() {
            for (const auto& physical_device : physical_devices) {
                const auto properties = physical_device.get_physical_device_properties();
                const auto features = physical_device.get_physical_device_features();

                return physical_device;
            }
            throw "Failed to find a suitable GPU.";
        }();

        // Get queue families on physical device
        const auto queue_families = physical_device.get_queue_families(surface);

        // Create device
        const vulkan::Device device{{{
            .queues = [&queue_families]() {
                std::vector<vulkan::DeviceQueueCreateInfo> create_infos;
                // Only one queue per unique queue family index
                for (const auto queue_family : std::set<uint32_t>{
                    queue_families.graphics_queue,
                    queue_families.present_queue
                }) {
                    create_infos.push_back({{
                        .queue_family_index = queue_families.graphics_queue,
                        .queue_priorities   = { 1.0f }
                    }});
                }
                return create_infos;
            }(),
            .enabled_extensions = {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME    
            },
            .enabled_features = {
                .geometryShader  = VK_TRUE
            }
        }}, physical_device};

        // Retrieve queue handles
        const auto graphics_queue = vulkan::get_queue(device, queue_families.graphics_queue, 0);
        const auto present_queue = vulkan::get_queue(device, queue_families.present_queue, 0);
        
        // Get supported surface formats
        const auto surface_formats = vulkan::get_surface_formats(physical_device, surface);

        // Get supported presentation modes
        const auto present_modes = vulkan::get_surface_present_modes(physical_device, surface);

        // Get surface capabilities
        const auto surface_capabilities = vulkan::get_surface_capabilities(physical_device, surface);

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
        const auto surface_extent = vulkan::get_surface_extent(surface_capabilities, width, height);

        // Create descriptor set layout
        const auto descriptor_set_layout = vulkan::create_descriptor_set_layout({{
            .bindings = {
                {{
                    .binding         = 0,
                    .descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    .descriptorCount = 1,
                    .stageFlags      = VK_SHADER_STAGE_VERTEX_BIT
                }}
            }
        }}, device);

        // Create pipeline layout
        const auto pipeline_layout = vulkan::create_pipeline_layout({{
            .set_layouts = {
                descriptor_set_layout
            }
        }}, device);

        // Create command pool
        const auto command_pool = device.create_command_pool({{
            .flags              = 0,
            .queue_family_index = queue_families.graphics_queue
        }});

        // Create swapchain
        const bool concurrent = queue_families.graphics_queue != queue_families.present_queue;
        const auto swapchain = vulkan::create_swapchain({{
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
        }}, device);

        // Create swapchain image views
        const auto swapchain_image_views = [&]() {
            // Get swapchain images
            const auto swapchain_images = vulkan::get_swapchain_images(device, swapchain);
            
            std::vector<Deleter<VkImageView>> swapchain_image_views{swapchain_images.size()};
            for (size_t i = 0; i < swapchain_images.size(); ++i) {
                swapchain_image_views[i] = vulkan::create_image_view({{
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
                }}, device);
            }
            return swapchain_image_views;
        }();

        // Create render pass
        const auto render_pass = vulkan::create_render_pass({{
            .attachments = {
                {{
                    .flags            = 0,
                    .format           = surface_format.format,
                    .samples          = VK_SAMPLE_COUNT_1_BIT,
                    .load_op          = VK_ATTACHMENT_LOAD_OP_CLEAR,
                    .store_op         = VK_ATTACHMENT_STORE_OP_STORE,
                    .stencil_load_op  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                    .stencil_store_op = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                    .initial_layout   = VK_IMAGE_LAYOUT_UNDEFINED,
                    .final_layout     = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
                }}
            },
            .subpasses = {
                {{
                    .pipeline_bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS,
                    .color_attachments = {
                        {
                            .attachment = 0,
                            .layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
                        }
                    },
                }}
            },
            .dependencies = {
                {{
                    // Subpasses
                    .src_subpass = VK_SUBPASS_EXTERNAL,
                    .dst_subpass = 0,

                    // Stage masks
                    .src_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                    .dst_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,

                    // Access masks
                    .src_access_mask = 0,
                    .dst_access_mask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                }}
            }
        }}, device);

        // Create pipelines
        const auto pipeline = vulkan::create_pipeline({{
            .stages = {
                // Vertex shader stage
                {{
                    .stage  = VK_SHADER_STAGE_VERTEX_BIT,
                    .module = vulkan::create_shader_module("vert.spv", device),
                    .name   = "main",
                }},
                // Geometry shader stage
                {{
                    .stage  = VK_SHADER_STAGE_GEOMETRY_BIT,
                    .module = vulkan::create_shader_module("geom.spv", device),
                    .name   = "main",
                }},
                // Fragment shader stage
                {{
                    .stage  = VK_SHADER_STAGE_FRAGMENT_BIT,
                    .module = vulkan::create_shader_module("frag.spv", device),
                    .name   = "main",
                }}
            },

            .vertex_input_state = {{
                .vertex_binding_descriptions = {
                    {
                        .binding   = 0,
                        .stride    = sizeof(Vertex),
                        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
                    }
                },
                .vertex_attribute_descriptions = {
                    {
                        .location = 0,
                        .binding  = 0,
                        .format   = VK_FORMAT_R32G32B32_SFLOAT,
                        .offset   = offsetof(Vertex, position)
                    },
                    {
                        .location = 1,
                        .binding  = 0,
                        .format   = VK_FORMAT_R32G32B32_SFLOAT,
                        .offset   = offsetof(Vertex, color)
                    }
                }
            }},

            .input_assembly_state = {{
                .topology                 = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                .primitive_restart_enable = VK_FALSE
            }},

            .viewport_state = {{
                .viewports = {
                    {
                        .x        = 0.0f,
                        .y        = 0.0f,
                        .width    = static_cast<float>(surface_extent.width),
                        .height   = static_cast<float>(surface_extent.height),
                        .minDepth = 0.0f,
                        .maxDepth = 1.0f
                    }
                },

                .scissors = {
                    {
                        .offset = { 0, 0 },
                        .extent = surface_extent
                    }
                }
            }},

            .rasterization_state = {{
                .depth_clamp_enable         = VK_FALSE,
                .rasterizer_discard_enable  = VK_FALSE,
                .polygon_mode               = VK_POLYGON_MODE_FILL,
                .cull_mode                  = VK_CULL_MODE_BACK_BIT,
                .front_face                 = VK_FRONT_FACE_COUNTER_CLOCKWISE,
                .depth_bias_enable          = VK_FALSE,
                .depth_bias_constant_factor = 0.0f,
                .depth_bias_clamp           = 0.0f,
                .depth_bias_slope_factor    = 0.0f,
                .line_width                 = 1.0f
            }},

            .multisample_state = {{
                .rasterization_samples = VK_SAMPLE_COUNT_1_BIT,
                .min_sample_shading    = 1.0f,
            }},

            .color_blend_state = {{
                .logic_op_enable = VK_FALSE,
                .attachments = {
                    {{
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
                    }}
                },

                .blend_constants = { 0.0f, 0.0f, 0.0f, 0.0f }
            }},

            .layout = pipeline_layout,

            .render_pass = render_pass,
        }}, VK_NULL_HANDLE, device);

        // Create framebuffers
        std::vector<Deleter<VkFramebuffer>> swapchain_framebuffers{swapchain_image_views.size()};
        for (size_t i = 0; i < swapchain_framebuffers.size(); ++i) {
            swapchain_framebuffers[i] = vulkan::create_framebuffer({{
                .render_pass = render_pass,
                .attachments = {{
                    swapchain_image_views[i]
                }},
                .width  = surface_extent.width,
                .height = surface_extent.height,
                .layers = 1
            }}, device);
        }

        // Create vertices
        const std::vector<Vertex> vertices = {
            {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
            {{ 0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
            {{ 0.5f,  0.5f, 0.0f}, {0.0f, 0.0f , 1.0f}},
            {{-0.5f,  0.5f, 0.0f}, {1.0f, 1.0f , 1.0f}}
        };

        // Create indices
        const std::vector<uint16_t> indices = {
            0, 1, 2, 2, 3, 0
        };

        // Calculate vertex buffer size
        const auto vertex_buffer_size = sizeof(Vertex) * vertices.size();

        // Create vertex buffer
        const auto vertex_buffer = device.create_buffer({{
            .size         = vertex_buffer_size,
            .usage        = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            .sharing_mode = VK_SHARING_MODE_EXCLUSIVE,
        }});

        // Find memory requirements for vertex buffer
        auto memory_requirements = device.get_buffer_memory_requirements(vertex_buffer);

        // Allocate memory for vertex buffer
        const auto vertex_buffer_memory = device.allocate_memory({{
            .allocation_size   = memory_requirements.size,
            .memory_type_index = vulkan::find_memory_type(
                physical_device,
                memory_requirements.memoryTypeBits,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
            )
        }});

        // Bind memory to vertex buffer
        vkBindBufferMemory(device, vertex_buffer, vertex_buffer_memory, 0);

        {
            // Create staging buffer
            const auto staging_buffer = device.create_buffer({{
                .size         = vertex_buffer_size,
                .usage        = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                .sharing_mode = VK_SHARING_MODE_EXCLUSIVE,
            }});

            // Find memory requirements for staging buffer
            memory_requirements = device.get_buffer_memory_requirements(staging_buffer);

            // Allocate memory for staging buffer
            const auto staging_buffer_memory = device.allocate_memory({{
                .allocation_size   = memory_requirements.size,
                .memory_type_index = vulkan::find_memory_type(
                    physical_device,
                    memory_requirements.memoryTypeBits,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
                )
            }});

            // Bind memory to staging buffer
            vkBindBufferMemory(device, staging_buffer, staging_buffer_memory, 0);

            // Copy vertex data to staging buffer
            staging_buffer_memory.map().copy(vertices.data(), vertex_buffer_size);

            // Copy staging buffer to vertex buffer
            const auto command_buffer = command_pool.allocate_command_buffers({
                .level                = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                .command_buffer_count = 1
            })[0];

            command_buffer.begin({{
                .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
            }});

            command_buffer.copy_buffer(
                staging_buffer,
                vertex_buffer,
                {
                    {
                        .srcOffset = 0,
                        .dstOffset = 0,
                        .size = vertex_buffer_size
                    }
                }
            );

            command_buffer.end();

            vulkan::queue_submit({
                {{
                    .command_buffers = {
                        command_buffer
                    },
                }}
            }, graphics_queue);

            vkQueueWaitIdle(graphics_queue);

            //command_pool.free_command_buffers({command_buffer}));
            // TODO vkFreeCommandBuffers(device, command_pool, 1, &command_buffer);
        }

        // Calculate index buffer size
        const auto index_buffer_size = sizeof(indices[0]) * indices.size();

        // Create index buffer
        const auto index_buffer = device.create_buffer({{
            .size         = index_buffer_size,
            .usage        = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            .sharing_mode = VK_SHARING_MODE_EXCLUSIVE,
        }});

        // Find memory requirements for index buffer
        memory_requirements = device.get_buffer_memory_requirements(index_buffer);

        // Allocate memory for index buffer
        const auto index_buffer_memory = device.allocate_memory({{
            .allocation_size   = memory_requirements.size,
            .memory_type_index = vulkan::find_memory_type(
                physical_device,
                memory_requirements.memoryTypeBits,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
            )
        }});

        // Bind memory to index buffer
        vkBindBufferMemory(device, index_buffer, index_buffer_memory, 0);

        {
            // Create staging buffer
            const auto staging_buffer = device.create_buffer({{
                .size         = index_buffer_size,
                .usage        = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                .sharing_mode = VK_SHARING_MODE_EXCLUSIVE,
            }});

            // Find memory requirements for staging buffer
            memory_requirements = device.get_buffer_memory_requirements(staging_buffer);

            // Allocate memory for staging buffer
            const auto staging_buffer_memory = device.allocate_memory({{
                .allocation_size   = memory_requirements.size,
                .memory_type_index = vulkan::find_memory_type(
                    physical_device,
                    memory_requirements.memoryTypeBits,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
                )
            }});

            // Bind memory to staging buffer
            vkBindBufferMemory(device, staging_buffer, staging_buffer_memory, 0);

            // Copy index data to staging buffer
            staging_buffer_memory.map().copy(indices.data(), index_buffer_size);

            // Copy staging buffer to index buffer
            const auto command_buffer = command_pool.allocate_command_buffers({
                .level                = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                .command_buffer_count = 1
            })[0];

            command_buffer.begin({{
                .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
            }});

            command_buffer.copy_buffer(
                staging_buffer,
                index_buffer,
                {
                    {
                        .srcOffset = 0,
                        .dstOffset = 0,
                        .size = index_buffer_size
                    }
                }
            );

            command_buffer.end();

            vulkan::queue_submit({
                {{
                    .command_buffers = {
                        command_buffer
                    },
                }}
            }, graphics_queue);

            vkQueueWaitIdle(graphics_queue);


            //command_pool.free_command_buffers({command_buffer});
            // TODO vkFreeCommandBuffers(device, command_pool, 1, &command_buffer);
        }

        // Create uniform buffers
        std::vector<vulkan::Buffer>       uniform_buffers;
        std::vector<vulkan::DeviceMemory> uniform_buffer_memories;
        for (size_t i = 0; i < swapchain_image_views.size(); ++i) {
            // Create uniform buffer
            uniform_buffers.emplace_back(device.create_buffer({{
                .size         = sizeof(UniformBufferObject),
                .usage        = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                .sharing_mode = VK_SHARING_MODE_EXCLUSIVE,
            }}));

            // Find memory requirements for uniform buffer
            memory_requirements = device.get_buffer_memory_requirements(uniform_buffers[i]);

            // Allocate memory for uniform buffer
            uniform_buffer_memories.emplace_back(vulkan::DeviceMemory{{{
                .allocation_size   = memory_requirements.size,
                .memory_type_index = vulkan::find_memory_type(
                    physical_device,
                    memory_requirements.memoryTypeBits,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
                )
            }}, device});

            // Bind memory to uniform buffer
            vkBindBufferMemory(device, uniform_buffers[i], uniform_buffer_memories[i], 0);
        }

        // Create descriptor pool
        const auto descriptor_pool = vulkan::create_descriptor_pool({{
            .pool_sizes = {
                {
                    .type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    .descriptorCount = static_cast<uint32_t>(swapchain_image_views.size())
                }
            },
            .max_sets = static_cast<uint32_t>(swapchain_image_views.size())
        }}, device);

        // Allocate descriptor sets
        const auto descriptor_sets = vulkan::allocate_descriptor_sets({{
            .descriptor_pool = descriptor_pool,
            .set_layouts     = {swapchain_image_views.size(), descriptor_set_layout}
        }}, device);

        // Update descriptor sets
        for (size_t i = 0; i < swapchain_image_views.size(); ++i) {
            vulkan::update_descriptor_sets(device, {
                {{
                    .dst_set           = descriptor_sets[i],
                    .dst_binding       = 0,
                    .dst_array_element = 0,
                    .descriptor_type   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                    .descriptor_count  = 1,
                    .buffer_info       = {
                        .buffer = uniform_buffers[i],
                        .offset = 0,
                        .range = sizeof(UniformBufferObject) // VK_WHOLE_SIZE
                    }
                }}
            }, {});
        }

        // Allocate command buffers
        const auto command_buffers = command_pool.allocate_command_buffers({
            .level                = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .command_buffer_count = static_cast<uint32_t>(swapchain_framebuffers.size())
        });

        // Record command buffers
        for (size_t i = 0; i < command_buffers.size(); ++i) {
            // Begin command buffer
            command_buffers[i].begin({{
                .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT
            }});

            // Begin render pass
            vulkan::begin_render_pass({{
                .render_pass  = render_pass,
                .framebuffer  = swapchain_framebuffers[i],
                .render_area  = {
                    .offset = { 0, 0 },
                    .extent = surface_extent
                },
                .clear_values = {
                    { 0.0f, 0.0f, 0.0f, 1.0f }
                }
            }}, command_buffers[i], VK_SUBPASS_CONTENTS_INLINE);

            // Bind pipeline
            vkCmdBindPipeline(command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

            // Bind vertex buffers
            vulkan::cmd_bind_vertex_buffers(command_buffers[i], 0, { vertex_buffer }, { 0 });

            // Bind index buffer
            vulkan::cmd_bind_index_buffer(command_buffers[i], index_buffer, 0, VK_INDEX_TYPE_UINT16);

            // Bind descriptor sets
            vulkan::cmd_bind_descriptor_sets(command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout, 0, { descriptor_sets[i] }, {});

            // Draw
            vkCmdDrawIndexed(command_buffers[i], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

            // End render pass
            vkCmdEndRenderPass(command_buffers[i]);

            // End command buffer
            vulkan::vulkan_assert(vkEndCommandBuffer(command_buffers[i]), "Failed to record command buffer.");
        }

        // Create semaphores
        constexpr auto max_frames_in_flight = 2;
        const auto image_available_semaphores = vulkan::create_semaphores(device, max_frames_in_flight);
        const auto render_finished_semaphores = vulkan::create_semaphores(device, max_frames_in_flight);
        const auto in_flight_fences = vulkan::create_fences(device, max_frames_in_flight, true);

        // Loop
        size_t current_frame = 0;
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();

            /// Wait for frame to be finished and reset fence
            vulkan::wait_for_fence(device, in_flight_fences[current_frame]);
            vulkan::reset_fence(device, in_flight_fences[current_frame]);

            // Get next image from swapchain
            const auto image_index = vulkan::acquire_next_image(device, swapchain, image_available_semaphores[current_frame]);
            
            // Update uniform buffer
            {
                // Calculate delta time
                static auto start_time = std::chrono::high_resolution_clock::now();
                const auto current_time = std::chrono::high_resolution_clock::now();
                float time = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();

                // Define uniform buffer object
                UniformBufferObject ubo = {
                    .model      = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
                    .view       = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
                    .projection = glm::perspective(glm::radians(45.0f), surface_extent.width / (float) surface_extent.height, 0.1f, 10.0f)
                };
                ubo.projection[1][1] *= -1;

                // Copy uniform buffer object to uniform buffer
                uniform_buffer_memories[image_index].map().copy(&ubo, sizeof(ubo));
            }

            // Submit command buffer to graphics queue
            vulkan::queue_submit({
                {{
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
                }}
            }, graphics_queue, in_flight_fences[current_frame]);

            // Present images
            vulkan::queue_present({{
                .wait_semaphores = {
                    render_finished_semaphores[current_frame]
                },
                .swapchains = {
                    swapchain
                },
                .image_indices = {
                    image_index
                }
            }}, present_queue);

            // Wait until queue is idle
            vkQueueWaitIdle(present_queue);

            // Advance to next frame
            current_frame = (current_frame + 1) % max_frames_in_flight;
        }

        // Wait until device is idle
        vkDeviceWaitIdle(device);
    }

}

int main() {
    try {
        stirling::StirlingInstance stirling_instance;
    } catch (const char* message) {
        std::cout << message << '\n';
    }
    return 0;
}