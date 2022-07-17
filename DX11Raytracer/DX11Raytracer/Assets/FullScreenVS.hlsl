struct v2f
{
    float4 uv : Texcoord; // (pixel UV, tile UV)
    float4 pos : SV_Position;
};

cbuffer Buffer_CB : register(b0)
{
    uint _TileSizeX;
    uint _TileSizeY;
    uint _TileCountX;
    uint _TileCountY;
};

v2f main(uint index : INDEX)
{
    v2f o;

    o.pos.x = (index == 1) ? 3.f : -1.f;
    o.pos.y = (index <= 1) ? -1.f : 3.f;
    o.pos.z = 1.f;
    o.pos.w = 1.f;

    // This version is in the [0, 1] range
    //o.uv.x = (index == 1) ? 2.f : 0.f;
    //o.uv.y = (index <= 1) ? 0.f : 2.f;

    // This version maps to pixel coords
    uint2 resolution = uint2(_TileSizeX * _TileCountX, _TileSizeY * _TileCountY);
    o.uv.x = (index == 1) ? resolution.x * 2.f : 0.f;
    o.uv.y = (index <= 1) ? 0.f : resolution.y * 2.f;

    o.uv.z = (index == 1) ? (resolution.x >> 4u) * 2.f : 0.f;
    o.uv.w = (index <= 1) ? 0.f : (resolution.y >> 4u) * 2.f;

    return o;
}