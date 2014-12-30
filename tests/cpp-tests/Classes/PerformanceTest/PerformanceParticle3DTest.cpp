#include "PerformanceParticle3DTest.h"
#include "3dparticle/CCParticle3DEmitter.h"
#include "3dparticle/CCParticle3DAffector.h"
#include "3dparticle/CCParticle3DRender.h"
#include "3dparticle/ParticleUniverse/ParticleEmitters/CCPUParticle3DPointEmitter.h"
#include "3dparticle/ParticleUniverse/ParticleEmitters/CCPUParticle3DBoxEmitter.h"
#include "3dparticle/ParticleUniverse/ParticleEmitters/CCPUParticle3DCircleEmitter.h"
#include "3dparticle/ParticleUniverse/ParticleEmitters/CCPUParticle3DLineEmitter.h"
#include "3dparticle/ParticleUniverse/ParticleEmitters/CCPUParticle3DPositionEmitter.h"
#include "3dparticle/ParticleUniverse/ParticleEmitters/CCPUParticle3DSphereSurfaceEmitter.h"
#include "3dparticle/ParticleUniverse/ParticleAffectors/CCPUParticle3DAlignAffector.h"
#include "3dparticle/ParticleUniverse/ParticleAffectors/CCPUParticle3DBoxCollider.h"
#include "3dparticle/ParticleUniverse/ParticleAffectors/CCPUParticle3DColorAffector.h"
#include "3dparticle/ParticleUniverse/ParticleAffectors/CCPUParticle3DGravityAffector.h"
#include "3dparticle/ParticleUniverse/ParticleAffectors/CCPUParticle3DScaleAffector.h"
#include "3dparticle/ParticleUniverse/ParticleAffectors/CCPUParticle3DFlockCenteringAffector.h"
#include "3dparticle/ParticleUniverse/ParticleAffectors/CCPUParticle3DForceFieldAffector.h"
#include "3dparticle/ParticleUniverse/ParticleAffectors/CCPUParticle3DLinearForceAffector.h"
#include "3dparticle/ParticleUniverse/ParticleAffectors/CCPUParticle3DParticleFollower.h"
#include "3dparticle/ParticleUniverse/ParticleAffectors/CCPUParticle3DPathFollower.h"
#include "3dparticle/ParticleUniverse/ParticleAffectors/CCPUParticle3DPlaneCollider.h"
#include "3dparticle/ParticleUniverse/ParticleAffectors/CCPUParticle3DRandomiser.h"
#include "3dparticle/ParticleUniverse/ParticleAffectors/CCPUParticle3DSineForceAffector.h"
#include "3dparticle/ParticleUniverse/ParticleAffectors/CCPUParticle3DTextureRotator.h"
#include "3dparticle/ParticleUniverse/ParticleAffectors/CCPUParticle3DVortexAffector.h"
#include "3dparticle/ParticleUniverse/CCPUParticleSystem3D.h"
#include "3dparticle/ParticleUniverse/CCPUParticle3DScriptCompiler.h"


enum {
    kTagInfoLayer = 1,
    kTagMainLayer = 2,
    kTagParticleSystem = 3,
    kTagLabelAtlas = 4,
    kTagMenuLayer = 1000,

    TEST_COUNT = 2,
};

enum {
    kMaxParticles = 14000,
    kNodesIncrease = 1,
};

static int s_nParCurIdx = 0;

////////////////////////////////////////////////////////
//
// ParticleMenuLayer
//
////////////////////////////////////////////////////////
Particle3DMenuLayer::Particle3DMenuLayer(bool bControlMenuVisible, int nMaxCases, int nCurCase)
: PerformBasicLayer(bControlMenuVisible, nMaxCases, nCurCase)
{

}

void Particle3DMenuLayer::showCurrentTest()
{
    auto scene = (Particle3DMainScene*)getParent();
    int subTest = scene->getSubTestNum();
    int parNum  = scene->getParticlesNum();

    Particle3DMainScene* pNewScene = nullptr;

    switch (_curCase)
    {
    case 0:
        pNewScene = new (std::nothrow) Particle3DPerformTest1;
        break;
    case 1:
        pNewScene = new (std::nothrow) Particle3DPerformTest2;
        break;
    }

    s_nParCurIdx = _curCase;
    if (pNewScene)
    {
        pNewScene->initWithSubTest(subTest, parNum);

        Director::getInstance()->replaceScene(pNewScene);
        pNewScene->release();
    }
}

