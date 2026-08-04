#version 100

attribute vec4 inPosition;
attribute vec2 inTexCoord;
attribute vec4 inColor;

uniform mat4 matWVP;
uniform vec4 sectionPos;
uniform vec4 fogParam;

uniform vec3 cameraPos;

varying vec4 lightColor;
varying vec2 lightmapUV;
varying vec2 texCoord_texture;

float ComputeFog(vec3 camToWorldPos, vec3 param)
{
	float fdist = max(length(camToWorldPos) - param.x, 0.0);
	float density = clamp(clamp(fdist/(param.y-param.x), 0.0, 1.0) * param.z, 0.0, 1.0);
	return 1.0 - density;
}

void main(void)
{
	vec3 blockPos;
	blockPos = inPosition.xyz / 15.0;
	
	// output position.
	blockPos = sectionPos.xyz + blockPos;
	gl_Position = matWVP * vec4(blockPos, 1.0);

	texCoord_texture = inTexCoord / 2048.0;
	
	lightmapUV.y = inPosition.w / 255.0;
	lightmapUV.x = inColor.w / 255.0;

	lightColor.xyz = inColor.rgb / 255.0;;
	lightColor.w = ComputeFog(blockPos - cameraPos, fogParam.xyz);
}

