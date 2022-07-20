
StructuredBuffer<float4> buffer : register(t0);
SamplerState splr : register(s0);

cbuffer Buffer_CB : register(b0)
{
    uint _TileSizeX;
    uint _TileSizeY;
    uint _TileCountX;
    uint _TileCountY;
};

float4 main(float4 uv : Texcoord) : SV_Target
{
    uint2 tuv = (uint2)uv.zw; // tile uv
    uint2 uvi = (uint2)uv.xy;
    uint2 luv = uvi % 16u; // local uv in tile

    //uint idx = (tuv.y * 16u + tuv.x) * 256u + (luv.y * 16u + luv.x);
    uint idx = (tuv.y * _TileCountX + tuv.x) * (_TileSizeX * _TileSizeY) + (luv.y * _TileSizeX + luv.x);

    return float4(buffer[idx].rgb, 1); // apply gamma correction
    //return float4(pow(buffer[idx].rgb, 2.0), 1); // apply gamma correction
}