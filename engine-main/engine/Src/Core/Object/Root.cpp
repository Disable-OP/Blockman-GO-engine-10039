#include "Root.h"
#include "Util/PathUtil.h"
#include "Util/Encoder.h"
#include "Util/QueryObject.h"
#include "Exception.h"
#include "Resource/ArchiveManager.h"
#include "Resource/FileSystem.h"
#include "Resource/ZipArchive.h"
#include "Resource/APKFileSystem.h"
#include "Resource/ResourceGroupManager.h"
#include "Resource/LordThread.h"
#include "Audio/SoundSystem.h"
#include "Image/ImageCodecMgr.h"
#include "Model/MeshManager.h"
#include "Anim/AnimManager.h"
#include "Anim/SkeletonManager.h"
#include "Scene/SceneManager.h"
#include "Effect/EffectSystemManager.h"
#include "Tessolator/TessManager.h"
#include "Time.h"
#include "ConfigFile.h"
#include "Render/Viewport.h"
#include "Actor/ActorManager.h"
#include "Scene/LightmapMgr.h"
#include "Render/RenderStage.h"
#include "UI/GUIImagesetManager.h"
#include "UI/GUIWindowManager.h"
#include "UI/Touch.h"

#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "rapidxml_print.hpp"

#include "rapidjson/document.h"

using namespace rapidxml;

#ifdef LORD_INCLUDE_GUI
#include "UI/GUISystem.h"
#endif

namespace LORD
{

	const String DeviceFeature::cs_etc1_format = "GL_OES_compressed_ETC1_RGB8_texture";
	const String DeviceFeature::cs_pvr_format = "GL_IMG_texture_compression_pvrtc";
	const String DeviceFeature::cs_atitc_format = "GL_ATI_texture_compression_atitc";
	const String DeviceFeature::cs_atitc_format2 = "GL_AMD_compressed_ATC_texture";
	const String DeviceFeature::cs_dxt1_format = "GL_EXT_texture_compression_dxt1";
	const String DeviceFeature::cs_s3tc_format = "GL_EXT_texture_compression_s3tc";
	const String DeviceFeature::cs_s3tc_format2 = "GL_OES_texture_compression_S3TC";
	const String DeviceFeature::cs_half_float_texture = "GL_OES_texture_half_float";
	const String DeviceFeature::cs_half_float_texture_linear = "GL_OES_texture_half_float_linear";


	DeviceFeature::DeviceFeature()
	{

	}

	DeviceFeature::~DeviceFeature()
	{
		m_features.clear();
	}

	void DeviceFeature::checkTextureFormat(const String& features)
	{
		if (features.find(DeviceFeature::cs_etc1_format) != String::npos)
		{
			m_supportETC1 = true;
		}

		if (features.find(DeviceFeature::cs_atitc_format) != String::npos
			|| features.find(DeviceFeature::cs_atitc_format2) != String::npos)
		{
			m_supportATITC = true;
		}

		if (features.find(DeviceFeature::cs_dxt1_format) != String::npos
			|| features.find(DeviceFeature::cs_s3tc_format) != String::npos
			|| features.find(DeviceFeature::cs_s3tc_format2) != String::npos)
		{
			m_supportDXT1 = true;
		}

		if (features.find(DeviceFeature::cs_pvr_format) != String::npos)
		{
			m_supportPVR = true;
		}

		if (features.find(DeviceFeature::cs_half_float_texture) != String::npos)
		{
			m_supportHalfFloatTexture = true;

			if (features.find(DeviceFeature::cs_half_float_texture_linear) != String::npos)
			{
				m_supportHalfFloatTextureLinear = true;
			}
		}
	}

	bool DeviceFeature::supportPVR() const
	{
		return m_supportPVR;
	}

	bool DeviceFeature::supportATITC() const
	{
		return m_supportATITC;
	}

	bool DeviceFeature::supportDXT1() const
	{
		return m_supportDXT1;
	}

	bool DeviceFeature::supportETC1() const
	{
		return m_supportETC1;
	}

	bool DeviceFeature::supportHFTexture() const
	{
		return m_supportHalfFloatTexture;
	}

	bool DeviceFeature::supportHFTextureLinear() const
	{
		return m_supportHalfFloatTextureLinear;
	}

