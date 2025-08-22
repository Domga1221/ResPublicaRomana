#version 450 core
layout(location = 0) out vec4 FragColor;

in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

// material parameters
layout(location = 4) uniform sampler2D albedoMap;
layout(location = 5) uniform sampler2D normalMap;
layout(location = 6) uniform sampler2D metallicMap;
layout(location = 7) uniform sampler2D roughnessMap;
layout(location = 8) uniform sampler2D aoMap;

// lights
layout(location = 9) uniform vec3 lightPositions[4];
layout(location = 13) uniform vec3 lightColors[4];
layout(location = 17) uniform int lightCount;

// irradiance map for IBL
layout(location = 18) uniform samplerCube irradianceMap;
// specular IBL
layout(location = 19) uniform samplerCube prefilterMap;
layout(location = 20) uniform sampler2D brdfLUT;

layout(location = 21) uniform vec3 camPos;

// shadow map
layout(location = 22) uniform sampler2D shadowMap;
layout(location = 23) uniform vec3 directionalLightPos;
layout(location = 24) uniform bool applyShadow;
in vec4 FragPosLightSpace;

// tone mapping color correct
layout(location = 25) uniform bool colorCorrect;

// SSAO Blur texture
layout(location = 26) uniform sampler2D SSAOBlurTexture;
layout(location = 27) uniform bool applySSAO;
layout(location = 28) uniform vec3 u_resolution;

const float PI = 3.14159265359;



// muss ich mir nochmal anschauen
// ----------------------------------------------------------------------------
// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
// Don't worry if you don't get what's going on; you generally want to do normal 
// mapping the usual way for performance anyways; I do plan make a note of this 
// technique somewhere later in the normal mapping tutorial.
vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normalMap, TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(WorldPos);
    vec3 Q2  = dFdy(WorldPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}   
// ----------------------------------------------------------------------------


// shadow - 1.0 when in shadow, 0.0 when not in shadow
float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal) {
    // transform light space fragment position from light space into clip space by perspective division
    // only necessary when using a perspective projection
    // output is in range from -1 to 1
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // transform to NDC range 0 to 1
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r;

    float currentDepth = projCoords.z;

    vec3 directionalLightDir = normalize(directionalLightPos - WorldPos);
    float bias = max(0.05 * (1.0 - dot(normal, directionalLightDir)), 0.005);  
    
    // no PCF
    //float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    // with PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    // 0.0 when outside the far_plane region of the light's frustum
    if(projCoords.z > 1.0) {
        shadow = 0.0;
    }

    return shadow;
}

void main()
{		
    vec3 albedo = vec3(texture(albedoMap, TexCoords));
	FragColor = vec4(albedo, 1.0);

	// PBR STUFFS
	albedo = pow(texture(albedoMap, TexCoords).rgb, vec3(2.2));
	float metallic = texture(metallicMap, TexCoords).r;
	float roughness = texture(roughnessMap, TexCoords).r;
	float ao = texture(aoMap, TexCoords).r; 

    vec3 N = getNormalFromMap();
    vec3 V = normalize(camPos - WorldPos);
    vec3 R = reflect(-V, N);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < lightCount; ++i) 
    {
        // calculate per-light radiance
        vec3 L = normalize(lightPositions[i] - WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(lightPositions[i] - WorldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lightColors[i] * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
           
        vec3 numerator    = NDF * G * F; 
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;
        
        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;	  

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }   
    
    // ambient lighting (note that the next IBL tutorial will replace 
    // this ambient lighting with environment lighting).
    //vec3 ambient = vec3(0.03) * albedo * ao;

    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness); 

    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic; // soll die zeile da rein??? keine ahnung LearnOpenGL wei� es auch nicht :)
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse    = irradiance * albedo;

    // sample prefilter mpa and BRDF lut, combine them like in Split-Sum approximation to get specular component
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
    vec2 brdf = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);


    vec3 ambient    =  (kD * diffuse + specular) * ao; 
    
    //shadow
    if(applyShadow) {
        float shadow = ShadowCalculation(FragPosLightSpace, N);
        shadow = clamp(shadow, 0.0, 0.8);
        ambient *= (1.0 - shadow);
    }

    float ssaoBlurValue = texture(SSAOBlurTexture, gl_FragCoord.xy / u_resolution.xy).r;
    if(applySSAO) {
        //ivec2 fragCoord = ivec2(int(gl_FragCoord.x), int(gl_FragCoord.y));
        //float SSAOBlurValue = texelFetch(SSAOBlurTexture, fragCoord, 0).r;
        ambient *= ssaoBlurValue;
    }

    vec3 color = ambient + Lo;

    if (colorCorrect) {
        // HDR tonemapping
        color = color / (color + vec3(1.0));
        // gamma correct
        color = pow(color, vec3(1.0/2.2)); 
    }

    //color = Lo;
    FragColor = vec4(color, 1.0);
    //FragColor = vec4(vec3(ssaoBlurValue), 1.0);
    //FragColor = vec4(irradiance, 1.0);
    //FragColor = vec4(vec3(roughness), 1.0);
    //FragColor = vec4(vec3(shadow), 1.0);
    //FragColor = vec4(vec3(texture(shadowMap, TexCoords).r), 1.0);
    //FragColor = vec4(vec3(Lo), 1.0);
    //FragColor = texture(albedoMap, TexCoords);
    //FragColor = texture(metallicMap, TexCoords);
    //FragColor = vec4(vec3(kD), 1.0);
    //FragColor = vec4(texture(brdfLUT, TexCoords).rgb, 1.0);
}