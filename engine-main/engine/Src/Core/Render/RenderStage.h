/********************************************************************
filename: 	RenderStage.h
file path:	dev\engine\Src\Core\Render\

version:	1
author:		ajohn
company:	supernano
*********************************************************************/

#ifndef __LORD_RENDERSTAGE_H
#define __LORD_RENDERSTAGE_H

#include "Core.h"
#include "RenderTarget.h"

namespace LORD
{
	enum RenderStageID
	{
		RSI_EditorBackground,
		RSI_LDRSceneColor,
		RSI_PostProcess,
		RSI_UI,
		RSI_EditorFrontground,
		RSI_End
	};


	class LORD_CORE_API RenderStage : public ObjectAlloc
	{

		friend class RenderStageManager;

	public:

		void setEnable( bool _enable )
		{
			m_bEnable = _enable;
		}

		bool getEnable() const
		{
			return m_bEnable;
		}

		const RenderStageID getID() const
		{
			return m_rsID;
		}

		virtual void render() = 0;
		virtual bool initialize() = 0;
		virtual void destroy() = 0;
	protected:
	
		RenderStage( RenderStageID stageName );
		virtual ~RenderStage();

		bool m_bEnable;
		bool m_bInitialized;
		RenderStageID m_rsID;
	};

	class LORD_CORE_API EditorBackgroundRenderStage : public RenderStage
	{
		friend class RenderStageManager;

	public:
		virtual void render();
		virtual bool initialize();
		virtual void destroy();
	protected:
		EditorBackgroundRenderStage();
		virtual ~EditorBackgroundRenderStage();
	};
	
	class LORD_CORE_API LDRSceneColorRenderStage : public RenderStage
	{
		friend class RenderStageManager;
	public:
		virtual bool initialize();
		virtual void render();
		virtual void destroy();

	protected:
		LDRSceneColorRenderStage();
		virtual ~LDRSceneColorRenderStage();
	};

	class LORD_CORE_API PostProcessRenderStage : public RenderStage
	{
		friend class RenderStageManager;
		enum 
		{
			DownSamplePass = 5
		};

	public:
		virtual bool initialize();
		virtual void destroy();
		virtual void render();

		PostProcessRenderStage();
		virtual ~PostProcessRenderStage();
		
		struct LORD_CORE_API GaussianBlurringHelper
		{
			static void setGaussianMean( float _val )
			{
				m_gaussianMean = _val;
			}

			static void setGaussianDeviation( float _val )
			{
				m_gaussianDev = _val;
			}

			static void setGaussianMultiplier( float _val )
			{
				m_gaussianMultiplier = _val;
			}

			/*static float ComputeGaussianValue( float x, float mean, float std_deviation )
			{
			return ( 1.0f / sqrt( 2.0f * Math::PI * std_deviation * std_deviation ) )
			* expf( ( -( ( x - mean ) * ( x - mean ) ) ) / ( 2.0f * std_deviation * std_deviation ) );
			}*/

			static void calculateBlurringFilter( ui32 _width, ui32 _height )
			{
				if( _width == m_width && _height == m_height ) return ;

				/*for( int x = 0, idx = 0; x < 9; x++ )
				{
				float _v = ( static_cast< float >( x ) - 4.0f ) / 4.0f;
				float _weight = m_gaussianMultiplier * ComputeGaussianValue(_v, m_gaussianMean, m_gaussianDev);
				m_horizonFilter[idx] = Vector4(float(x-4)/_width, 0.0f, _weight, 0.0f);

				m_verticalFilter[idx] = Vector4(0.0f, float(x-4)/_height, _weight, 0.0f);

				idx ++;
				}*/

				for (int x = 0, idx = 0; x < 4; x++)
				{
					m_horizonFilter[idx] = Vector4(float(x - 2) / (float)_width, 0.0f, 0.0f, 0.0f);
					m_verticalFilter[idx] = Vector4(0.0f, float(x - 2) / (float)_height, 0.0f, 0.0f);

					idx++;
				}


				m_width = _width, m_height = _height;
			}

			static void enableHorizonFiter()
			{
				memcpy( &m_blurringFilter[0], &m_horizonFilter[0], sizeof(m_blurringFilter) );
			}

			static void enableVerticalFiter()
			{
				memcpy( &m_blurringFilter[0], &m_verticalFilter[0], sizeof(m_blurringFilter) );
			}

			static void horizonBlurring( RenderTargetID _srcID, RenderTargetID _dstID );
			static void verticalBlurring( RenderTargetID _srcID, RenderTargetID _dstID );

			static Vector4 m_horizonFilter[4];
			static Vector4 m_verticalFilter[4];
			static Vector4 m_blurringFilter[4];
			static ui32 m_width;
			static ui32 m_height;

			// gaussian function parameters

			static float m_gaussianMultiplier;
			static float m_gaussianMean;
			static float m_gaussianDev;

			static Material* m_pMtlGaussianBlurring;
			static Renderable* m_pRaGaussianBlurring;

			static bool initialize( PostProcessRenderStage* pprs );
			static void destroy();
			static int m_texStage[2];
		};

	protected:
		int m_texStage[3];
		class GPUBuffer* m_pScreenAlignedQuadVB;
		class GPUBuffer* m_pScreenAlignedQuadIB;
	};

	class LORD_CORE_API UIRenderStage : public RenderStage
	{
		friend class RenderStageManager;

	public:
		virtual bool initialize();
		virtual void render();
		virtual void destroy();
	protected:
		UIRenderStage();
		virtual ~UIRenderStage();

	};

	class LORD_CORE_API RenderStageManager : public ObjectAlloc, public Singleton<RenderStageManager>
	{
	public:
		RenderStageManager();
		~RenderStageManager();

		static RenderStageManager* Instance()
		{
			return ms_pSingleton;
		}

		bool initialize();

		RenderStage* getRenderStageByID( RenderStageID _rsid )
		{
			for( vector<RenderStage*>::type::iterator bit = m_renderStages.begin(); bit != m_renderStages.end(); ++bit )
			{
				if( (*bit)->getID() == _rsid )
				{
					return *bit;
				}
			}

			return NULL;
		}

		void processRenderStages();

		Texture* blurLDRSceneTarget(bool blur);
		bool	isBlurLDRSceneTarget() const { return m_BlurSceneTarget; }

	protected:
		vector<RenderStage*>::type m_renderStages;
		bool	m_BlurSceneTarget;
	};
}

#endif