////////////////////////////////////////////////////////
//
// ParticleMainScene
//
////////////////////////////////////////////////////////
void Particle3DMainScene::initWithSubTest(int asubtest, int particles)
{
    //srandom(0);

    subtestNumber = asubtest;
    auto s = Director::getInstance()->getWinSize();

    lastRenderedCount = 0;
    quantityParticles = particles;

    MenuItemFont::setFontSize(65);
    auto decrease = MenuItemFont::create(" - ", [&](Ref *sender) {
		quantityParticles -= kNodesIncrease;
		if( quantityParticles < 0 )
			quantityParticles = 0;

		updateQuantityLabel();
		removeChildByTag(kTagParticleSystem + quantityParticles, true);
		//createParticleSystem(quantityParticles);
	});
    decrease->setColor(Color3B(0,200,20));
    auto increase = MenuItemFont::create(" + ", [&](Ref *sender) {
		quantityParticles += kNodesIncrease;
		if( quantityParticles > kMaxParticles )
			quantityParticles = kMaxParticles;

		updateQuantityLabel();
		createParticleSystem(quantityParticles);
	});
    increase->setColor(Color3B(0,200,20));

    auto menu = Menu::create(decrease, increase, nullptr);
    menu->alignItemsHorizontally();
    menu->setPosition(Vec2(s.width/2, s.height/2+15));
    addChild(menu, 1);

    auto infoLabel = Label::createWithTTF("0 nodes", "fonts/Marker Felt.ttf", 30);
    infoLabel->setColor(Color3B(0,200,20));
    infoLabel->setPosition(Vec2(s.width/2, s.height - 90));
    addChild(infoLabel, 1, kTagInfoLayer);

    // particles on stage
    auto labelAtlas = LabelAtlas::create("0000", "fps_images.png", 12, 32, '.');
    addChild(labelAtlas, 0, kTagLabelAtlas);
    labelAtlas->setPosition(Vec2(s.width-66,50));

    // Next Prev Test
    auto menuLayer = new (std::nothrow) Particle3DMenuLayer(true, TEST_COUNT, s_nParCurIdx);
    addChild(menuLayer, 1, kTagMenuLayer);
    menuLayer->release();

    //// Sub Tests
    //MenuItemFont::setFontSize(40);
    //auto pSubMenu = Menu::create();
    //for (int i = 1; i <= 6; ++i)
    //{
    //    char str[10] = {0};
    //    sprintf(str, "%d ", i);
    //    auto itemFont = MenuItemFont::create(str, CC_CALLBACK_1(Particle3DMainScene::testNCallback, this));
    //    itemFont->setTag(i);
    //    pSubMenu->addChild(itemFont, 10);

    //    if (i <= 3)
    //    {
    //        itemFont->setColor(Color3B(200,20,20));
    //    }
    //    else
    //    {
    //        itemFont->setColor(Color3B(0,200,20));
    //    }
    //}
    //pSubMenu->alignItemsHorizontally();
    //pSubMenu->setPosition(Vec2(s.width/2, 80));
    //addChild(pSubMenu, 2);

    auto label = Label::createWithTTF(title().c_str(), "fonts/arial.ttf", 32);
    addChild(label, 1);
    label->setPosition(Vec2(s.width/2, s.height-50));

	auto camera = Camera::createPerspective(30.0f, s.width / s.height, 1.0f, 1000.0f);
	camera->setPosition3D(Vec3(0.0f, 0.0f, 100.0f));
	camera->lookAt(Vec3(0.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f));
	camera->setCameraFlag(CameraFlag::USER1);
	this->addChild(camera);

    updateQuantityLabel();
    createParticleSystem(quantityParticles);

    schedule(CC_SCHEDULE_SELECTOR(Particle3DMainScene::step));
}

std::string Particle3DMainScene::title() const
{
    return "No title";
}

void Particle3DMainScene::step(float dt)
{
    auto atlas = (LabelAtlas*) getChildByTag(kTagLabelAtlas);
    auto ps = (PUParticleSystem3D*) getChildByTag(kTagParticleSystem);

    char str[10] = {0};
    sprintf(str, "%4d", ps->getParticleQuota());
    atlas->setString(str);
}