	void DeviceFeature::initialize()
	{
		//m_seperateSampleAlpha = false;
		m_supportETC1 = false;
		m_supportPVR = false;
		m_supportDXT1 = false;
		m_supportATITC = false;
		m_supportHalfFloatTexture = false;
		m_supportHalfFloatTextureLinear = false;
	}

	void FrameState::output()
	{
#ifdef LORD_INCLUDE_GUI
		/*ui32 temp_dp = m_drawCallTimes;
		ui32 temp_trangles = m_triangleNum;
		ui32 temp_vertex_size = m_uploadedGeometricSize;
		ui32 temp_texture_size = m_uploadedTextureSizeInBytes;
		GUIString strUIString;
		m_fpsMsg = StringUtil::Format("FPS: %d, MaxFrame: %d", Root::Instance()->getFPS(), Root::Instance()->getMaxFrameTime());
		strUIString = m_fpsMsg.c_str();
		Renderer::Instance()->renderText(strUIString, Vector2(5, 5), "", Color::GREEN);
		m_uploadedTextureSizeInBytesMsg = StringUtil::Format("texture size: %.3f mb", ((temp_texture_size / 1024.f) / 1024.f));
		strUIString = m_uploadedTextureSizeInBytesMsg.c_str();
		Renderer::Instance()->renderText(strUIString, Vector2(5, 30), "", Color::GREEN);
		m_uploadedGeometricSizeMsg = StringUtil::Format("vertex size: %.3f mb", ((temp_vertex_size / 1024.f) / 1024.f));
		strUIString = m_uploadedGeometricSizeMsg.c_str();
		Renderer::Instance()->renderText(strUIString, Vector2(5, 55), "", Color::GREEN);
		m_drawCallTimesMsg = StringUtil::Format("drawCall Time: %d", temp_dp);
		strUIString = m_drawCallTimesMsg.c_str();
		Renderer::Instance()->renderText(strUIString, Vector2(5, 75), "", Color::GREEN);
		m_triangleNumMsg = StringUtil::Format("Triangles: %d", temp_trangles);
		strUIString = m_triangleNumMsg.c_str();
		Renderer::Instance()->renderText(strUIString, Vector2(5, 100), "", Color::GREEN);
		m_lockTimesMsg = StringUtil::Format("lock Time: %d", m_lockTimes);
		strUIString = m_lockTimesMsg.c_str();
		Renderer::Instance()->renderText(strUIString, Vector2(5, 125), "", Color::GREEN);
		StreamThread* streamThread = Root::Instance()->getThreadThread();
		if (streamThread->IsRunning())
		{
		int respondCount = streamThread->GetRespondCount();
		int requestCount = streamThread->GetRequestCount();
		m_streamThreadCount = StringUtil::Format("Requests: %d Responds: %d", requestCount, respondCount);
		strUIString = m_streamThreadCount.c_str();
		Renderer::Instance()->renderText(strUIString, Vector2(5, 175), "", Color::GREEN);
		}
		m_mathOpCount = StringUtil::Format("V2: %d, V3: %d, V4: %d, Qun: %d, Mat: %d",
		Vector2::OP_COUNT, Vector3::OP_COUNT, Vector4::OP_COUNT, Quaternion::OP_COUNT, Matrix4::OP_COUNT);
		strUIString = m_mathOpCount.c_str();
		Renderer::Instance()->renderText(strUIString, Vector2(5, 150), "", Color::GREEN);*/
		Vector2::OP_COUNT = 0;
		Vector3::OP_COUNT = 0;
		Vector4::OP_COUNT = 0;
		Quaternion::OP_COUNT = 0;
		Matrix4::OP_COUNT = 0;
#endif
	}


	//---------------------------------------------------------------------
	/*Root* Root::Instance()
	{
	return ms_pSingleton;
	}*/

