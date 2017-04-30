cbuffer MVP_CONSTANT_BUFFER : register(b0)
{
	float4x4 MVP;
};

struct PSInput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

PSInput VSMain(float4 position : POSITION, float3 normal : NORMAL, float3 tangent : TANGENT, float biTangent : BITANGENT, float texCoord : TEXCOORD)
{
	PSInput result;

	result.position = mul(MVP, position);
	result.color = position;
	//result.color = float4(normal, 1.0f);

	return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
	return input.color;
}
