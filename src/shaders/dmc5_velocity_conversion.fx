#define DMC5VelocityRS \
"RootFlags ( ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |" \
"            DENY_DOMAIN_SHADER_ROOT_ACCESS |" \
"            DENY_GEOMETRY_SHADER_ROOT_ACCESS |" \
"            DENY_HULL_SHADER_ROOT_ACCESS )," \
"DescriptorTable ( SRV(t0), visibility = SHADER_VISIBILITY_PIXEL )," \
"CBV(b0), " \
"StaticSampler(s0," \
"           filter = FILTER_MIN_MAG_MIP_POINT," \
"           addressU = TEXTURE_ADDRESS_CLAMP," \
"           addressV = TEXTURE_ADDRESS_CLAMP," \
"           addressW = TEXTURE_ADDRESS_CLAMP," \
"           visibility = SHADER_VISIBILITY_PIXEL )"

Texture2D<float4> MotionVectors : register(t0);

cbuffer Parameters : register(b0)
{
    row_major float4x4 MatrixTransform;
};

[RootSignature(DMC5VelocityRS)]
void DMC5VelocityVS(
    inout float4 color : COLOR0,
    inout float2 texCoord : TEXCOORD0,
    inout float4 position : SV_Position)
{
    position = mul(position, MatrixTransform);
}

[RootSignature(DMC5VelocityRS)]
float4 DMC5VelocityPS(
    float4 color : COLOR0,
    float2 texCoord : TEXCOORD0,
    float4 position : SV_Position) : SV_Target0
{
    const int2 pixel = int2(position.xy);

    const float4 velocity = MotionVectors.Load(int3(pixel, 0));

    // DMC5:
    // R = unrelated/sentinel
    // G = motion X
    // B = motion Y
    // A = unrelated
    //
    // Output conventional XY motion vectors in RG.
    return float4(velocity.g, velocity.b, 0.0f, 1.0f);
}