	Texture* Root::s_BlankTexture = NULL;
	float Root::g_fViewportRadio = 9.0f / 16.0f;
	const ui32 perframetime = 1000 / 60;
	Root::Root()
		: m_pAssetMgr(NULL)
		, m_bRendererInited(false)
		, m_bSupportGPUSkin(true)
		, m_lastTime(0)
		, m_frameTime(0)
		, m_currentTime(0)
		, m_timeCount(0)
		, m_frameCount(0)
		, m_curFameCount(0)
		, m_fps(0)
		, m_maxFrameTime(0)
		, m_isEditorMode(false)
		, m_FileSystemArchiveFactory(NULL)
		, m_ZipPackArchiveFactory(NULL)
		, m_ApkFileSystemArchiveFactory(NULL)
		, m_ArchiveManager(NULL)
		, m_ResourceGroupManager(NULL)
		, m_Timer(NULL)
		, m_EffectSystemManager(NULL)
		, m_TessManager(NULL)
		, m_StreamThreading(NULL)
		, m_CalcThreading(NULL)
		, m_startMipmap(0)
		, m_enableFilterAdditional(false)
		, m_bFixedAspect(true)
		, m_fAspectRadio(9.0f / 16.0f)
		, m_realWidth(1024)
		, m_realHeight(768)
	{
	}

	Root::~Root()
	{
	}

	bool Root::initialize_mini()
	{
		Math::InitMathLib();
		setlocale(LC_ALL, "zh_CN.UTF-8");
		try
		{
			Log::init("LordLog.cfg");

			LordLogDebug("Loading charset encoding...");
			_EncoderInit();
			LordLogDebug("Charset encoding loaded success.");

			m_Timer = LordNew Time;

			m_StreamThreading = LordNew StreamThread;

			m_MainThreadEvent.Create();
		}
		catch (const Exception &e)
		{
			LordLogError("%s", e.getMessage().c_str());
			return false;
		}
		return true;
	}

	bool Root::initialize(const RootCfg& cfg)
	{
		// check root path
		m_rootPath = cfg.rootPath;
		if (!PathUtil::IsEndWithSeperator(m_rootPath))
			m_rootPath += PathUtil::SEPERATOR;

		Math::InitMathLib();

		m_pAssetMgr = cfg.pAssetMgr;
		m_isEditorMode = cfg.bEditorMode;

		m_bFixedAspect = cfg.bFixedAspect;
		m_fAspectRadio = cfg.fAspectRadio;

		// check root path
		setlocale(LC_ALL, "zh_CN.UTF-8");

		try
		{
			Log::init("LordLog.cfg");

			LordLogDebug("Loading charset encoding...");
			_EncoderInit();
			LordLogDebug("Charset encoding loaded success.");

			if (!PathUtil::IsDirExist(m_rootPath))
			{
				LordLogError("Set root path failed [%s], initialise Lord Engine failed.", m_rootPath.c_str());
				return false;
			}

			LordNew ImageCodecMgr;
			LordNew MeshManager;
			LordNew AnimManager;
			LordNew SkeletonManager;
			LordNew SceneManager;
			LordNew SoundSystem;
			LordNew ActorManager;
			LordNew LightmapMgr(512, 512);

			m_ArchiveManager = LordNew ArchiveManager;
			m_FileSystemArchiveFactory = LordNew FileSystemArchiveFactory;
			m_ZipPackArchiveFactory = LordNew ZipArchiveFactory;
			m_ArchiveManager->addArchiveFactory(m_FileSystemArchiveFactory);
			m_ArchiveManager->addArchiveFactory(m_ZipPackArchiveFactory);

			// Õ‚≤ø¥Êµµ
			for (size_t i = 0; i<cfg.externalArchiveFactorys.size(); i++)
				m_ArchiveManager->addArchiveFactory(cfg.externalArchiveFactorys[i]);

#if (LORD_PLATFORM == LORD_PLATFORM_ANDROID)
			m_ApkFileSystemArchiveFactory = LordNew APKFileSystemArchiveFactory((AAssetManager*)cfg.pAssetMgr);
			m_ArchiveManager->addArchiveFactory(m_ApkFileSystemArchiveFactory);
#endif
			m_ResourceGroupManager = LordNew ResourceGroupManager;
			//m_ResourceGroupManager->setRootPath(cfg.rootPath);
			//m_ResourceGroupManager->setRootPath();
			m_Timer = LordNew Time;
			m_EffectSystemManager = LordNew EffectSystemManager;
			m_TessManager = LordNew TessManager;

#ifdef LORD_INCLUDE_GUI
			LordNew GUISystem;
#endif

			LordNew TouchManager;
			LordNew KeyEventManager;

			m_StreamThreading = LordNew StreamThread;

			m_MainThreadEvent.Create();
			m_CalcThreading = LordNew CalcThread(&m_MainThreadEvent);
		}
		catch (const Exception &e)
		{
			LordLogError("%s", e.getMessage().c_str());
			return false;
		}

		// setup ConfigFile

		/*#if (LORD_PLATFORM == LORD_PLATFORM_ANDROID)

		AAssetManager* pAssetMgr = (AAssetManager*)cfg.pAssetMgr;
		if(!pAssetMgr)
		{
		LordLogError("Android AssetManager isn't set, initialise Lord Engine failed.");
		return false;
		}

		ConfigFile cfgFile;

		DataStream* pDataStream;
		AAsset* pAsset = AAssetManager_open(pAssetMgr, cfg.resCfgFile.c_str(), AASSET_MODE_BUFFER);
		if(!pAsset)
		{
		LordLogError("Not found resource config fail [%s], initialise Lord Engine failed.", cfg.resCfgFile.c_str());
		return false;
		}

		off_t length = AAsset_getLength(pAsset);
		void* membuf = LordMalloc(length);
		memcpy(membuf, AAsset_getBuffer(pAsset), length);
		AAsset_close(pAsset);

		pDataStream = (DataStream*)(LordNew MemoryDataStream(membuf, length, true, true));

		cfgFile.load(pDataStream);
		setupResource(cfgFile);

		LordDelete pDataStream;
		#else*/

		if (!PathUtil::IsFileExist(cfg.resCfgFile))
		{
			LordLogError("Not found resource config fail [%s], initialise Lord Engine failed.", cfg.resCfgFile.c_str());
			return false;
		}

		ConfigFile cfgFile;
		cfgFile.load(cfg.resCfgFile);
		setupResource(cfgFile);

		configEngine(cfg.engineCfgFile);
		//#endif
		m_deviceFeature.initialize();
		setEnableFrameProfile(true);

		LordLogInfo("Lord Engine initialised success.");

		return true;
	}

