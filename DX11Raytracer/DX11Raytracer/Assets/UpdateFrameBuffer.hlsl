StructuredBuffer<float> ImageBuffer : register(t0);
RWTexture2D<float4> Framebuffer : register(u0);

/*cbuffer Dither_CB : register(b0)
{
    float _ShadowDither;
    float _MidDither;
    float2 padding;
};*/

[numthreads(16, 16, 1)]
void CSMain(uint3 tId : SV_DispatchThreadID, uint3 gtId : SV_GroupThreadID, uint3 gId : SV_GroupID, uint gIdx : SV_GroupIndex)
{
    // Unpack source index
    uint srcIdx = (gId.y * 16 + gId.x) * 16 + gIdx.x;
    Framebuffer[tId.xy] = float4(
        ImageBuffer[srcIdx * 3u + 0u],
        ImageBuffer[srcIdx * 3u + 1u],
        ImageBuffer[srcIdx * 3u + 2u],
        1.f
        );
}