void Particle3DMainScene::createParticleSystem(int idx)
{
   // removeChildByTag(kTagParticleSystem, true);

	auto ps = PUParticleSystem3D::create();
	BlendFunc blend;
	blend.src = GL_ONE;
	blend.dst = GL_ONE;
	ps->setBlendFunc(blend);
	ps->setKeepLocal(false);

	ps->setCameraMask((unsigned short)CameraFlag::USER1);
	ps->setPosition(CCRANDOM_MINUS1_1() * 50.0f, CCRANDOM_MINUS1_1() * 50.0f);
	addChild(ps, 0, kTagParticleSystem + idx);

    doTest();
}

void Particle3DMainScene::testNCallback(Ref* sender)
{
    subtestNumber = static_cast<Node*>(sender)->getTag();

    auto menu = static_cast<Particle3DMenuLayer*>( getChildByTag(kTagMenuLayer) );
    menu->restartCallback(sender);
}

void Particle3DMainScene::updateQuantityLabel()
{
    if( quantityParticles != lastRenderedCount )
    {
        auto infoLabel = (Label *) getChildByTag(kTagInfoLayer);
        char str[20] = {0};
        sprintf(str, "%u particles", quantityParticles);
        infoLabel->setString(str);

        lastRenderedCount = quantityParticles;
    }
}

////////////////////////////////////////////////////////
//
// ParticlePerformTest1
//
////////////////////////////////////////////////////////
std::string Particle3DPerformTest1::title() const
{
    char str[20] = {0};
    sprintf(str, "Particle3D Test"/*, subtestNumber*/);
    std::string strRet = str;
    return strRet;
}

void Particle3DPerformTest1::doTest()
{
    auto s = Director::getInstance()->getWinSize();
    auto ps = (PUParticleSystem3D*)getChildByTag(kTagParticleSystem);
	ps->setParticleQuota(100);
	//emitter
	{
		auto ppe = PUParticle3DSphereSurfaceEmitter::create();
		ppe->setRadius(3.0f);
		ppe->setParticleDirection(Vec3(0.0f, 0.0f, 0.0f));
		//ppe->setForceEmission(true);
		//PUDynamicAttributeFixed *angle = new PUDynamicAttributeFixed();
		//angle->setValue(0.0f);
		//ppe->setDynAngle(angle);
		PUDynamicAttributeFixed *velocity = new PUDynamicAttributeFixed();
		velocity->setValue(0.0f);
		ppe->setDynVelocity(velocity);
		PUDynamicAttributeFixed *alive = new PUDynamicAttributeFixed();
		alive->setValue(1.8f);
		ppe->setDynTotalTimeToLive(alive);
		PUDynamicAttributeFixed *dim = new PUDynamicAttributeFixed();
		dim->setValue(2.0f);
		ppe->setDynParticleAllDimensions(dim);
		PUDynamicAttributeFixed *rate = new PUDynamicAttributeFixed();
		rate->setValue(55.0f);
		ppe->setDynEmissionRate(rate);
		//ppe->setParticleColorRangeStart(Vec4(0.0f, 0.0f, 0.0f, 1.0f));
		//ppe->setParticleColorRangeEnd(Vec4(1.0f, 1.0f, 1.0f, 1.0f));
		ps->addEmitter(ppe);
	}

	//affector
	{
		auto pca = PUParticle3DColorAffector::create();
		pca->addColor(0.0f, Vec4(1.0f, 1.0f, 0.0f, 1.0f));
		pca->addColor(1.0f, Vec4(1.0f, 0.0f, 0.0f, 1.0f));
		ps->addAffector(pca);

		auto pga = PUParticle3DGravityAffector::create();
		pga->setMass(0.03f);
		pga->setGravity(150.0f);
		ps->addAffector(pga);

		//auto psa = PUParticle3DScaleAffector::create();
		//PUDynamicAttributeFixed *scl = new PUDynamicAttributeFixed();
		//scl->setValue(-4.5f);
		//psa->setDynScaleXYZ(scl);
		//ps->addAffector(psa);

		//      auto pfca = PUParticle3DFlockCenteringAffector::create();
		//      ps->addAffector(pfca);

		//auto pffa = PUParticle3DForceFieldAffector::create();
		//ps->addAffector(pffa);

		//auto plfa = PUParticle3DLinearForceAffector::create();
		//plfa->setForceVector(Vec3(0.0f, 10.0f, 0.0f));
		//ps->addAffector(plfa);

		//auto ppf = PUParticle3DParticleFollower::create();
		//ps->addAffector(ppf);

		//auto pptf = PUParticle3DPathFollower::create();
		//pptf->addPoint(Vec3(0.0f, 0.0f, 0.0f));
		//pptf->addPoint(Vec3(50.0f, 50.0f, -50.0f));
		//pptf->addPoint(Vec3(0.0f, 50.0f, -50.0f));
		//pptf->addPoint(Vec3(50.0f, 0.0f, 0.0f));
		//pptf->addPoint(Vec3(0.0f, 0.0f, 0.0f));
		//ps->addAffector(pptf);

		//auto ppc = PUParticle3DPlaneCollider::create();
		//ppc->setNormal(Vec3(0.0f, 1.0f, 0.0f));
		//ps->addAffector(ppc);

		//auto pr = PUParticle3DRandomiser::create();
		//ps->addAffector(pr);

		//auto psfa = PUParticle3DSineForceAffector::create();
		//psfa->setForceVector(Vec3(0.0f, 10.0f, 0.0f));
		//psfa->setFrequencyMax(10.0f);
		//ps->addAffector(psfa);

		//auto ptr = PUParticle3DTextureRotator::create();
		//PUDynamicAttributeFixed *rSpeed = new PUDynamicAttributeFixed();
		//rSpeed->setValue(2.0f);
		//ptr->setRotationSpeed(rSpeed);
		//ps->addAffector(ptr);

		//auto pva = PUParticle3DVortexAffector::create();
		//PUDynamicAttributeFixed *rSpeed = new PUDynamicAttributeFixed();
		//rSpeed->setValue(10.0f);
		//pva->setRotationSpeed(rSpeed);
		//pva->setRotationVector(Vec3(0.0f, 1.0f, 0.0f));
		//ps->addAffector(pva);
	}

	//render
	{
		auto pr = Particle3DQuadRender::create("Particle3D/textures/pump_flare_04.png");
		ps->setRender(pr);
	}

	ps->startParticle();
}

