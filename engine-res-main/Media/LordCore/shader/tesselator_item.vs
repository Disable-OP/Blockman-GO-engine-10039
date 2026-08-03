#version 100

attribute vec4 inPosition;
attribute vec2 inTexCoord;
attribute vec4 inColor;

uniform mat4 matWVP;
uniform mat4 matWorld;

uniform vec4 fogParam;
uniform vec3 cameraPos;
uniform vec4 extraColor;

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
	vec3 blockPos = (inPosition.xyz / 127.0 - 1.0) * 2.0;
	vec4 vWorldPos = matWorld * vec4(blockPos, 1.0);
			
	texCoord = inTexCoord / 2048.0;
	
	gl_Position = matWVP * vec4(blockPos, 1.0);
	
	// lighting
	lightColor.xyz = inColor.rgb * extraColor.rgb;
	lightColor.w = ComputeFog(vWorldPos.xyz - cameraPos, fogParam.xyz);
}