	bool Root::initRenderer(Renderer* pRenderer, const Renderer::RenderCfg& config)
	{
		LordAssert(pRenderer);
		if (!pRenderer->initialize(config))
			return false;

		if (!onRendererInited())
			return false;

		LordLogInfo("Init Renderer success.");

#ifdef LORD_USE_RENDERTARGET
		LordNew RenderStageManager;

#if( LORD_PLATFORM == LORD_PLATFORM_ANDROID || LORD_PLATFORM == LORD_PLATFORM_WINDOWS )

		if (!RenderStageManager::Instance()->initialize())
		{
			LordLogInfo("Initialize RenderStageManager Failed !");

			return false;
		}
#endif
		LordLogInfo("Initialize RenderStageManager Success !");
#endif

		s_BlankTexture = TextureManager::Instance()->createTexture("blank.png");
		s_BlankTexture->load();
		return true;
	}


	bool Root::onRendererInited()
	{
		if (m_bRendererInited)
			return true;

		if (!SceneManager::Instance()->initialize())
			return false;

		ActorManager::Instance()->createMaterials();

#ifdef LORD_INCLUDE_GUI
		// setup viewport
		Viewport* pViewport = Renderer::Instance()->getMainViewport(); //getFrameBuffer()->getViewport();

		Camera* p2DCamera = SceneManager::Instance()->get2DCamera();
		pViewport->setViewProjMatrix(p2DCamera->getViewProjMatrix());
		uint width = pViewport->getWidth();
		uint height = pViewport->getHeight();

		GUISystem::Instance()->Initialize(width, height);
#endif

		m_EffectSystemManager->createRenderableSet();
		m_TessManager->createRenderableSet();
		// add the renderable

		m_bRendererInited = true;

		return true;
	}

