float4x4 wvpMat;

struct VS_INPUT
{
    float4 position : POSITION;
    float4 colour : COLOR0;
};

struct VS_OUTPUT
{
    float4 position : POSITION;
    float4 colour : COLOR0;
};

VS_OUTPUT main_vs(VS_INPUT input)
{
    VS_OUTPUT output;
    output.position = mul(wvpMat, input.position);
    output.colour = input.colour;
    return output;
}

float4 main_ps(VS_OUTPUT input) : COLOR0
{
    return input.colour;
}
