
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

    //uint2 luv = uvi % 16u; // local uv in tile
    //uint idx = (tuv.y * _TileCountX + tuv.x) * (_TileSizeX * _TileSizeY) + (luv.y * _TileSizeX + luv.x);

    uint idx = uvi.y * _TileCountX * _TileSizeX + uvi.x;

    return float4(buffer[idx].rgb, 1);
}