////////////////////////////////////////////////////////
//
// ParticlePerformTest2
//
////////////////////////////////////////////////////////
std::string Particle3DPerformTest2::title() const
{
    char str[20] = {0};
    sprintf(str, "B (%d) size=8", subtestNumber);
    std::string strRet = str;
    return strRet;
}

void Particle3DPerformTest2::doTest()
{
    auto s = Director::getInstance()->getWinSize();
    auto particleSystem = (ParticleSystem*) getChildByTag(kTagParticleSystem);

    // duration
    particleSystem->setDuration(-1);

    // gravity
    particleSystem->setGravity(Vec2(0,-90));

    // angle
    particleSystem->setAngle(90);
    particleSystem->setAngleVar(0);

    // radial
    particleSystem->setRadialAccel(0);
    particleSystem->setRadialAccelVar(0);

    // speed of particles
    particleSystem->setSpeed(180);
    particleSystem->setSpeedVar(50);

    // emitter position
    particleSystem->setPosition(Vec2(s.width/2, 100));
    particleSystem->setPosVar(Vec2(s.width/2,0));

    // life of particles
    particleSystem->setLife(2.0f);
    particleSystem->setLifeVar(1);

    // emits per frame
    particleSystem->setEmissionRate(particleSystem->getTotalParticles() / particleSystem->getLife());

    // color of particles
    Color4F startColor(0.5f, 0.5f, 0.5f, 1.0f);
    particleSystem->setStartColor(startColor);

    Color4F startColorVar(0.5f, 0.5f, 0.5f, 1.0f);
    particleSystem->setStartColorVar(startColorVar);

    Color4F endColor(0.1f, 0.1f, 0.1f, 0.2f);
    particleSystem->setEndColor(endColor);

    Color4F endColorVar(0.1f, 0.1f, 0.1f, 0.2f);    
    particleSystem->setEndColorVar(endColorVar);

    // size, in pixels
    particleSystem->setEndSize(8.0f);
    particleSystem->setStartSize(8.0f);
    particleSystem->setEndSizeVar(0);
    particleSystem->setStartSizeVar(0);

    // additive
    particleSystem->setBlendAdditive(false);
}

void runParticle3DTest()
{
    auto scene = new (std::nothrow) Particle3DPerformTest1;
    scene->initWithSubTest(1, kNodesIncrease);

    Director::getInstance()->replaceScene(scene);
    scene->release();
}