	bool Root::onSize(uint width, uint height)
	{
		if (m_bRendererInited)
		{
			m_realWidth = width;
			m_realHeight = height;

			Viewport* pViewport = Renderer::Instance()->getMainViewport(); //getFrameBuffer()->getViewport();
			LordAssert(pViewport);

			if (m_bFixedAspect)
			{
				//uint newheight = uint((float)width * m_fAspectRadio);
				uint newheight;
				fixedAspect(width, newheight);

				pViewport->resize(0, Math::Max((int)height - (int)newheight, 0) / 2, width, newheight);
				height = newheight;
			}
			else
			{
				pViewport->resize(0, 0, width, height);
			}

			Renderer::Instance()->setViewport(pViewport);

			Camera* pMainCamera = SceneManager::Instance()->getMainCamera();
			pMainCamera->setWidth(width);
			pMainCamera->setHeight(height);
			pMainCamera->update();

			Camera* p2DCamera = SceneManager::Instance()->get2DCamera();
			p2DCamera->setWidth(width);
			p2DCamera->setHeight(height);
			p2DCamera->update();

			pViewport->setViewProjMatrix(p2DCamera->getViewProjMatrix());

#ifdef LORD_INCLUDE_GUI
			GUISystem::Instance()->OnSized(width, height);
#endif
		}
#ifdef LORD_USE_RENDERTARGET
		if (RenderTargetManager::Instance())
		{
			RenderTargetManager::Instance()->onResize(m_realWidth, m_realHeight);
		}
#endif


		return true;
	}

	void Root::destroy()
	{
		if (TouchManager::Instance())
			LordDelete TouchManager::Instance();
		if (KeyEventManager::Instance())
			LordDelete KeyEventManager::Instance();

		if (SceneManager::Instance())
			SceneManager::Instance()->closeScene();

		if (LightmapMgr::Instance())
			LordDelete LightmapMgr::Instance();

#ifdef LORD_INCLUDE_GUI
		if (GUISystem::Instance())
		{
			GUISystem::Instance()->Destroy();
			LordDelete GUISystem::Instance();
		}
#endif
		if (ActorManager::Instance())
			LordDelete ActorManager::Instance();

		if (m_EffectSystemManager)
		{
			m_EffectSystemManager->destroyRenderableSet();
			m_EffectSystemManager->destroyAllEffectSystemTemplates();
			m_EffectSystemManager->destroyAllEffectSystems();
			LordSafeDelete(m_EffectSystemManager);
		}

		if (m_TessManager)
		{
			m_TessManager->destroyRenderableSet();
			LordSafeDelete(m_TessManager);
		}

		if (SceneManager::Instance())
			SceneManager::Instance()->destroy();

		if (SoundSystem::Instance())
			LordDelete SoundSystem::Instance();
		if (SceneManager::Instance())
			LordDelete SceneManager::Instance();
		if (SkeletonManager::Instance())
			LordDelete SkeletonManager::Instance();
		if (AnimManager::Instance())
			LordDelete AnimManager::Instance();
		if (MeshManager::Instance())
			LordDelete MeshManager::Instance();
		if (ImageCodecMgr::Instance())
			LordDelete ImageCodecMgr::Instance();

		if (QueryObjectManager::Instance())
		{
			QueryObjectManager::Instance()->destroy();
			LordDelete QueryObjectManager::Instance();
		}

		if (m_StreamThreading)
		{
			m_StreamThreading->Shutdown();
			LordSafeDelete(m_StreamThreading);
		}

		m_MainThreadEvent.Destroy();
		if (m_CalcThreading)
		{
			m_CalcThreading->Shutdown();
			LordSafeDelete(m_CalcThreading);
		}

		if (s_BlankTexture)
		{
			TextureManager::Instance()->releaseResource(s_BlankTexture);
			s_BlankTexture = NULL;
		}
		LordSafeDelete(m_ArchiveManager);
		LordSafeDelete(m_ZipPackArchiveFactory);
		LordSafeDelete(m_FileSystemArchiveFactory);
		LordSafeDelete(m_ApkFileSystemArchiveFactory);
		LordSafeDelete(m_ResourceGroupManager);
		LordSafeDelete(m_Timer);

#ifdef LORD_USE_RENDERTARGET
		LordDelete RenderStageManager::Instance();
#endif

		LordLogInfo("Lord Engine has been shutdown.");

		if (Renderer::Instance())
		{
			Renderer::Instance()->destroy();
			LordDelete Renderer::Instance();
			LordLogInfo("Lord Renderer has been shutdown.");
		}

		Math::ReleaseMathLib();
	}

