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

out vec4 gPosition;
out vec4 gNormal;
out vec4 gAlbedoSpec;

void main()
{
	// Store the fragment position vector in the first gbuffer texture
	gPosition = vec4(fragPosOut, 1.0);
	
	// Sample the normal from the normal map
	vec3 normalTex = texture(normalMap, texturesOut).xyz;
	normalTex = normalize(normalTex * 2.0 - 1.0); // Convert from [0, 1] to [-1, 1]
	
	// Tangent space to world space transformation for normals
	mat3 TBN = mat3(tangentOut, bitangentOut, normalOut);
	vec3 normal = normalize(TBN * normalTex);
	
	// Store the normal vector in the second gbuffer texture
	gNormal = vec4(normal, 1.0);
	
	// Store the fragment's albedo color and specular intensity
	vec4 albedo = texture(textureArray, texturesOut);
	gAlbedoSpec = vec4(albedo.rgb, specularIntensity);
}
