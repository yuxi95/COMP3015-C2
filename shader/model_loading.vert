#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 normal;
out vec2 TexCoords;
out vec3 position;
out vec3 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

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

uniform sampler2D shadowMap;


uniform vec3 lightPosition;

uniform vec3 movingLightPosition;

uniform vec3 eye;

uniform bool toggleDiffuse;
uniform bool toggleSpecular;

uniform bool toggleToonShading;

uniform bool toggleSpotLight;

uniform float shininess;

uniform bool toggleFlatShading;



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

void main()
{
    position = (model * vec4(aPos, 1.0)).xyz;
    


    TexCoords = aTexCoords;
    normal = (model * vec4(aNormal, 0.0)).xyz;
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    if (toggleFlatShading)
    {
        vec3 n = normalize(normal);

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

        color = (diffuse + specular) * materialColor;
    }
}