dxc.exe -spirv -T vs_6_0 -E main texture.vert.hlsl -Fo texture.vert.hlsl.spv
dxc.exe -spirv -T ps_6_0 -E main texture.frag.hlsl -Fo texture.frag.hlsl.spv

glslc.exe -fshader-stage=vert "color.vert" -o "color.vert.spv"
glslc.exe -fshader-stage=frag "color.frag" -o "color.frag.spv"
