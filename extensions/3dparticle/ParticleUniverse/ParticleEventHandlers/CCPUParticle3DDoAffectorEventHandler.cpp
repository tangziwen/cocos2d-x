/****************************************************************************
 Copyright (c) 2014 Chukong Technologies Inc.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "3dparticle/ParticleUniverse/ParticleEventHandlers/CCPUParticle3DDoAffectorEventHandler.h"
#include "3dparticle/ParticleUniverse/ParticleAffectors/CCPUParticle3DAffector.h"
#include "3dparticle/ParticleUniverse/CCPUParticleSystem3D.h"

NS_CC_BEGIN
// Constants
const bool PUParticle3DDoAffectorEventHandler::DEFAULT_PRE_POST = false;

//-----------------------------------------------------------------------
PUParticle3DDoAffectorEventHandler::PUParticle3DDoAffectorEventHandler(void) : 
PUParticle3DEventHandler(),
_prePost(DEFAULT_PRE_POST)
{
}
//-----------------------------------------------------------------------
void PUParticle3DDoAffectorEventHandler::handle (PUParticleSystem3D* particleSystem, PUParticle3D* particle, float timeElapsed)
{
///** Search for the affector.
//*/
//ParticleTechnique* technique = 0;
//ParticleAffector* affector = particleTechnique->getAffector(_affectorName);
//if (!affector)
//{
//	// Search all techniques in this ParticleSystem for an affector with the correct name
//	ParticleSystem* system = particleTechnique->getParentSystem();
//	size_t size = system->getNumTechniques();
//	for(size_t i = 0; i < size; ++i)
//	{
//		technique = system->getTechnique(i);
//		affector = technique->getAffector(_affectorName);
//		if (affector)
//		{
//			break;
//		}
//	}
//}

	auto affector = particleSystem->getAffector(_affectorName);
	if (affector)
	{
		// Call the affector even if it has enabled set to 'false'.
		if (_prePost)
		{
			affector->preUpdateAffector(timeElapsed);
			affector->updatePUAffector(particle, timeElapsed);
			affector->postUpdateAffector(timeElapsed);
		}
		else
		{
			affector->updatePUAffector(particle, timeElapsed);
		}
	}
}

PUParticle3DDoAffectorEventHandler* PUParticle3DDoAffectorEventHandler::create()
{
	auto peh = new PUParticle3DDoAffectorEventHandler();
	peh->autorelease();
	return peh;
}

NS_CC_END
