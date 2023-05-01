struct VSInput {
  float3 position_l : POSITION;
  float2 tex_coord : TEXCOORD;
  float3 normal : NORMAL;
  float3 tangent : TANGENT;
};

struct PSInput {
    float4 position_h : SV_POSITION;
    float2 tex_coord : TEXCOORD;
};

PSInput VSMain(VSInput input) {
    PSInput result;
    result.position_h = float4(input.position_l, 1.0f);
    result.tex_coord = input.tex_coord;

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET {
    return float4(input.tex_coord, 0.0f, 1.0f);
}