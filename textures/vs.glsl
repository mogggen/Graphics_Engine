#version 430

layout(location=0) in vec3 inPosition;
layout(location=1) in vec4 inTangent; // This is a 4D vector, the fourth component is the handedness (bitangent sign)
layout(location=2) in vec2 inTextureCoord;
layout(location=3) in vec3 inNormal;

layout(location=0) out vec2 texturesOut;
layout(location=1) out vec3 fragPosOut;
layout(location=2) out vec3 normalOut;
layout(location=3) out vec3 tangentOut;
layout(location=4) out vec3 bitangentOut;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	// Transform vertex attributes to view space
	vec4 viewPos = view * model * vec4(inPosition, 1.0);
	fragPosOut = viewPos.xyz;
	texturesOut = inTextureCoord;

	// Transform normal to view space
	vec4 viewNormal = view * vec4(inNormal, 0.0);
	normalOut = normalize(viewNormal.xyz);

	// Transform tangent and bitangent to view space
	vec4 viewTangent = view * model * inTangent;
	tangentOut = normalize(viewTangent.xyz);
	
	// Calculate bitangent using the handedness
	vec3 viewBitangent = cross(normalOut, tangentOut) * inTangent.w;
	bitangentOut = normalize(viewBitangent);
	
	// Calculate the final vertex position in clip space
	gl_Position = projection * viewPos;
}
