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

#include "3dparticle/ParticleUniverse/ParticleEventHandlers/CCPUParticle3DEventHandlerManager.h"
#include "3dparticle/ParticleUniverse/ParticleEventHandlers/CCPUParticle3DDoAffectorEventHandler.h"
#include "3dparticle/ParticleUniverse/ParticleEventHandlers/CCPUParticle3DDoEnableComponentEventHandler.h"
#include "3dparticle/ParticleUniverse/ParticleEventHandlers/CCPUParticle3DDoExpireEventHandler.h"
#include "3dparticle/ParticleUniverse/ParticleEventHandlers/CCPUParticle3DDoFreezeEventHandler.h"
#include "3dparticle/ParticleUniverse/ParticleEventHandlers/CCPUParticle3DDoPlacementParticleEventHandler.h"
#include "3dparticle/ParticleUniverse/ParticleEventHandlers/CCPUParticle3DDoScaleEventHandler.h"
#include "3dparticle/ParticleUniverse/ParticleEventHandlers/CCPUParticle3DDoStopSystemEventHandler.h"

NS_CC_BEGIN
PUParticle3DEventHandlerManager::PUParticle3DEventHandlerManager()
{
    
}

PUParticle3DEventHandlerManager::~PUParticle3DEventHandlerManager()
{
    
}

PUParticle3DEventHandlerManager* PUParticle3DEventHandlerManager::Instance()
{
    static PUParticle3DEventHandlerManager pem;
    return &pem;
}

PUScriptTranslator* PUParticle3DEventHandlerManager::getTranslator( const std::string &type )
{
    //if (type == "Align"){
    //    return &_alignAffectorTranslator;
    //}else if (type == "Dummy02"){
    //    return &_baseColliderTranslator;
    //}else if (type == "Dummy01"){
    //    return &_baseForceAffectorTranslator;
    //}else if (type == "BoxCollider"){
    //    return &_boxColliderTranslator;
    //}else if (type == "CollisionAvoidance"){
    //    return &_collisionAvoidanceAffectorTranstor;
    //}else if (type == "Colour"){
    //    return &_colorAffectorTranslator;
    //}else if (type == "FlockCentering"){
    //    return &_flockCenteringAffectorTranslator;
    //}else if (type == "ForceField"){
    //    return &_forceFieldAffectorTranslator;
    //}else if (type == "GeometryRotator"){
    //    return &_geometryRotatorTranslator;
    //}else if (type == "Gravity"){
    //    return &_gravityAffectorTranslator;
    //}else if (type == "InterParticleCollider"){
    //    return &_interParticleColliderTranslator;
    //}else if (type == "Jet"){
    //    return &_jetAffectorTranslator;
    //}else if (type == "Line"){
    //    return &_lineAffectorTranslator;
    //}else if (type == "LinearForce"){
    //    return &_linearForceAffectorTranslator;
    //}else if (type == "ParticleFollower"){
    //    return &_particleFollowerTranslator;
    //}else if (type == "PathFollower"){
    //    return &_pathFollowerTranslator;
    //}else if (type == "PlaneCollider"){
    //    return &_planeColliderTranslator;
    //}else if (type == "Randomiser"){
    //    return &_randomiserTranslator;
    //}else if (type == "Scale"){
    //    return &_scaleAffectorTranslator;
    //}else if (type == "ScaleVelocity"){
    //    return &_scaleVelocityAffectorTranslator;
    //}else if (type == "SineForce"){
    //    return &_sineForceAffectorTranslator;
    //}else if (type == "SphereCollider"){
    //    return &_sphereColliderTranslator;
    //}else if (type == "TextureAnimator"){
    //    return &_textureAnimatorTranslator;
    //}else if (type == "TextureRotator"){
    //    return &_textureRotatorTranslator;
    //}else if (type == "VelocityMatching"){
    //    return &_velocityMatchingAffectorTranslator;
    //}else if (type == "Vortex"){
    //    return &_vortexAffectorTranslator;
    //}
    return nullptr;
}

PUParticle3DEventHandler* PUParticle3DEventHandlerManager::createEventHandler( const std::string &type )
{
	if (type == "DoAffector"){
		return PUParticle3DDoAffectorEventHandler::create();
	}else if (type == "DoEnableComponent"){
		return PUParticle3DDoEnableComponentEventHandler::create();
	}else if (type == "DoExpire"){
		return PUParticle3DDoExpireEventHandler::create();
	}else if (type == "DoFreeze"){
		return PUParticle3DDoFreezeEventHandler::create();
	}else if (type == "DoPlacementParticle"){
		return PUParticle3DDoPlacementParticleEventHandler::create();
	}else if (type == "DoScale"){
		return PUParticle3DDoScaleEventHandler::create();
	}else if (type == "DoStopSystem"){
		return PUParticle3DDoStopSystemEventHandler::create();
	}
    return nullptr;
}

NS_CC_END