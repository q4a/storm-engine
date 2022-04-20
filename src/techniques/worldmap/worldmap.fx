struct VS_OUTPUT
{
    float4 pos      : POSITION;
    vector diffuse  : COLOR0;
    vector specular : COLOR1;
    float2 uv1out   : TEXCOORD0;
    float4 norm     : TEXCOORD1;
};

uniform extern float4x4 prj1; // c0 - c3
uniform extern float4x4 prj2; // c4 - c6

// pos is dcl_position v0, col is dcl_color v1, uv1 is dcl_texcoord v2
VS_OUTPUT main(float3 pos : POSITION, float4 col : COLOR, float2 uv1 : TEXCOORD)
{
    //VS_OUTPUT Output;
    VS_OUTPUT Output = (VS_OUTPUT)0;

    // Position
    // mov  r0, v0
    // mul  r1, r0.xxxx, c[0]
    // mad  r1, r0.yyyy, c[1], r1
    // mad  r1, r0.zzzz, c[2], r1
    // add  oPos, r1, c[3]
    Output.pos = mul(pos.xxxx, prj1[0]) + mul(pos.yyyy, prj1[1]) + mul(pos.zzzz, prj1[2]) + prj1[3];

    // Color
    // mov  oD0, v1.xxxw
    Output.diffuse = col.xxxw;
    // mul  oD1, v1.yyyw, c[6]
    Output.specular = mul(col.yyyw, prj2[2]);

    // Texture coordinates of the first stage
    // mov  oT0, v2
    Output.uv1out = uv1;

    // Generate texture coordinates of the second stage
    // Vector length for a given square
    ////////// vlen is r2.w, prj2[1] is r2 //////////
    // mov  r2, c[5]
    // mad  r2.w, v1.z, r2.z, r2.x
    //float vlen = mul(col.z, prj2[1].z) + prj2[1].x;
    float vlen = col.z * prj2[1].z + prj2[1].x;

    // Vertical component of the vector
    ////////// vhigh is r1.w //////////
    // dp4  r1, r0, c[4]
    //float vhigh = (pos.x*prj2[0].x) + (pos.y*prj2[0].y) + (pos.z*prj2[0].z) + (pos.w*prj2[0].w);
    float vhigh = pos.x*prj2[0].x + pos.y*prj2[0].y + pos.z*prj2[0].z;

    // Normalize and reduce to unit range
    // rcp  r2.w, r2.w
    // mad  r1.w, r1.w, r2.w, c[5].w
    vhigh = vhigh/vlen + prj2[1].w;
    // mov oT1, r1.wwww
    Output.norm = float4(vhigh, vhigh, vhigh, vhigh);

    return Output;
}


technique WdmModelDrawStd
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;

        ColorArg1[0] = texture;
        ColorOp[0] = SelectArg1;
        ColorOp[1] = disable;
        AlphaOp[0] = disable;

        AlphaTestEnable = false;
        AlphaBlendEnable = false;
    }
}

technique WdmModelDrawStdAlphaTest
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;

        ColorArg1[0] = texture;
        ColorOp[0] = SelectArg1;
        ColorOp[1] = disable;
        AlphaArg1[0] = texture;
        AlphaOp[0] = SelectArg1;
        AlphaOp[1] = disable;

        AlphaTestEnable = false;
        AlphaBlendEnable = true;
    }
}

technique WdmModelDrawStdA
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;

        ColorArg1[0] = texture;
        ColorOp[0] = SelectArg1;
        ColorOp[1] = disable;
        AlphaArg1[0] = tfactor;
        AlphaOp[0] = SelectArg1;
        AlphaOp[1] = disable;

        AlphaTestEnable = false;
        AlphaBlendEnable = true;

        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;
    }
}

technique WdmCounterDrawSky
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;

        TexCoordIndex[0] = 0;
        TexCoordIndex[1] = 0;
        ColorArg1[0] = texture;
        ColorOp[0] = SelectArg1;
        ColorArg1[1] = texture;
        ColorArg2[1] = current;
        ColorOp[1] = blendfactoralpha;
        ColorOp[2] = disable;
        AlphaOp[0] = selectarg1;
        AlphaArg1[0] = tfactor;
        AlphaOp[1] = disable;

        AlphaTestEnable = false;
        AlphaBlendEnable = false;
    }
}

technique WdmCounterDrawNumber
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;

        ColorArg1[0] = texture;
        ColorOp[0] = SelectArg1;
        ColorArg1[1] = texture;
        ColorArg2[1] = current;
        ColorOp[1] = addsigned;
        ColorOp[2] = disable;
        AlphaOp[0] = disable;

        AlphaTestEnable = false;
        AlphaBlendEnable = false;
    }
}

