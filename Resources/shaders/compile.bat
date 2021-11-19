%VULKAN_SDK%/Bin/glslc.exe bullet_shader.vert -o bullet_shader_vert.spv
%VULKAN_SDK%/Bin/glslc.exe bullet_shader.frag -o bullet_shader_frag.spv

%VULKAN_SDK%/Bin/glslc.exe default_shader.vert -o default_shader_vert.spv
%VULKAN_SDK%/Bin/glslc.exe default_shader.frag -o default_shader_frag.spv

%VULKAN_SDK%/Bin/glslc.exe line_shader.vert -o line_shader_vert.spv
%VULKAN_SDK%/Bin/glslc.exe line_shader.frag -o line_shader_frag.spv
pause