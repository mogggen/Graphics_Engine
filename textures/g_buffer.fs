#version 430
layout(location=0) in vec2 texturesOut;
layout(location=1) in vec3 fragPosOut;
layout(location=2) in vec3 normalOut;
layout(location=3) in vec3 tangentOut; // Tangent from the vertex shader
layout(location=4) in vec3 bitangentOut; // Bitangent from the vertex shader

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform float lightIntensity;

uniform vec3 camPos;
uniform float specularIntensity;
uniform sampler2D textureArray;
uniform sampler2D normalMap; // Your normal map texture

out vec4 Color;

void main()
{
	vec3 viewDir = normalize(camPos - fragPosOut);
	vec3 lightDir = normalize(lightPos - fragPosOut);

	// Sample the normal from the normal map
	vec3 normalTex = texture(textureArray, texturesOut).xyz;
	normalTex = normalize(normalTex * 2.0 - 1.0); // Convert from [0, 1] to [-1, 1]

	// Tangent space to world space transformation for normals
	mat3 TBN = mat3(tangentOut, bitangentOut, normalOut);
	vec3 normal = normalize(TBN * normalTex); // Transform normal from tangent to world space

	vec3 halfwayDir = normalize(lightDir + viewDir);
	vec3 ambientLight = lightIntensity * lightColor;

	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	float spec = pow(max(dot(normal, halfwayDir), 0.0), 64);
	vec3 specular = lightColor * spec * specularIntensity;

	Color = texture(normalMap, texturesOut) * vec4(ambientLight + diffuse + specular, 1.0);
}
