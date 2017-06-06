cbuffer MVP_CONSTANT_BUFFER : register(b0)
{
	float4x4 MVP;
};

struct PSInput
{
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
	float4 color : COLOR;
};

PSInput VSMain(float4 position : POSITION, float3 normal : NORMAL, float3 tangent : TANGENT, float biTangent : BITANGENT, float texCoord : TEXCOORD)
{
	PSInput result;

	result.position = mul(MVP, position);
	result.normal = normal;
	result.color = float4(normal, 1.0f);
	//result.color = float4(normal, 1.0f);

	return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
	float3 lightColor = float3(1.0f, 1.0f, 1.0f);

	float ambientStrength = 0.5f;
	float3 ambient = ambientStrength * lightColor;

	float3 norm = normalize(input.normal);
	float3 lightDir = normalize(float3(1.0f, -3.0f, 1.0f));
	float diff = max(dot(norm, lightDir), 0.0);
	float3 diffuse = diff * lightColor;

	float3 result = (ambient + diffuse) * input.color;

	return float4(result, 1.0f);
}
