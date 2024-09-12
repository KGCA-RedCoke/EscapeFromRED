float4 g_Light6400k = float4(1.0, 0.976, 0.99, 1.0);

float4 Mask_R(float InColor)
{
	return float4(InColor, 0, 0, 1);
};

float4 Mask_G(float InColor)
{
	return float4(0, InColor, 0, 1);
};

float4 Mask_B(float InColor)
{
	return float4(0, 0, InColor, 1);
};

float4 Mask_A(float InColor)
{
	return float4(0, 0, 0, InColor);
};
