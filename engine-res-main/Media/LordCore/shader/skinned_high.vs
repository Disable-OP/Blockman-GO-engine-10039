#version 100

#define MAX_BONE_NUM 60

attribute vec4 inPosition;
attribute vec3 inNormal;
attribute vec2 inTexCoord;
attribute vec4 inBlendIndices;
attribute vec3 inBlendWeights;

uniform mat4 matWVP;
uniform mat4 matWorld;

uniform vec4 fogParam;
uniform vec3 cameraPos;
uniform vec3 mainLightDir;
uniform vec4 mainLightColor;
uniform vec4 subLightColor;
uniform vec4 brightness;
uniform vec4 boneMatRows[3*MAX_BONE_NUM];

varying vec4 lightColor;
varying vec2 texCoord;

vec3 MulBone3( vec3 vInputPos, int nMatrix, float fBlendWeight )
{
    vec3 vResult;
    vResult.x = dot( vInputPos, boneMatRows[3*nMatrix+0].xyz );
    vResult.y = dot( vInputPos, boneMatRows[3*nMatrix+1].xyz );
    vResult.z = dot( vInputPos, boneMatRows[3*nMatrix+2].xyz );
    return vResult * fBlendWeight;
}

vec3 MulBone4( vec4 vInputPos, int nMatrix, float fBlendWeight )
{
    vec3 vResult;
    vResult.x = dot( vInputPos, boneMatRows[(3*nMatrix)+0].xyzw );
    vResult.y = dot( vInputPos, boneMatRows[(3*nMatrix)+1].xyzw );
    vResult.z = dot( vInputPos, boneMatRows[(3*nMatrix)+2].xyzw );
    return vResult * fBlendWeight;
}

float ComputeFog(vec3 camToWorldPos, vec3 param)
{
	float fdist = max(length(camToWorldPos) - param.x, 0.0);
	float density = clamp(clamp(fdist/(param.y-param.x), 0.0, 1.0) * param.z, 0.0, 1.0);
	return 1.0 - density;
}

void main(void)
{
	vec3 vPos;
	vec3 vNorm;
	vec4 vWorldPos;
    ivec3 BoneIndices;

	BoneIndices.x = int(inBlendIndices.x);
	BoneIndices.y = int(inBlendIndices.y);
	BoneIndices.z = int(inBlendIndices.z);
	
  	vPos = MulBone4(inPosition, BoneIndices.x, inBlendWeights.x)
  	 + MulBone4(inPosition, BoneIndices.y, inBlendWeights.y)
  	 + MulBone4(inPosition, BoneIndices.z, inBlendWeights.z);
	
	vNorm = MulBone3(inNormal, BoneIndices.x, inBlendWeights.x)
	 + MulBone3(inNormal, BoneIndices.y, inBlendWeights.y)
	 + MulBone3(inNormal, BoneIndices.z, inBlendWeights.z);
	// blend vertex position & normal
	
	vWorldPos = matWorld * vec4(vPos, 1.0);
	
	gl_Position = matWVP * vec4(vPos, 1.0);
	texCoord = inTexCoord;
	
	vNorm = normalize(mat3(matWorld)*vNorm);
	
	float mainParam = max(dot(mainLightDir, vNorm), 0.0);
	float subParam = max(dot(-mainLightDir, vNorm), 0.0);
	
	lightColor = mainParam * mainLightColor + subParam * subLightColor + vec4(0.6);
	lightColor = lightColor * brightness;
	lightColor.w = ComputeFog(vWorldPos.xyz - cameraPos, fogParam.xyz);
}
