dxc.exe -spirv -T vs_6_0 -E main texture.vert.hlsl -Fo texture.vert.hlsl.spv
dxc.exe -spirv -T ps_6_0 -E main texture.frag.hlsl -Fo texture.frag.hlsl.spv