	const String& Root::getWriteablePath() const
	{
		//#if( LORD_PLATFORM == LORD_PLATFORM_ANDROID || LORD_PLATFORM == LORD_PLATFORM_WINDOWS )
		//return m_rootPath;
		//#else
		return m_strWriteablePath;
		//#endif
	}

	/*ui32 Root::getFrameTime() const
	{
	return m_frameTime;
	}*/

	void Root::enableStreamThread(bool enable)
	{
		if (m_StreamThreading)
		{
			if (enable)
				m_StreamThreading->Startup();
			else
				m_StreamThreading->Shutdown();
		}
	}

	void Root::enableCalcThread(bool enable)
	{
		if (m_CalcThreading)
		{
			if (enable)
			{
				m_CalcThreading->Startup();
			}
			else
			{
				m_CalcThreading->Shutdown();
			}
		}
	}

	void Root::calcFrameTime()
	{
		m_currentTime = m_Timer->getMilliseconds();
		m_frameTime = Math::Min(m_currentTime - m_lastTime, (ui32)20);
		m_lastTime = m_currentTime;
	}

	void Root::frameMove(ui32 frameTime)
	{
		m_frameTime = frameTime;
		if (m_maxFrameTime < m_frameTime)
			m_maxFrameTime = m_frameTime;

		++m_curFameCount;
		m_timeCount += ui32((float)m_frameTime / ActorManager::Instance()->GetSpeedUp() + Math::MIN_REAL);
		m_fps = (ui32)(1000.0f * (float)++m_frameCount / ((float)m_timeCount + Math::MIN_REAL));
		if (m_timeCount >= 5000)
		{
			m_timeCount -= 5000;
			m_frameCount = 0;
			m_maxFrameTime = 0;
		}

		//calcFrameTime();

#ifdef LORD_USE_RENDERTARGET
#if( LORD_PLATFORM == LORD_PLATFORM_MAC_IOS)
		static bool init_r = false;
		if (!init_r)
		{
			if (LORD::RenderTargetManager::Instance())
			{
				if (!LORD::RenderTargetManager::Instance()->initialize())
				{
					LordLogError("RenderTargetManager::initialize failed !");
					return;
				}
			}

			if (LORD::RenderStageManager::Instance())
			{
				if (!LORD::RenderStageManager::Instance()->initialize())
				{
					LordLogError("RenderStageManager::initialize failed !");
					return;
				}
			}

			init_r = true;

		}
#endif
#endif


		if (m_CalcThreading->IsRunning())
		{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
			m_MainThreadEvent.Wait();
#else
#ifdef _DEBUG
			if (m_MainThreadEvent.Wait())
			{
				LordLogWarning("CalcThread is busy, the MainThread will be waitting it!");
			}
#else
			m_MainThreadEvent.Wait();
#endif
#endif
			m_CalcThreading->RespondEvents();
		}

		if (m_StreamThreading && m_StreamThreading->IsRunning())
		{
			m_StreamThreading->RespondEvents();
		}

		SceneManager::Instance()->update();

#ifdef LORD_INCLUDE_GUI
		GUISystem::Instance()->UpdateGUI();
#endif
		ActorManager::Instance()->UpdateActors(m_frameTime);

		updateAllManagerDelayResource();
	}

	void Root::changeFilterAdditionalMap(const String& mapName)
	{
		if (m_enableFilterAdditional)
			RenderTargetManager::Instance()->changeFilterBlendmapName(mapName);
	}

	void Root::updateAllManagerDelayResource()
	{
		int ct = m_frameCount % 25;
		ct = ct / 5;
		switch (ct)
		{
		case 0:
			TextureManager::Instance()->updateDelayedRelease(m_frameTime * 5);
			GUIImagesetManager::Instance()->updateDelayedRelease(m_frameTime * 5);
			break;
		case 1:
			MeshManager::Instance()->updateDelayedRelease(m_frameTime * 5);
			break;
		case 2:
			SkeletonManager::Instance()->updateDelayedRelease(m_frameTime * 5);
			break;
		case 3:
			AnimManager::Instance()->updateDelayedRelease(m_frameTime * 5);
			break;
		case 4:
			ActorManager::Instance()->DestroyDelayActors();
			break;
		}
	}

