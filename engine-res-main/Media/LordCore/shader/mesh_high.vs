#version 100

attribute vec3 inPosition;
attribute vec3 inNormal;
attribute vec2 inTexCoord;

uniform mat4 matWorld;
uniform mat4 matWVP;

uniform vec4 fogParam;
uniform vec3 cameraPos;
uniform vec3 mainLightDir;
uniform vec4 mainLightColor;
uniform vec4 subLightColor;
uniform vec4 brightness;

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
	vec4 vWorldPos = matWorld * vec4(inPosition, 1.0);
	gl_Position = matWVP * vec4(inPosition, 1.0);
		
	texCoord = inTexCoord;
	
	vec3 vNorm = normalize(mat3(matWorld) * inNormal);
	
	float mainParam = max(dot(mainLightDir, vNorm), 0.0);
	float subParam = max(dot(-mainLightDir, vNorm), 0.0);
	
	lightColor = mainParam * mainLightColor + subParam * subLightColor + vec4(0.6);
	lightColor = lightColor * brightness;
	lightColor.w = ComputeFog(vWorldPos.xyz - cameraPos, fogParam.xyz);
}
