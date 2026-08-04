#include "Core.h"
#include "EffectSystemManager.h"
#include "EffectParticleControllerSet.h"
#include "Geom/SimpleSpline.h"

namespace LORD
{
	EffectParticleControllerSet::EffectParticleControllerSet()
	{

	}

	EffectParticleControllerSet::~EffectParticleControllerSet()
	{
		destroyAllControllers();
	}

	void EffectParticleControllerSet::_notifyStart()
	{
		for (ui32 i = 0; i<(ui32)mControllers.size(); ++i)
		{
			mControllers[i]->_notifyStart(NULL);
		}
	}

	void EffectParticleControllerSet::_updateParticle(EffectParticle* particle)
	{
		for (ui32 i = 0; i<(ui32)mControllers.size(); ++i)
		{
			mControllers[i]->controll(particle->mTime, particle, true);
		}
	}

	void EffectParticleControllerSet::copyAttributesTo(EffectParticleControllerSet* ParticleControllerSet)
	{
		for (ui32 i = 0; i<(ui32)mControllers.size(); ++i)
		{
			EffectController* cloneController = EffectSystemManager::Instance()->cloneController(getController(i));
			ParticleControllerSet->addController(cloneController);
		}

		if(mSpline != NULL)
		{
			ParticleControllerSet->mSpline = LordNew SimpleSpline;
			for(ui16 i=0; i<mSpline->getNumPoints(); ++i)
			{
				ParticleControllerSet->mSpline->addPoint(mSpline->getPoint(i));
			}
		}
	}

	void EffectParticleControllerSet::getPropertyList(PropertyList& list)
	{

	}

	bool EffectParticleControllerSet::getPropertyType(const String& name, PropertyType& type)
	{
		return false;
	}

	bool EffectParticleControllerSet::getPropertyValue(const String& name, String& value)
	{
		return false;
	}

	bool EffectParticleControllerSet::setPropertyValue(const String& name,const String& value)
	{
		return false;
	}
}