	void Root::setupResource(const ConfigFile& cfgFile)
	{
		size_t num;
		const ConfigFile::Settings* settingSection = cfgFile.getAllSetting(num);

		String secName, typeName, archName;
		for (size_t i = 0; i < num; ++i)
		{
			secName = settingSection[i].name;
			ConfigFile::SettingsMultiMap* settings = settingSection[i].setting;
			for (ConfigFile::SettingsMultiMap::iterator jt = settings->begin();
				jt != settings->end(); ++jt)
			{
				typeName = jt->first;
				archName = jt->second;
				//bool recursive = false;
				if (archName[0] == '*')
				{
					archName.erase(0, 1);
					StringUtil::Trim(archName);
					//recursive = true;
				}

				m_ResourceGroupManager->addResourceLocation(archName, typeName,
					secName);
			}
		}
	}

	void Root::configEngine(const String& fileName)
	{
		EngineConfig engineConfig;

		LORD::String lstrFile = LORD::Root::Instance()->getRootPath() + fileName;
		FILE* lpFile = fopen(lstrFile.c_str(), "rb");
		if (NULL == lpFile)
		{
			LordLogInfo("Not found engine config file [%s], use default engine config.", fileName.c_str());
		}
		else
		{
			fseek(lpFile, 0, SEEK_END);
			long liFileSize = ftell(lpFile);
			fseek(lpFile, 0, SEEK_SET);

			char* pJSONData = (char*)LordMalloc(liFileSize + 1);
			fread(pJSONData, sizeof(char), liFileSize, lpFile);
			pJSONData[liFileSize] = 0;
			fclose(lpFile);

			rapidjson::Document* doc = new rapidjson::Document;
			doc->Parse(pJSONData);

			if (doc->HasParseError())
			{
				LordLogInfo("invalid engine config file [%s].", fileName.c_str());
			}
			else
			{
				rapidjson::Value::MemberIterator itEngineConfig = doc->MemberBegin();
				rapidjson::Value::MemberIterator it = itEngineConfig->value.MemberBegin();

				for (; it != itEngineConfig->value.MemberEnd(); ++it)
				{
					String strObjectName = it->name.GetString();


					if (strObjectName == "EnableHighEffectActor")
					{
						engineConfig.m_bEnableHighEffectActor = it->value.GetBool();
					}
					else if (strObjectName == "EnableFilterAdditional")
					{
						engineConfig.m_bEnableFilterAdditional = it->value.GetBool();
					}
					else if (strObjectName == "EnableDistortion")
					{
						engineConfig.m_bEnableDistortion = it->value.GetBool();
					}
					else if (strObjectName == "EnableCalcThread")
					{
						engineConfig.m_bEnableCalcThread = it->value.GetBool();
					}
					else if (strObjectName == "EnableStreamThread")
					{
						engineConfig.m_bEnableStreamThread = it->value.GetBool();
					}
				}

			}

			delete doc;
			LordFree(pJSONData);
		}
#if 0
		LORD::String lstrFile = LORD::Root::Instance()->getRootPath() + fileName;
		FILE* lpFile = fopen(lstrFile.c_str(), "rb");
		if (NULL == lpFile)
		{
			LordLogInfo("Not found engine config file [%s], use default engine config.", fileName.c_str());
		}
		else
		{
			fseek(lpFile, 0, SEEK_END);
			long liFileSize = ftell(lpFile);
			fseek(lpFile, 0, SEEK_SET);

			char* xmlData = (char*)LordMalloc(liFileSize + 1);
			fread(xmlData, sizeof(char), liFileSize, lpFile);
			xmlData[liFileSize] = 0;
			fclose(lpFile);

			xml_document<>* doc = LordNewT(xml_document<>);        // character type defaults to cha
			doc->parse<0>(xmlData);

			xml_node<>* pEngineConfigRoot = doc->first_node();
			if (!pEngineConfigRoot)
			{
				LordLogInfo("invalid engine config file [%s].", fileName.c_str());
			}
			else
			{
				xml_node<>* pValueNode = pEngineConfigRoot->first_node();
				while (pValueNode)
				{
					String strNodeName = pValueNode->name();
					xml_attribute<>* pValue = pValueNode->first_attribute();
					if (pValue)
					{
						String strValue = pValue->value();
						if (strNodeName == "EnableHighEffectActor")
						{
							engineConfig.m_bEnableHighEffectActor = StringUtil::ParseBool(strValue);
						}
						else if (strNodeName == "EnableFilterAdditional")
						{
							engineConfig.m_bEnableFilterAdditional = StringUtil::ParseBool(strValue);
						}
						else if (strNodeName == "EnableDistortion")
						{
							engineConfig.m_bEnableDistortion = StringUtil::ParseBool(strValue);
						}
						else if (strNodeName == "EnableCalcThread")
						{
							engineConfig.m_bEnableCalcThread = StringUtil::ParseBool(strValue);
						}
						else if (strNodeName == "EnableStreamThread")
						{
							engineConfig.m_bEnableStreamThread = StringUtil::ParseBool(strValue);
						}
					}
					pValueNode = pValueNode->next_sibling();
				}
			}

			LordDeleteT(doc, xml_document<>);
			LordFree(xmlData);
		}
#endif

		//config all
		SceneManager::Instance()->setRenderActorHigh(engineConfig.m_bEnableHighEffectActor);
		setEnableFilterAdditional(engineConfig.m_bEnableFilterAdditional);
		EffectSystemManager::Instance()->setEnableDistortionRender(engineConfig.m_bEnableDistortion);
		enableStreamThread(engineConfig.m_bEnableStreamThread);
		enableCalcThread(engineConfig.m_bEnableCalcThread);
	}

