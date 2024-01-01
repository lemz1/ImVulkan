
Texture2D texture : register(t0);
SamplerState samplerState : register(s0);

struct FragmentInput
{
    float2 texCoord : TEXCOORD0;
};

struct FragmentOutput
{
    float4 color : SV_Target;
};

FragmentOutput main(FragmentInput input)
{
    FragmentOutput output;
    output.color = texture.Sample(samplerState, input.texCoord);
    return output;
}
