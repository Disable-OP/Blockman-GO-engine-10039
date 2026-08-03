#version 100

#define MAX_BONE_NUM 60

attribute vec4 inPosition;
attribute vec2 inTexCoord;
attribute vec4 inColor;    // rgb = color a = index

uniform mat4 matWVP;
uniform mat4 matWorld;
uniform vec4 fogParam;
uniform vec3 cameraPos;
uniform vec4 extraColor;

uniform vec4 boneMatRows[3*MAX_BONE_NUM];

varying vec4 lightColor;
varying vec2 texCoord;

vec3 MulBone3(vec3 vInputPos, int nMatrix)
{
    vec3 vResult;
    vResult.x = dot( vInputPos, boneMatRows[3*nMatrix+0].xyz );
    vResult.y = dot( vInputPos, boneMatRows[3*nMatrix+1].xyz );
    vResult.z = dot( vInputPos, boneMatRows[3*nMatrix+2].xyz );
    return vResult;
}

vec3 MulBone4(vec4 vInputPos, int nMatrix)
{
    vec3 vResult;
    vResult.x = dot( vInputPos, boneMatRows[(3*nMatrix)+0].xyzw );
    vResult.y = dot( vInputPos, boneMatRows[(3*nMatrix)+1].xyzw );
    vResult.z = dot( vInputPos, boneMatRows[(3*nMatrix)+2].xyzw );
    return vResult;
}

float ComputeFog(vec3 camToWorldPos, vec3 param)
{
	float fdist = max(length(camToWorldPos) - param.x, 0.0);
	float density = clamp(clamp(fdist/(param.y-param.x), 0.0, 1.0) * param.z, 0.0, 1.0);
	return 1.0 - density;
}

void main(void)
{
	vec3 blockPos = (inPosition.xyz / 127.0 - 1.0) * 8.0;
	texCoord = inTexCoord / 2048.0;
	vec3 color = inColor.rgb / 255.0;
	color *= extraColor.rgb;
	int BoneIndices = int(inColor.a);
		
	vec3 vPos = MulBone4(vec4(blockPos, 1.0), BoneIndices);
	vec4 vWorldPos = matWorld * vec4(vPos, 1.0);
	gl_Position = matWVP * vec4(vPos, 1.0);

	lightColor.xyz = color;	

	lightColor.w = ComputeFog(vWorldPos.xyz - cameraPos, fogParam.xyz);
}

