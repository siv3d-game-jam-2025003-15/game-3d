//	Copyright (c) 2008-2023 Ryo Suzuki.
//	Copyright (c) 2016-2023 OpenSiv3D Project.
//	Licensed under the MIT License.

# version 410

//
//	Textures
//
uniform sampler2D Texture0;

struct Light
{
    vec4 position;
	vec4 diffuseColor;
	vec4 attenuation;
};

//
//	PSInput
//
layout(location = 0) in vec3 WorldPosition;
layout(location = 1) in vec2 UV;
layout(location = 2) in vec3 Normal;

//
//	PSOutputg_pointLightPos
//
layout(location = 0) out vec4 FragColor;

//
//	Constant Buffer
//
layout(std140) uniform PSPerFrame // slot 0
{
	vec3 g_globalAmbientColor;
	vec3 g_sunColor;
	vec3 g_sunDirection;
};

layout(std140) uniform PSPerView // slot 1
{
	vec3 g_eyePosition;
};

layout(std140) uniform PSPointLightPos // slot 2
{
	vec3 g_pointLightPos;
	float g_pointLightStrong;
};

layout(std140) uniform PSPerMaterial // slot 3
{
	vec3  g_ambientColor;
	uint  g_hasTexture;
	vec4  g_diffuseColor;
	vec3  g_specularColor;
	float g_shininess;
	vec3  g_emissionColor;
};

const uint MaxPointLights = 4;
layout(std140) uniform PSLighting // slot 4
{
    Light g_lights[MaxPointLights];
};

//
//	Functions
//
vec3 CalculatePointLight(uint index, vec3 surfaceNormal, vec3 surfacePosition) {
    Light light = g_lights[index];
    vec3 lightPosition = light.position.xyz;
    vec3 lightDirection = lightPosition - surfacePosition;
    float d = length(lightDirection);
    float Kc = light.attenuation.x;
    float Kl = light.attenuation.y;
    float Kq = light.attenuation.z;
    float f_att = 1.0 / (Kc + Kl * d + Kq * d * d);
    //float f_att = 1.0 / (Kc + Kl * d );
    lightDirection = normalize(lightDirection);
    float diffuseInfluence = max(dot(lightDirection, surfaceNormal), 0.0f) * f_att;
    return light.diffuseColor.rgb * diffuseInfluence;
}

vec4 GetDiffuseColor(vec2 uv)
{
	vec4 diffuseColor = g_diffuseColor;

	if (g_hasTexture == 1)
	{
		diffuseColor *= texture(Texture0, uv);
	}

	return diffuseColor;
}

vec3 CalculateDiffuseReflection(vec3 n, vec3 l, vec3 lightColor, vec3 diffuseColor, vec3 ambientColor)
{
	vec3 directColor = lightColor * max(dot(n, l), 0.0f);
	return (ambientColor + directColor) * diffuseColor;
}


vec3 CalculateSpecularReflection(vec3 n, vec3 h, float shininess, float nl, vec3 lightColor, vec3 specularColor)
{
	float highlight = pow(max(dot(n, h), 0.0f), shininess) * float(0.0f < nl);
	return (lightColor * specularColor * highlight);
}

void main()
{
	// Setting the light color and direction
	vec3 lightColor		= g_sunColor;
//	vec3 lightDirection	= normalize(g_pointLightPos - WorldPosition);
    vec3 lightDirection = g_sunDirection;

	// Obtaining color information from normal vectors and UVs
	vec3 n = normalize(Normal);
	vec3 l = lightDirection;
	vec4 diffuseColor = GetDiffuseColor(UV);
	vec3 ambientColor = g_ambientColor * g_globalAmbientColor;

	// Diffuse
	vec3 diffuseReflection = CalculateDiffuseReflection(n, l, lightColor, diffuseColor.rgb, ambientColor);

	for (uint i = 0u; i < MaxPointLights; ++i) {
        diffuseReflection += diffuseColor.rgb * CalculatePointLight(i, n, WorldPosition);
    }

	// Specular
	vec3 v = normalize(g_eyePosition - WorldPosition);
	vec3 h = normalize(v + lightDirection);
	vec3 specularReflection = CalculateSpecularReflection(n, h, g_shininess, dot(n, l), lightColor, g_specularColor);

	// Distance attenuation
//	float attenuation = 1.0 / (1.0 + 0.1 * pow(length(g_pointLightPos - WorldPosition), g_pointLightStrong));
//	float attenuation = 1.0;

	// Pixel discard due to alpha (transparency)
    vec4 texColor = texture(Texture0,UV);
    if (texColor.a < 0.1)
    {
        discard;
    }

//	FragColor = vec4(diffuseReflection*attenuation + specularReflection*attenuation + g_emissionColor, diffuseColor.a);
	FragColor = vec4(diffuseReflection + specularReflection + g_emissionColor, diffuseColor.a);
}
