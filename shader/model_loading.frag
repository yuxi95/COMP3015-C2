#version 430 core

out vec4 FragColor;

in vec3 position;
in vec2 TexCoords;
in vec3 normal;
in vec3 color;


uniform vec3 albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;




struct Light
{
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float constant;
    float linear;
    float quadratic;
};

uniform Light spotLight;

// uniform vec3 ambient;
uniform vec3 materialColor = vec3(1.0, 1.0, 1.0);

uniform sampler2D albedoMap;



uniform vec3 lightPosition;

uniform vec3 movingLightPosition;

uniform vec3 eye;

uniform bool toggleDiffuse;
uniform bool toggleSpecular;

uniform bool toggleToonShading;

uniform bool toggleSpotLight;

uniform float shininess;

uniform bool toggleFlatShading;

uniform bool toggleFog;

uniform float fogDensity = 0.001;

uniform vec3 fogColor = vec3(1.0);

vec3 toonShading(vec3 lightDirection, vec3 normal)
{
    float intensity;
	vec3 color;
	intensity = max(0.0, dot(lightDirection, normal));

	if (intensity > 0.7)
		color = vec3(1.0, 0.5, 0.5);
	else if (intensity > 0.5)
		color = vec3(0.6, 0.3, 0.3);
	else if (intensity > 0.25)
		color = vec3(0.4, 0.2, 0.2);
	else
		color = vec3(0.2, 0.1, 0.1);
	return color;
}

float diffuseLighting(vec3 lightDirection, vec3 normal)
{
    return max(dot(lightDirection, normal), 0.0);
}

float specularighting(vec3 lightDirection,vec3 viewDirection,vec3 normal, float shininess)
{
    vec3 h = normalize(lightDirection + viewDirection);

    return pow(max(dot(lightDirection, normal), 0.0), shininess);
}

const float PI = 3.14159265359;
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

 

void main()
{   
 

    vec3 albedo = texture(albedoMap, TexCoords).rgb;

    vec3 ambient = albedo * 0.3;

    if (toggleFlatShading)
    {
        FragColor = vec4(ambient + color * albedo , 1.0);
    }
    else
    {
        vec3 n = normalize(normal);

        vec3 ambient = albedo * 0.3;

        vec3 l1 = normalize(-vec3(0.0, -1.0, 1.0));

        vec3 l2 = normalize(lightPosition - position);
        
        vec3 l3 = normalize(movingLightPosition - position);

        float diffuse1 = 0.0;
        float diffuse2 = 0.0;
        float diffuse3 = 0.0;
        
        if (toggleDiffuse)
        {
            diffuse1 = diffuseLighting(l1, n);
            diffuse2 = diffuseLighting(l2, n);
            diffuse3 = diffuseLighting(l3, n);
        }

        vec3 v = normalize(eye - position);

        float specular1 = 0.0;
        float specular2 = 0.0;
        float specular3 = 0.0;

        if (toggleSpecular)
        {
            specular1 = specularighting(l1, v, n, shininess);
            specular2 = specularighting(l2, v, n, shininess);
            specular3 = specularighting(l3, v, n, shininess);
        }

        float diffuse = (diffuse1 + diffuse1 + diffuse2) * 0.33;
        float specular = (specular1 + specular2 + specular3) * 0.33;

        if (toggleSpotLight)
        {
            // spotlight (soft edges)
            vec3 lightDir = normalize(spotLight.position - position);
            float theta = dot(lightDir, normalize(-spotLight.direction)); 
            float epsilon = (spotLight.cutOff - spotLight.outerCutOff);
            float intensity = clamp((theta - spotLight.outerCutOff) / epsilon, 0.0, 1.0);
            diffuse  *= intensity;
            specular *= intensity;
        }

        vec3 finalColor = ambient + diffuse * albedo * materialColor + specular * materialColor ;
        
        FragColor = vec4(finalColor, 1.0);

        if (toggleToonShading)
        {
            FragColor = vec4(finalColor * toonShading(l2, normal), 1.0);
        }
    }



    if (toggleFog)
    {
        float distance = length(gl_FragCoord.xyz);

        float fogFactor = exp(-fogDensity * distance);

        FragColor = mix(vec4(fogColor, 1.0), FragColor, fogFactor);
    }
}