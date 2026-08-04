/********************************************************************
filename: 	Core.h
file path:	dev\engine\Src\Core\Common\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_CORE_H__
#define __LORD_CORE_H__

#include "LordConfig.h"
#include "CoreDef.h"
#include "TypeDef.h"


namespace LORD
{

	class PathUtil;
	class StringUtil;
	class Time;
	class Vector2;
	class Vector3;
	class Vector4;
	class Matrix4;
	class Vector2i;
	class Vector3i;
	class Vector4i;
	class Quaternioni;
	class Matrix3i;
	class Matrix4i;
	class Quaternion;
	class Triangle;
	class Circle;
	class ERect;
	class Sphere;
	class Plane;
	class Frustum;
	class Ray;
	class ParamSurface;
	class SimpleSpline;
	class QueryObject;
	class QueryObjectManager;
	class Root;
	class ConfigFile;
	class Color;
	class Box;
	class PixelBox;
	class Image;
	class ImageCodec;
	class ImageCodecMgr;
	class GPUBuffer;
	class Shader;
	class ShaderProgram;
	class RenderState;
	class Texture;
	class TextureManager;
	class RenderInput;
	class RenderView;
	class RenderTargetView;
	class DepthStencilView;
	class Viewport;
	class FrameBuffer;
	class TextureModify;
	class Material;
	class Renderer;
	class ManualRenderListener;
	class Renderable;
	class RenderQueue;
	class Bone;
	class Skeleton;
	class SkeletonManager;
	class Animation;
	class AnimState;
	class AnimKeyFrame;
	class AnimTrack;
	class SRTTrack;
	class AnimManager;
	class AnimBlender;
	class Mesh;
	class SubMesh;
	class SkinnedMesh;
	class MeshManager;
	class SceneNode;
	class SceneManager;
	class Camera;
	class ModelEntity;
	class SkinedEntity;
	class GameObject;
	class Zoom;
	class Scene;
	class TerrainBlock;
	class DataStream;
	class ResourceManager;
	class ResourceGroupManager;
	class Resource;
	class Archive;
	class ArchiveFactory;
	class ArchiveManager;
	class ZipArchive;
	class ZipArchiveFactory;
	class FileSystemArchive;
	class FileSystemArchiveFactory;
	class APKFileSystemArchive;
	class APKFileSystemArchiveFactory;
	class ResourcePack;
	class EffectController;
	class EffectTranslation;
	class EffectRotationSelf;
	class EffectRotationAxis;
	class EffectRotationRevolution;
	class EffectOffcenter;
	class EffectColorChange;
	class EffectScaleChange;
	class EffectScaleNoise;
	class EffectColorNoise;
	class EffectColorSet;
	class EffectScaleSet;
	class EffectKeyFrame;
	class EffectLayer;
	class EffectLayer2DBillBoard;
	class EffectLayer3DBillBoard;
	class EffectLayerColumn;
	class EffectLayerTrail;
	class EffectLayer3DGrid;
	class EffectLayer2DGrid;
	class EffectLayerLighting;
	class EffectLayerParaboloidal;
	class EffectLayerParticles;
	class EffectLayerParticlesPoint;
	class EffectLayerParticlesCube;
	class EffectLayerParticlesSphere;
	class EffectLayerParticlesCylinder;
	class EffectLayerParticlesCurve;
	class EffectLayerModel;
	class EffectLayerSkin;
	class EffectMaterial;
	class EffectParticle;
	class EffectParticleControllerSet;
	class EffectRenderable;
	class EffectSystem;
	class EffectSystemManager;
	class TessSystem;
	class TessMaterial;
	class TessRenderable;
	class SkinedRenderable;
	class RasterizerRenderable;
	class ItemRenderMesh;
	class ItemRenderable;
	class FxRenderable;
	class FontRenderable;
	class BatchRenderable;
	class EnchantSkinedRenderable;
	class LineRenderable;
	class TessObject;
	class SkillAttack;
	class SkillAttackManager;
	class SkillAnimSystem;
	class SkillSystem;
	class ActorTemplateDataManager;
	class ActorObject;
	class ActorManager;
	class TextureSet;
	class LightmapMgr;
	class LightmapRasterizer;
	class VuforiaRender;
}

#include "CoreStd.h"
#include "Memory/LordMemory.h"

// Note - not in the original STL, but exists in SGI STL and STLport
// For gcc 4.3 see http://gcc.gnu.org/gcc-4.3/changes.html
#if (LORD_COMPILER == LORD_COMPILER_GNUC) && !defined(STLPORT)
#   if (LORD_COMPILER_VERSION >= 430)
#       include <tr1/unordered_map>
#       include <tr1/unordered_set>
#   else
#       include <ext/hash_map>
#       include <ext/hash_set>
#   endif
#elif (LORD_COMPILER == LORD_COMPILER_CLANG)
#   if defined(_LIBCPP_VERSION)
#       include <unordered_map>
#       include <unordered_set>
#   else
#       include <tr1/unordered_map>
#       include <tr1/unordered_set>
#       include <tr1/functional>
#   endif
#else
#   if ((LORD_COMPILER == LORD_COMPILER_MSVC) && !defined(STLPORT) && (LORD_COMPILER_VERSION >= 100)) // VC++ 10.0
#    	include <unordered_map>
#    	include <unordered_set>
#	else
#   	include <hash_set>
#   	include <hash_map>
#	endif
#endif


#include "Util/PathUtil.h"
#include "Util/StringUtil.h"
#include "Util/AssertX.h"
#include "Object/Exception.h"
#include "Log/Log.h"
#include "Resource/Threading.h"
#include "Math/MathLib.h"
#include "Geom/GeomLib.h"
#include "Object/Singleton.h"
#include "Object/Time.h"
#include "Resource/DataStream.h"
#include "Resource/ResourceManager.h"
#include "Resource/ResourceGroupManager.h"
#include "Resource/Resource.h"
#include "Resource/Archive.h"
#include "Resource/ArchiveFactory.h"
#include "Resource/ArchiveManager.h"
#include "Resource/FileSystem.h"

#include "Image/Color.h"
#include "Image/PixelFormat.h"
#include "Image/PixelConversions.h"
#include "Image/Image.h"
#include "Image/ImageCodec.h"
#include "Image/ImageCodecMgr.h"
#include "Render/GPUBuffer.h"
#include "Render/Shader.h"
#include "Render/ShaderProgram.h"
#include "Render/RenderState.h"
#include "Render/Texture.h"
#include "Render/RenderInput.h"
#include "Render/RenderView.h"
#include "Render/RenderTargetView.h"
#include "Render/DepthStencilView.h"
#include "Render/Viewport.h"
#include "Render/FrameBuffer.h"
#include "Render/Renderer.h"
#include "Render/TextureManager.h"
#include "Render/Renderable.h"
#include "Render/RenderQueue.h"
#include "Effect/EffectHeader.h"
#include "Tessolator/TessHeader.h"
#include "Scene/Camera.h"
#include "Scene/CameraMain.h"
#include "Scene/SceneNode.h"
#include "Scene/SceneManager.h"
#include "Scene/TextureSet.h"
#endif