technique WdmClouds
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;

        ZWriteEnable = false;

        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;
        CullMode = none;

        AlphaTestEnable = false;
        AlphaBlendEnable = true;

        ColorArg1[0] = texture;
        ColorArg2[0] = diffuse;
        ColorOp[0] = modulate;
        ColorArg1[1] = texture;
        ColorArg2[1] = current;
        ColorOp[1] = modulate;
        ColorArg1[2] = specular;
        ColorArg2[2] = current;
        ColorOp[2] = add;
        ColorOp[3] = disable;
        AlphaArg1[0] = texture;
        AlphaArg2[0] = diffuse;
        AlphaOp[0] = modulate;
        AlphaOp[1] = disable;

        AddressU[1] = clamp;
        AddressV[1] = clamp;

        VertexShader = compile vs_1_1 main();
    }
}

technique WdmLightning
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;

        ZWriteEnable = false;

        SrcBlend = srcalpha;
        DestBlend = one;
        CullMode = none;

        AlphaTestEnable = false;
        AlphaBlendEnable = true;

        ColorArg1[0] = texture;
        ColorOp[0] = SelectArg1;
        ColorOp[1] = disable;
        AlphaArg1[0] = diffuse;
        AlphaOp[0] = SelectArg1;
        AlphaOp[1] = disable;
    }
}

technique WdmRain
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;

        ZWriteEnable = false;

        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;
        CullMode = none;

        AlphaTestEnable = false;
        AlphaBlendEnable = true;

        ColorArg1[0] = texture;
        ColorOp[0] = SelectArg1;
        ColorOp[1] = disable;
        AlphaArg1[0] = texture;
        AlphaArg2[0] = diffuse;
        AlphaOp[0] = modulate;
        AlphaOp[1] = disable;
    }
}

technique WdmWarringShipClouds
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;

        ZWriteEnable = false;

        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;
        CullMode = none;

        AlphaTestEnable = false;
        AlphaBlendEnable = true;

        ColorArg1[0] = texture;
        ColorArg2[0] = diffuse;
        ColorOp[0] = modulate;
        ColorOp[1] = disable;
        AlphaArg1[0] = texture;
        AlphaArg2[0] = diffuse;
        AlphaOp[0] = modulate;
        AlphaOp[1] = disable;
    }
}

technique WdmShipWaterTrack
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;

        ZWriteEnable = false;

        SrcBlend = one;
        DestBlend = one;
        CullMode = none;

        AlphaTestEnable = false;
        AlphaBlendEnable = true;

        ColorArg1[0] = texture;
        ColorArg2[0] = diffuse;
        ColorOp[0] = modulate;
        ColorOp[1] = disable;
        AlphaOp[0] = disable;
    }
}

technique WdmDebugDraw
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;

        ZEnable = false;
        ZWriteEnable = false;

        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;
        CullMode = none;

        AlphaTestEnable = false;
        AlphaBlendEnable = true;

        ColorArg1[0] = texture;
        ColorArg2[0] = diffuse;
        ColorOp[0] = SelectArg2;
        ColorOp[1] = disable;
        AlphaOp[0] = disable;
    }
}

technique WdmDrawLabelIcon
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;

        ZEnable = false;
        ZWriteEnable = false;

        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;
        CullMode = none;

        AlphaTestEnable = false;
        AlphaBlendEnable = true;

        ColorArg1[0] = diffuse;
        ColorArg2[0] = texture;
        ColorOp[0] = selectarg2;
        AlphaArg1[0] = diffuse;
        AlphaArg2[0] = texture;
        AlphaOp[0] = selectarg2;

        ColorArg1[1] = texture;
        ColorArg2[1] = current;
        ColorOp[1] = blendfactoralpha;
        AlphaArg1[1] = texture;
        AlphaArg2[1] = current;
        AlphaOp[1] = blendfactoralpha;

        ColorOp[2] = disable;
        AlphaArg1[2] = diffuse;
        AlphaArg2[2] = current;
        AlphaOp[2] = modulate;

        AlphaOp[3] = disable;
    }
}

technique WdmDrawMapBlend
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;

        ZEnable = false;
        ZWriteEnable = false;

        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;
        CullMode = none;

        AlphaTestEnable = false;
        AlphaBlendEnable = true;

        ColorArg1[0] = texture;
        ColorArg2[0] = diffuse;
        ColorOp[0] = modulate;
        ColorOp[1] = disable;
        AlphaArg1[0] = texture;
        AlphaArg2[0] = diffuse;
        AlphaOp[0] = modulate;
        AlphaOp[1] = disable;
    }
}

