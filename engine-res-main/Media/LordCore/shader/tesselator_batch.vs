#version 100

attribute vec3 inPosition;
attribute vec2 inTexCoord;
attribute vec4 inColor; 

uniform mat4 matWVP;
uniform vec4 fogParam;
uniform vec3 cameraPos;

varying vec4 lightColor;
varying vec2 texCoord;

float ComputeFog(vec3 camToWorldPos, vec3 param)
{
	float fdist = max(length(camToWorldPos) - param.x, 0.0);
	float density = clamp(clamp(fdist/(param.y-param.x), 0.0, 1.0) * param.z, 0.0, 1.0);
	return 1.0 - density;
}

void main(void)
{
	gl_Position = matWVP * vec4(inPosition, 1.0);

	texCoord = inTexCoord;
	lightColor.xyz = inColor.xyz;
	lightColor.w = ComputeFog(inPosition - cameraPos, fogParam.xyz);
}


