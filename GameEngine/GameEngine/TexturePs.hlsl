Texture2D<float4>  shaderTexture[2];
SamplerState SampleType;

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	uint textureID : BLENDINDICES0;
	float4 colour : COLOR;
};

// Pixel Shader
float4 TexturePixelShader(PixelInputType input) : SV_TARGET
{
	float4 textureColor;
	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	if (input.textureID == 0)
	{
		textureColor = shaderTexture[0].Sample(SampleType, input.tex);
	}
	else if (input.textureID == 1)
	{
		textureColor = shaderTexture[1].Sample(SampleType, input.tex);
	}
	else
	{
		textureColor = shaderTexture[1].Sample(SampleType, input.tex);
	}

	textureColor *= input.colour;

	return textureColor;
}