technique WdmSeaDraw1
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;

        TextureTransformFlags[0] = count2;
        AddressU[0] = clamp;
        AddressV[0] = clamp;

        ZEnable = false;
        ZWriteEnable = false;

        AlphaTestEnable = false;
        AlphaBlendEnable = false;

        ColorArg1[0] = texture;
        ColorOp[0] = SelectArg1;
        ColorOp[1] = disable;
        AlphaOp[0] = disable;
    }
}

technique WdmSeaDraw2
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;

        TextureTransformFlags[0] = count2;
        AddressU[0] = clamp;
        AddressV[0] = clamp;

        ZEnable = false;
        ZWriteEnable = false;

        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;

        ColorArg1[0] = texture;
        ColorArg2[0] = tfactor;
        ColorOp[0] = modulate;
        ColorOp[1] = disable;
        AlphaArg2[0] = tfactor;
        AlphaOp[0] = SelectArg2;
        AlphaOp[1] = disable;
    }
}

technique WdmSeaDraw3
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;

        TextureTransformFlags[0] = count2;
        TextureTransformFlags[1] = count2;
        TexCoordIndex[0] = 0;
        TexCoordIndex[1] = 0;

        ZEnable = false;
        ZWriteEnable = false;

        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = one;

        ColorArg1[0] = texture;
        ColorOp[0] = SelectArg1;
        ColorArg0[1] = tfactor;
        ColorArg1[1] = texture;
        ColorArg2[1] = current;
        ColorOp[1] = lerp;
        ColorOp[2] = disable;
        AlphaArg2[0] = tfactor;
        AlphaOp[0] = SelectArg2;
        AlphaOp[1] = disable;
    }
}

technique WdmSeaDraw4
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;

        TextureTransformFlags[0] = count2;
        TextureTransformFlags[1] = count2;
        TexCoordIndex[0] = 0;
        TexCoordIndex[1] = 0;

        ZEnable = false;
        ZWriteEnable = false;

        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = one;
        DestBlend = one;

        ColorArg1[0] = texture;
        ColorOp[0] = SelectArg1;
        ColorArg1[1] = texture;
        ColorArg2[1] = current;
        ColorOp[1] = blendfactoralpha;
        ColorArg1[2] = current;
        ColorArg2[2] = tfactor;
        ColorOp[2] = subtract;
        ColorArg1[3] = current;
        ColorArg2[3] = current;
        ColorOp[3] = modulate4x;
        ColorOp[4] = disable;

        AlphaArg2[0] = tfactor;
        AlphaOp[0] = SelectArg2;
        AlphaArg2[1] = tfactor;
        AlphaOp[1] = SelectArg2;
        AlphaArg2[2] = tfactor;
        AlphaOp[2] = SelectArg2;
        AlphaOp[3] = disable;
    }
}

technique WdmSeaDrawFlare
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;

        ZEnable = true;
        ZWriteEnable = false;

        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = one;

        ColorArg1[0] = texture;
        ColorOp[0] = SelectArg1;
        ColorOp[1] = disable;

        AlphaArg1[0] = texture;
        AlphaArg2[0] = tfactor;
        AlphaOp[0] = modulate;
        AlphaOp[1] = disable;
    }
}

technique WdmInterfaceDrawBlend
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;

        ColorArg1[0] = texture;
        ColorArg2[0] = diffuse;
        ColorOp[0] = modulate;
        ColorOp[1] = disable;

        AlphaArg1[0] = texture;
        AlphaArg2[0] = diffuse;
        AlphaOp[0] = modulate;
        AlphaOp[1] = disable;

        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;
    }
}

technique WdmInterfaceDrawSky
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;

        ZEnable = false;
        ZWriteEnable = false;
        CullMode = none;

        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;

        ColorArg1[0] = texture;
        ColorOp[0] = SelectArg1;
        ColorArg1[1] = current;
        ColorOp[1] = SelectArg1;
        ColorOp[2] = disable;

        AlphaArg1[0] = texture;
        AlphaOp[0] = SelectArg1;
        AlphaArg1[1] = texture;
        AlphaArg2[1] = current;
        AlphaOp[1] = modulate;
        AlphaOp[2] = disable;
    }
}

technique WdmIslandWaves
{
    pass p0
    {
        Lighting = false;
        FogEnable = false;
        ZWriteEnable = false;

        AlphaTestEnable = false;
        AlphaBlendEnable = true;
        SrcBlend = srcalpha;
        DestBlend = invsrcalpha;

        TextureTransformFlags[0] = count2;
        AddressV[0] = clamp;

        ColorOp[0] = SelectArg1;
        ColorArg1[0] = texture;
        ColorOp[1] = disable;

        AlphaOp[0] = modulate;
        AlphaArg1[0] = tfactor;
        AlphaArg2[0] = texture;
        AlphaOp[1] = disable;
    }
}