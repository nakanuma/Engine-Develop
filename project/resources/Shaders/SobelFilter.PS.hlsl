#include "SobelFilter.hlsli"

struct Material
{
    float32_t4 color;
    int32_t enableLighting;
    float32_t4x4 uvTransform;
};

ConstantBuffer<Material> gMaterial : register(b0);

struct DirectionalLight
{
    float32_t4 color;
    float32_t3 direction;
    float intensity;
};

ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input)
{
    const float uPerPixel = 1.0f / 1280.0f;
    const float vPerPixel = 1.0f / 720.0f;
    const float weight[3][3] =
    {
        { 1, 0, -1 },
        { 2, 0, -2 },
        { 1, 0, -1 },
    };
    const float weight2[3][3] =
    {
        { -1, -2, -1 },
        { 0, 0, 0 },
        { 1, 2, 1 },
    };
    const float power = 20.0f;
    float4 outputX = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 outputY = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    PixelShaderOutput output;
    output.color = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float2 tempUV = input.texcoord.xy;
    float32_t4 textureColor;
    
    ///
    /// 縦方向
    ///
    
    // 左上のピクセル
    tempUV = input.texcoord.xy;
    tempUV.x -= uPerPixel;
    tempUV.y -= vPerPixel;
    textureColor = gTexture.Sample(gSampler, tempUV);
    textureColor = pow(textureColor, power);
    outputY += textureColor * weight[0][0];
    
    // 真ん中上のピクセル
    tempUV = input.texcoord.xy;
    tempUV.y -= vPerPixel;
    textureColor = gTexture.Sample(gSampler, tempUV);
    textureColor = pow(textureColor, power);
    outputY += textureColor * weight[0][1];
    
    // 右上のピクセル
    tempUV = input.texcoord.xy;
    tempUV.x += uPerPixel;
    tempUV.y -= vPerPixel;
    textureColor = gTexture.Sample(gSampler, tempUV);
    textureColor = pow(textureColor, power);
    outputY += textureColor * weight[0][2];
    
    
     // 真ん中左のピクセル
    tempUV = input.texcoord.xy;
    tempUV.x -= uPerPixel;
    textureColor = gTexture.Sample(gSampler, tempUV);
    textureColor = pow(textureColor, power);
    outputY += textureColor * weight[1][0];
    
    // 真ん中のピクセル
    tempUV = input.texcoord.xy;
    textureColor = gTexture.Sample(gSampler, tempUV);
    textureColor = pow(textureColor, power);
    outputY += textureColor * weight[1][1];
    
    // 真ん中右のピクセル
    tempUV = input.texcoord.xy;
    tempUV.x += uPerPixel;
    textureColor = gTexture.Sample(gSampler, tempUV);
    textureColor = pow(textureColor, power);
    outputY += textureColor * weight[1][2];
    
    
     // 左下のピクセル
    tempUV = input.texcoord.xy;
    tempUV.x -= uPerPixel;
    tempUV.y += vPerPixel;
    textureColor = gTexture.Sample(gSampler, tempUV);
    textureColor = pow(textureColor, power);
    outputY += textureColor * weight[2][0];
    
    // 真ん中下のピクセル
    tempUV = input.texcoord.xy;
    tempUV.y += vPerPixel;
    textureColor = gTexture.Sample(gSampler, tempUV);
    textureColor = pow(textureColor, power);
    outputY += textureColor * weight[2][1];
    
    // 右下のピクセル
    tempUV = input.texcoord.xy;
    tempUV.x += uPerPixel;
    tempUV.y += vPerPixel;
    textureColor = gTexture.Sample(gSampler, tempUV);
    textureColor = pow(textureColor, power);
    outputY += textureColor * weight[2][2];
    
    
    ///
    /// 横方向
    ///

    
    // 左上のピクセル
    tempUV = input.texcoord.xy;
    tempUV.x -= uPerPixel;
    tempUV.y -= vPerPixel;
    textureColor = gTexture.Sample(gSampler, tempUV);
    textureColor = pow(textureColor, power);
    outputX += textureColor * weight2[0][0];
    
    // 真ん中上のピクセル
    tempUV = input.texcoord.xy;
    tempUV.y -= vPerPixel;
    textureColor = gTexture.Sample(gSampler, tempUV);
    textureColor = pow(textureColor, power);
    outputX += textureColor * weight2[0][1];
    
    // 右上のピクセル
    tempUV = input.texcoord.xy;
    tempUV.x += uPerPixel;
    tempUV.y -= vPerPixel;
    textureColor = gTexture.Sample(gSampler, tempUV);
    textureColor = pow(textureColor, power);
    outputX += textureColor * weight2[0][2];
    
    
     // 真ん中左のピクセル
    tempUV = input.texcoord.xy;
    tempUV.x -= uPerPixel;
    textureColor = gTexture.Sample(gSampler, tempUV);
    textureColor = pow(textureColor, power);
    outputX += textureColor * weight2[1][0];
    
    // 真ん中のピクセル
    tempUV = input.texcoord.xy;
    textureColor = gTexture.Sample(gSampler, tempUV);
    textureColor = pow(textureColor, power);
    outputX += textureColor * weight2[1][1];
    
    // 真ん中右のピクセル
    tempUV = input.texcoord.xy;
    tempUV.x += uPerPixel;
    textureColor = gTexture.Sample(gSampler, tempUV);
    textureColor = pow(textureColor, power);
    outputX += textureColor * weight2[1][2];
    
    
     // 左下のピクセル
    tempUV = input.texcoord.xy;
    tempUV.x -= uPerPixel;
    tempUV.y += vPerPixel;
    textureColor = gTexture.Sample(gSampler, tempUV);
    textureColor = pow(textureColor, power);
    outputX += textureColor * weight2[2][0];
    
    // 真ん中下のピクセル
    tempUV = input.texcoord.xy;
    tempUV.y += vPerPixel;
    textureColor = gTexture.Sample(gSampler, tempUV);
    textureColor = pow(textureColor, power);
    outputX += textureColor * weight2[2][1];
    
    // 右下のピクセル
    tempUV = input.texcoord.xy;
    tempUV.x += uPerPixel;
    tempUV.y += vPerPixel;
    textureColor = gTexture.Sample(gSampler, tempUV);
    textureColor = pow(textureColor, power);
    outputX += textureColor * weight2[2][2];
    
    output.color = saturate(max(abs(outputX), abs(outputY)));
    
    if (output.color.r <= 0.2)
    {
        discard;
    }
    
    output.color.g = output.color.r;
    output.color.b = output.color.r;
    output.color.a = 1.0f;
    output.color *= gMaterial.color;
    return output;
}