	void Root::TriggerCalcThread()
	{
		if (m_CalcThreading->IsRunning())
		{
			m_CalcThreading->GetThreadEvent()->Trigger();
		}
	}


	void Root::enableRenderQueue(ui32 renderQueueID)
	{
		ui32 nCount = SceneManager::Instance()->getRenderQueueCount();
		if (renderQueueID >= nCount) return;

		if (renderQueueID == SceneManager::Instance()->getRenderQueueIndex("Effect"))
		{
			if (EffectSystemManager::Instance())
				EffectSystemManager::Instance()->setEnableRender(true);
			return;
		}

		if (renderQueueID == SceneManager::Instance()->getRenderQueueIndex("UI"))
		{
#ifdef LORD_INCLUDE_GUI
			if (GUISystem::Instance())
				GUISystem::Instance()->setEnableRender(true);
			return;
#endif
		}

		SceneManager* pSm = SceneManager::Instance();
		if (pSm)
		{
			RenderQueue*   pRenderQueue = pSm->getRenderQueueByIndex(renderQueueID);

			if (pRenderQueue)
			{
				pRenderQueue->enableRender();
			}
		}
	}

	void Root::disableRenderQueue(ui32 renderQueueID)
	{
		ui8 nCount = SceneManager::Instance()->getRenderQueueCount();
		if (renderQueueID >= nCount) return;

		if (renderQueueID == SceneManager::Instance()->getRenderQueueIndex("Effect"))
		{
			if (EffectSystemManager::Instance())
				EffectSystemManager::Instance()->setEnableRender(false);
			return;
		}

		if (renderQueueID == SceneManager::Instance()->getRenderQueueIndex("UI"))
		{
#ifdef LORD_INCLUDE_GUI
			if (GUISystem::Instance())
				GUISystem::Instance()->setEnableRender(false);
			return;
#endif
		}

		SceneManager* pSm = SceneManager::Instance();
		if (pSm)
		{
			RenderQueue*   pRenderQueue = pSm->getRenderQueueByIndex(renderQueueID);

			if (pRenderQueue)
			{
				pRenderQueue->disableRender();
			}
		}
	}

	void Root::reset()
	{
		SceneManager::Instance()->closeScene();
		GUIWindowManager::Instance()->DestroyAllGUIWindows();
		ActorManager::Instance()->FreeActors();
		ActorManager::Instance()->SetSpeedUp();
		EffectSystemManager::Instance()->destroyAllEffectSystems();

		blurDefaultBuffer(false);
	}

	Texture* Root::blurDefaultBuffer(bool blur)
	{
#ifdef LORD_USE_RENDERTARGET
		return RenderStageManager::Instance()->blurLDRSceneTarget(blur);
#else
		return NULL;
#endif
	}

}