cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VertexInputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 instancePosition : TEXCOORD1;
	uint textureID : BLENDINDICES0;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	uint textureID : BLENDINDICES0;
	float4 colour : COLOR;
};

// Vertex Shader
PixelInputType TextureVertexShader(VertexInputType input)
{
	PixelInputType output;

	input.normal = normalize(mul(input.normal, worldMatrix));

	output.colour = float4(1, 1, 1, 1);

	float3 light_dir = normalize(float3(0.1f, -1.0f, 0.1f));

	float intensity = dot( -light_dir, input.normal);

	intensity += 0.3f;

	// Ambient light.
	intensity = clamp(intensity, 0.0f, 1.0f);

	output.colour *= intensity;

	// Change the position vector to be 4 units for proper matrix calculations.
	input.position.w = 1.0f;

	input.position.x += input.instancePosition.x;
	input.position.y += input.instancePosition.y;
	input.position.z += input.instancePosition.z;


	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;
	output.textureID = input.textureID;

	return output;
}
