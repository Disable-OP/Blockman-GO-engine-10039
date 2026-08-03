/********************************************************************
filename: 	Root.h
file path:	dev\engine\Src\Core\Object\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_ROOT_H__
#define __LORD_ROOT_H__

#include "Core.h"
#include "Render/Renderer.h"
#include "Resource/LordThread.h"

namespace LORD
{
	class ArchiveManager;
	class ArchiveFactory;
	class ResourceGroupManager;
	class TessManager;

	class LORD_CORE_API DeviceFeature
	{
	public:
		DeviceFeature();
		~DeviceFeature();

		const static String cs_pvr_format;
		const static String cs_etc1_format;
		const static String cs_atitc_format;
		const static String cs_atitc_format2;
		const static String cs_dxt1_format;
		const static String cs_s3tc_format;
		const static String cs_s3tc_format2;
		const static String cs_half_float_texture;
		const static String cs_half_float_texture_linear;
		void initialize();

		void checkTextureFormat(const String& features);

		bool supportPVR() const;
		bool supportDXT1() const;
		bool supportATITC() const;
		bool supportETC1() const;
		bool supportHFTexture() const;
		bool supportHFTextureLinear() const;
		String& rendererName() { return m_rendererName; }
		const String& rendererName() const { return m_rendererName; }

		String& glesVersion() { return m_glesVersion; }
		const String& glesVersion() const { return m_glesVersion; }

		String& vendor() { return m_vendor; }
		const String& vendor() const { return m_vendor; }

		String& shadingLangVersion() { return m_shadingLanVersion; }
		const String& shadingLangVersion() const { return m_shadingLanVersion; }

		String& phoneType() { return m_phoneType; }
		const String& phoneType() const { return m_phoneType; }
		//void setSeperateSampleAlpha( bool _val ) { m_seperateSampleAlpha = _val; }
		//bool getSeperateSampleAlpha() const { return m_seperateSampleAlpha; }

	protected:
		typedef std::map<String, bool> FeaturesMap;
		FeaturesMap m_features;
		String		m_rendererName;
		String		m_glesVersion;
		String		m_vendor;
		String		m_shadingLanVersion;
		String		m_phoneType;
		//bool		m_seperateSampleAlpha;
		bool		m_supportDXT1;
		bool		m_supportPVR;
		bool		m_supportATITC;
		bool		m_supportETC1;

		bool		m_supportHalfFloatTexture;
		bool		m_supportHalfFloatTextureLinear;
	};

	class FrameState
	{
	public:
		FrameState()
			: m_uploadedTextureSizeInBytes(0)
			, m_uploadedGeometricSize(0)
			, m_drawCallTimes(0)
			, m_triangleNum(0)
			, m_lockTimes(0)
			, m_fps(0)
		{}

		~FrameState() {}

		inline void reset()
		{
			m_triangleNum = 0;
			m_drawCallTimes = 0;
			m_lockTimes = 0;
			m_fps = 0;
			m_maxFrameTimePerSecond = 0;
		}

		inline void incrUploadTextureSizeInBytes(ui32 _sizeInBytes) { m_uploadedTextureSizeInBytes += _sizeInBytes; }
		inline void decrUploadTextureSizeInBytes(ui32 _sizeInBytes) { m_uploadedTextureSizeInBytes -= _sizeInBytes; }
		inline void incrUploadGeometricSize(ui32 _sizeInBytes) { m_uploadedGeometricSize += _sizeInBytes; }
		inline void decrUploadGeometricSize(ui32 _sizeInBytes) { m_uploadedGeometricSize -= _sizeInBytes; }
		inline void incrDrawCallTimes(ui32 _times) { m_drawCallTimes += _times; }
		inline void incrLockTimes(ui32 _time) { m_lockTimes += _time; }
		inline void incrTriangleNum(ui32 _triangles) { m_triangleNum += _triangles; }
		void output();

		inline ui32 getFps() const { return m_fps; }
		inline ui32 getDrawCalls() const { return m_drawCallTimes; }
		inline ui32 getTextureSizeInBytes() const { return m_uploadedTextureSizeInBytes; }
		inline ui32 getVertexSize() const { return m_uploadedGeometricSize; }
		inline ui32 getTriangleNum() const { return m_triangleNum; }
	protected:
		ui32 m_uploadedTextureSizeInBytes;
		ui32 m_uploadedGeometricSize;
		ui32 m_drawCallTimes;
		ui32 m_lockTimes;
		ui32 m_fps;
		ui32 m_maxFrameTimePerSecond;
		ui32 m_minTimeFrame;
		ui32 m_maxTimeFrame;
		ui32 m_triangleNum;
		String m_fpsMsg;
		String m_maxFrameTimeMsg;
		String m_lockTimesMsg;
		String m_drawCallTimesMsg;
		String m_triangleNumMsg;
		String m_uploadedTextureSizeInBytesMsg;
		String m_uploadedGeometricSizeMsg;
		String m_streamThreadCount;
		String m_mathOpCount;
	};

	class LORD_CORE_API Root : public ObjectAlloc, public Singleton<Root>
	{
	public:
		inline static Root* Instance()
		{
			return ms_pSingleton;
		}

	public:
		Root();
		~Root();

		struct RootCfg
		{
			String		rootPath;
			String		resCfgFile;
			String		engineCfgFile;
			bool		bEditorMode;
			bool		bFixedAspect;
			float		fAspectRadio;
			void*		pAssetMgr;					// for Android platfrom
			vector<ArchiveFactory*>::type	externalArchiveFactorys;	// external archive factory

			RootCfg()
				: rootPath("./")
				, resCfgFile("resource.cfg")
				, engineCfgFile("engine.json")
				, bEditorMode(false)
				, bFixedAspect(false)
				, fAspectRadio(9.0f / 16.0f)
				, pAssetMgr(NULL)
			{}
		};

		struct EngineConfig
		{
			bool	m_bEnableHighEffectActor;
			bool	m_bEnableFilterAdditional;
			bool	m_bEnableDistortion;
			bool	m_bEnableCalcThread;
			bool	m_bEnableStreamThread;

			EngineConfig()
				: m_bEnableHighEffectActor(true)
				, m_bEnableFilterAdditional(false)
				, m_bEnableDistortion(false)
				, m_bEnableCalcThread(false)
				, m_bEnableStreamThread(true)
			{}
		};

	public:
		bool			initialize(const RootCfg& cfg);
		bool			initialize_mini(); // for server
		bool			initRenderer(Renderer* pRenderer, const Renderer::RenderCfg& config);
		bool			onRendererInited();
		bool			onSize(uint width, uint height);
		void            fixedAspect(uint width, uint& height) { height = uint((float)width * m_fAspectRadio); }
		void			destroy();

		// 释放场景资源，ui root window
		void			reset();
		void            serverTick() { m_serverFrameCount++; }
		ui32            getServerFrameCount() { return m_serverFrameCount; }

		const String&	getRootPath() const { return m_rootPath; }
		const String&   getWriteablePath() const;
		String			getMapPath() { return m_mapPath; }
		void			setMapPath(String mapPath) { this->m_mapPath = mapPath; }
		String			getWorldName() { return m_worldName; }
		void			setWorldName(String worldName) { this->m_worldName = worldName; }
		inline void     setWriteablePath(const String& strPath) { m_strWriteablePath = strPath; }
		inline void*	getAssetManager() const { return m_pAssetMgr; }
		inline bool		isRendererInited() const { return m_bRendererInited; }
		inline ui32		getCurrentTime() const { return m_Timer ? m_Timer->getMilliseconds() : 0; }
		inline ui32		getFrameTime() const { return m_frameTime; }
		inline ui32		getFrameCount() const { return m_frameCount; }
		inline uint		getFPS() const { return m_fps; }
		inline uint		getMaxFrameTime() const { return m_maxFrameTime; }

		void			enableStreamThread(bool enable);
		void			enableCalcThread(bool enable);
		CalcThread*		getCalcThread() const { return m_CalcThreading; }
		StreamThread*	getThreadThread() const { return m_StreamThreading; }

		void			frameMove(ui32 frameTime);
		void			calcFrameTime();

		// modify by ajohn. this function can't be exist.
		// void			setEditorMode(bool setting) { m_isEditorMode = setting; }
		inline bool		isEditorMode() { return m_isEditorMode; }
		inline bool		isSupportGPUSkin() { return m_bSupportGPUSkin; }
		inline void		setSupportGPUSkin(bool support) { m_bSupportGPUSkin = support; }

		inline ui32&			startMipmap() { return m_startMipmap; }
		inline const ui32&		startMipmap() const { return m_startMipmap; }

		inline DeviceFeature&	deviceFeatures() { return m_deviceFeature; }

		inline void		setEnableFrameProfile(bool _enable) { m_enableFrameProfile = _enable; }

		inline bool		getEnableFrameProfile() const { return m_enableFrameProfile; }

		inline FrameState&		frameState() { return m_frameState; }
		inline const FrameState& frameState() const { return m_frameState; }

		void			outputFrameState() { m_frameState.output(); }
		void			resetFrameState() { m_frameState.reset(); }
		void 			SetPhoneinformation(int max, int free, String cpu)
		{
			Maxmemory = max;
			Freememory = free;
			cputex = cpu;
		}

		void		TriggerCalcThread();

		ui32		getCurFrameCount() const { return m_curFameCount; }
		void		disableRenderQueue(ui32 renderQueueID);
		void		enableRenderQueue(ui32 renderQueueID);

		void		setEnableFilterAdditional(bool _val) { m_enableFilterAdditional = _val; }
		bool		getEnableFilterAdditional() const { return m_enableFilterAdditional; }

		void		changeFilterAdditionalMap(const String& mapName);

		bool		isFixedAspect() { return m_bFixedAspect; }

		float		getAspectRadio() { return m_fAspectRadio; }

		uint		getRealWidth() { return m_realWidth; }
		uint		getRealHeight() { return m_realHeight; }

		Texture*	blurDefaultBuffer(bool blur);

		static Texture*	getBlankTexture() { return s_BlankTexture; }
	protected:
		void		updateAllManagerDelayResource();
		void		setupResource(const ConfigFile& cfgFile);
		void		configEngine(const String& fileName);

	private:
		static Texture* s_BlankTexture;
		static float	g_fViewportRadio;
		String			m_rootPath;
		String			m_mapPath;
		String			m_worldName;
		String          m_strWriteablePath;
		void*			m_pAssetMgr;		// for android
		bool			m_bRendererInited;

		bool			m_bSupportGPUSkin;

		ui32			m_lastTime;
		ui32			m_frameTime;
		ui32			m_currentTime;
		ui32			m_timeCount;
		ui32			m_frameCount;
		ui32			m_curFameCount;
		uint			m_fps;
		uint			m_maxFrameTime;
		bool			m_isEditorMode;

		ui32            m_serverFrameCount = 0;

		ArchiveFactory*			m_FileSystemArchiveFactory;
		ArchiveFactory*			m_ZipPackArchiveFactory;
		ArchiveFactory*			m_ApkFileSystemArchiveFactory;

		ArchiveManager*			m_ArchiveManager;
		ResourceGroupManager*	m_ResourceGroupManager;
		Time*					m_Timer;
		EffectSystemManager*	m_EffectSystemManager;
		TessManager*			m_TessManager;
		StreamThread*			m_StreamThreading;
		CalcThread*				m_CalcThreading;
		Event					m_MainThreadEvent;

		ui32					m_startMipmap;
		DeviceFeature			m_deviceFeature;
		bool					m_enableFrameProfile;
		FrameState				m_frameState;
		int Maxmemory;
		int Freememory;
		String cputex;
		bool m_enableFilterAdditional;

		// 是否固定分辨率
		bool			m_bFixedAspect;
		float			m_fAspectRadio;
		uint			m_realWidth;
		uint			m_realHeight;
	};
}

#endif