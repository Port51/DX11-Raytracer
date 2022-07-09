
StructuredBuffer<float4> buffer : register(t0);
SamplerState splr : register(s0);

float4 main(float4 uv : Texcoord) : SV_Target
{
    uint2 tuv = (uint2)uv.zw; // tile uv
    uint2 uvi = (uint2)uv.xy;
    uint2 luv = uvi % 16u; // local uv in tile

    //return tuv.y == 1;

    //uint idx = uvi.x + uvi.y * 256;
    uint idx = (tuv.y * 16u + tuv.x) * 256u + (luv.y * 16u + luv.x);

    //return uv.y / 256.f < 0.5f;

    //return uv.y;
    //return uv.y < 0.5f ? 1 : 0;
    return float4(buffer[idx].rgb, 1);
    //return tex.Sample(splr, uv).rgba;
}