/****************************************************************************
Copyright (c) 2012 cocos2d-x.org
Copyright (c) 2013-2014 Chukong Technologies Inc.

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

#include "Camera3DTest.h"
#include "3d/CCCamera3D.h"
#include <algorithm>
#include "../testResource.h"
 #include "3d/CCParticleSystem3D.h"
enum
{
	IDC_NEXT = 100,
	IDC_BACK,
	IDC_RESTART
};

static int sceneIdx = -1;

static std::function<Layer*()> createFunctions[] =
{
	CL(Camera3DTestDemo)
};
#define MAX_LAYER    (sizeof(createFunctions) / sizeof(createFunctions[0]))

static Layer* nextSpriteTestAction()
{
	auto layer = (createFunctions[0])();
	return layer;
}

static Layer* backSpriteTestAction()
{
	sceneIdx--;
	int total = MAX_LAYER;
	if( sceneIdx < 0 )
		sceneIdx += total;

	auto layer = (createFunctions[0])();
	return layer;
}

static Layer* restartSpriteTestAction()
{
	auto layer = (createFunctions[0])();
	return layer;
}

//------------------------------------------------------------------
//
// SpriteTestDemo
//
//------------------------------------------------------------------

Camera3DTestDemo::Camera3DTestDemo(void)
	: BaseTest()
{
	_sprite3D=NULL;
	auto s = Director::getInstance()->getWinSize();
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto listener = EventListenerTouchAllAtOnce::create();
	/*listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(Camera3DTestDemo::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(Camera3DTestDemo::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(Camera3DTestDemo::onTouchEnded, this);*/
	listener->onTouchesBegan = CC_CALLBACK_2(Camera3DTestDemo::onTouchesBegan, this);
    listener->onTouchesMoved = CC_CALLBACK_2(Camera3DTestDemo::onTouchesMoved, this);
    listener->onTouchesEnded = CC_CALLBACK_2(Camera3DTestDemo::onTouchesEnded, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	auto layer3D=Layer3D::create();
	addChild(layer3D,0);
	_layer3D=layer3D;
	_ViewType = false;
	addNewSpriteWithCoords( Vec3(s.width/2, s.height/2,0),"Sprite3DTest/scene.c3t",false,false);
	addNewSpriteWithCoords( Vec3(s.width/2-50, s.height/2,0),"Sprite3DTest/orc.c3t",true,true);
	//addNewSpriteWithCoords( Vec3(s.width/2+50, s.height/2,0),"Sprite3DTest/orc.c3t",true,true);
	addNewSpriteWithCoords( Vec3(s.width/2, s.height/2,0),"Sprite3DTest/orc.c3t",true,true);
	addNewParticleSystemWithCoords(Vec3(s.width/2, s.height/2,0));
	TTFConfig ttfConfig("fonts/arial.ttf", 20);
	auto label1 = Label::createWithTTF(ttfConfig,"scale+");
	auto menuItem1 = MenuItemLabel::create(label1, CC_CALLBACK_1(Camera3DTestDemo::scaleCameraCallback,this,2));
	auto label2 = Label::createWithTTF(ttfConfig,"scale-");
	auto menuItem2 = MenuItemLabel::create(label2, CC_CALLBACK_1(Camera3DTestDemo::scaleCameraCallback,this,-2));
	auto label3 = Label::createWithTTF(ttfConfig,"rotate+");
	auto menuItem3 = MenuItemLabel::create(label3, CC_CALLBACK_1(Camera3DTestDemo::rotateCameraCallback,this,10));
	auto label4 = Label::createWithTTF(ttfConfig,"rotate-");
	auto menuItem4 = MenuItemLabel::create(label4, CC_CALLBACK_1(Camera3DTestDemo::rotateCameraCallback,this,-10));
	auto label5 = Label::createWithTTF(ttfConfig,"Switch View");
	auto menuItem5 = MenuItemLabel::create(label5, CC_CALLBACK_1(Camera3DTestDemo::SwitchViewCallback,this));
	auto menu = Menu::create(menuItem1,menuItem2,menuItem3,menuItem4,menuItem5,NULL);

	menu->setPosition(Vec2::ZERO);
	menuItem1->setPosition( Vec2( s.width-50, 280 ) );
	menuItem2->setPosition( Vec2( s.width-50, 240) );
	menuItem3->setPosition( Vec2( s.width-50, 200) );
	menuItem4->setPosition( Vec2( s.width-50, 160) );
	menuItem5->setPosition( Vec2( s.width-50, 120) );
	addChild(menu, 0);

	
	TTFConfig ttfCamera("fonts/arial.ttf", 10);
	_labelRolePos = Label::createWithTTF(ttfCamera,"Role :Position: 0 , 0 , 0 ");
	Vec2 tAnchor(0,0);
	_labelRolePos->setAnchorPoint(tAnchor);
	_labelRolePos->setPosition(10,200);

	_labelCameraPos = Label::createWithTTF(ttfCamera,"Camera : Eye Position: 0 , 0 , 0 , LookAt Position : 0 , 0 , 0 ");
	_labelCameraPos->setAnchorPoint(tAnchor);
	_labelCameraPos->setPosition(10,220);
	addChild(_labelRolePos, 0);
	addChild(_labelCameraPos, 0);
	schedule(schedule_selector(Camera3DTestDemo::updatelabel), 0.0f);  
    _Camerar=Camera3D::createPerspective(60, (GLfloat)s.width/s.height, 1, 1000);
    _Camerar->retain();
    _Camerar->lookAt(_sprite3D->getPosition3D()+Vec3(0, 0, 50),Vec3(0, 1, 0),_sprite3D->getPosition3D()+Vec3(0, 0, 0));
	_Camerar->setActiveCamera();
	_layer3D->addChild(_Camerar);
}
Camera3DTestDemo::~Camera3DTestDemo(void)
{
}

void Camera3DTestDemo::addNewParticleSystemWithCoords(Vec3 p)
{
	auto particleSystem3D = ParticleSystem3D::create("Particle3D\\particle3Dtest.particle");
	particleSystem3D->setPosition3D(p);
	particleSystem3D->setScale(1);
	particleSystem3D->start();
	_layer3D->addChild(particleSystem3D,0);
	//particleSystem3D->save("E:\\liuliangWork\\cocos2d-x\\tests\\cpp-tests\\Resources\\Particle3D\\particle3Dtest.particle");
}
std::string Camera3DTestDemo::title() const
{
	return "Testing Camera3D";
}

std::string Camera3DTestDemo::subtitle() const
{
	return "";
}
void Camera3DTestDemo::scaleCameraCallback(Ref* sender,float value)
{
	Camera3D::getActiveCamera()->scale(value); 	
}	
void Camera3DTestDemo::rotateCameraCallback(Ref* sender,float value)
{
	if(!_ViewType)
	{
		Camera3D::getActiveCamera()->rotateAlong(Vec3(0,1,0),value); 
	}
	else
	{
		_sprite3D->setRotation3D( _sprite3D->getRotation3D()+Vec3(0,value,0));
	
	}	
}
void Camera3DTestDemo::SwitchViewCallback(Ref* sender)
{
	_ViewType = !_ViewType;
	if(!_ViewType)
	{
		Camera3D::getActiveCamera()->removeFromParentAndCleanup(false);
		_layer3D->addChild(Camera3D::getActiveCamera());
		Camera3D::getActiveCamera()->lookAt(_sprite3D->getPosition3D()+Vec3(0, 0, 50),Vec3(0, 1, 0), _sprite3D->getPosition3D());
	}
	else
	{
		Camera3D::getActiveCamera()->removeFromParentAndCleanup(false);
		_sprite3D->addChild(Camera3D::getActiveCamera());
		_sprite3D->setPosition3D(Vec3(240, 160, 0));
		Camera3D::getActiveCamera()->lookAt(Vec3(0, 0, 50),Vec3(0, 1, 0),Vec3(0, 0, 0));
	}
}
void Camera3DTestDemo::onEnter()
{
	BaseTest::onEnter();
}

void Camera3DTestDemo::restartCallback(Ref* sender)
{
	auto s = new Camera3DTestScene();
	s->addChild(restartSpriteTestAction());

	Director::getInstance()->replaceScene(s);
	s->release();
}

void Camera3DTestDemo::nextCallback(Ref* sender)
{
	auto s = new Camera3DTestScene();
	s->addChild( nextSpriteTestAction() );
	Director::getInstance()->replaceScene(s);
	s->release();
}

void Camera3DTestDemo::backCallback(Ref* sender)
{
	auto s = new Camera3DTestScene();
	s->addChild( backSpriteTestAction() );
	Director::getInstance()->replaceScene(s);
	s->release();
}
void Camera3DTestDemo::addNewSpriteWithCoords(Vec3 p,std::string fileName,bool playAnimation,bool bindCamera)
{
	
	auto sprite = Sprite3D::create(fileName);
	sprite->setScale(1);
	_layer3D->addChild(sprite);
	float globalZOrder=sprite->getGlobalZOrder();
	sprite->setPosition3D( Vec3( p.x, p.y,p.z) );
	sprite->setGlobalZOrder(globalZOrder);
	if(playAnimation)
	{
		auto animation = Animation3D::create(fileName);
		if (animation)
		{
		auto animate = Animate3D::create(animation);
		bool inverse = (std::rand() % 3 == 0);

		int rand2 = std::rand();
		float speed = 1.0f;
		if(rand2 % 3 == 1)
		{
		speed = animate->getSpeed() + CCRANDOM_0_1();
		}
		else if(rand2 % 3 == 2)
		{
		speed = animate->getSpeed() - 0.5 * CCRANDOM_0_1();
		}
		animate->setSpeed(inverse ? -speed : speed);
		sprite->runAction(RepeatForever::create(animate));
		}
	}
	if(bindCamera)
	{
		_sprite3D=sprite;
			
	}
	else
	{
		sprite->setScale(5);
		sprite->setRotation3D(Vec3(0,0,0));
	}
}
void Camera3DTestDemo::onTouchesBegan(const std::vector<Touch*>& touches, cocos2d::Event  *event)
{
	for ( auto &item: touches )
    {
        auto touch = item;
        auto location = touch->getLocation();

    }
}
void Camera3DTestDemo::onTouchesMoved(const std::vector<Touch*>& touches, cocos2d::Event  *event)
{
	if(touches.size()==1)
	{
		auto touch = touches[0];
		auto location = touch->getLocation();
		Point newPos = touch->getPreviousLocation()-location;
		if(!_ViewType && _sprite3D)
		{

			Camera3D::getActiveCamera()->translate(Vec3(newPos.x*0.1,newPos.y*0.1,0));
		}
		else
		{
			float globalZOrder=_sprite3D->getGlobalZOrder();
			_sprite3D->setPosition3D( _sprite3D->getPosition3D()+Vec3(newPos.x*0.1,newPos.y*0.1,0));
			_sprite3D->setGlobalZOrder(globalZOrder);

		}
	}
	else if(touches.size()==2)
	{
		auto lastDistance = (touches[0]->getPreviousLocation()-touches[1]->getPreviousLocation()).length();
		auto newDistance = (touches[0]->getLocation()-touches[1]->getLocation()).length();
		if(lastDistance!=newDistance)
		{
			float scale = newDistance/lastDistance;  
			Camera3D::getActiveCamera()->scale(scale);
		}
		else
		{
			 float angleY=0;
			if(touches[0]->getLocation()!=touches[0]->getPreviousLocation())
			{
				auto off =touches[0]->getLocation()- touches[0]->getPreviousLocation();
				angleY=off.y*0.4;
			}
			else  if(touches[1]->getLocation()!=touches[1]->getPreviousLocation())
			{
				auto off =touches[0]->getLocation()- touches[0]->getPreviousLocation();
				angleY=off.y*0.4;
			}
			if(!_ViewType)
			{
				Camera3D::getActiveCamera()->rotateAlong(Vec3(0,1,0),angleY); 
			}
			else
			{
				_sprite3D->setRotation3D( _sprite3D->getRotation3D()+Vec3(0,angleY,0));

			}	
		}
	}
}
void Camera3DTestDemo::onTouchesEnded(const std::vector<Touch*>& touches, cocos2d::Event  *event)
{
	for ( auto &item: touches )
    {
        auto touch = item;
        auto location = touch->getLocation();
    }
}
void onTouchesCancelled(const std::vector<Touch*>& touches, cocos2d::Event  *event)
{
}
void Camera3DTestDemo::updatelabel(float fDelta)
{
	if(_sprite3D)
	{
		auto  vPosition_sprite =_sprite3D->getPosition3D();
		char   szText[100];
		sprintf(szText,"Role :Position: %.2f , %.2f , %.2f ",vPosition_sprite.x,vPosition_sprite.y,vPosition_sprite.z);
		std::string str = szText;

		_labelRolePos->setString(str);

		auto  vPosition_Eye		= Camera3D::getActiveCamera()->getEyePos();
		auto  vPosition_LookAt	= Camera3D::getActiveCamera()->getLookPos();
		sprintf(szText,"Camera : Eye Position:  %.2f , %.2f , %.2f  , LookAt Position :  %.2f , %.2f , %.2f  ",vPosition_Eye.x,vPosition_Eye.y,vPosition_Eye.z,vPosition_LookAt.x,vPosition_LookAt.y,vPosition_LookAt.z);
		std::string str2 = szText;
		_labelCameraPos->setString(str2);

	}

}

void Camera3DTestScene::runThisTest()
{
	auto layer = nextSpriteTestAction();
	addChild(layer);
	Director::getInstance()->replaceScene(this);
}


bool Layer3D::init()
{
	 auto s = Director::getInstance()->getWinSize();
	return true;
}
void Layer3D::visit(Renderer *renderer, const Mat4& parentTransform, uint32_t parentFlags)
{
	if (!_visible)
	{
		return;
	}
	_groupCommand.init(_globalZOrder);
	renderer->addCommand(&_groupCommand);
	renderer->pushGroup(_groupCommand.getRenderQueueID());
	bool dirty = (parentFlags & FLAGS_TRANSFORM_DIRTY) || _transformUpdated;
	if(dirty)
		_modelViewTransform = this->transform(parentTransform);
	_transformUpdated = false;
	Director* director = Director::getInstance();
	director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, _modelViewTransform);
	Camera3D::getActiveCamera()->applyProjection();
	//_BeginCommand.init(_globalZOrder);
	_BeginCommand.func = CC_CALLBACK_0(Layer3D::onBeginDraw, this);
	renderer->addCommand(&_BeginCommand);
	int i = 0;
	if(!_children.empty())
	{
		sortAllChildren();
		// draw children zOrder < 0
		/*for( ; i < _children.size(); i++ )
		{
			auto node = _children.at(i);

			if ( node && node->getLocalZOrder() < 0 )
				node->visit(renderer, _modelViewTransform, dirty);
			else
				break;
		}*/
		// self draw
		this->draw(renderer, _modelViewTransform, dirty);

		for(auto it=_children.cbegin()+i; it != _children.cend(); ++it)
			(*it)->visit(renderer, _modelViewTransform, dirty);
	}
	else
	{
		this->draw(renderer, _modelViewTransform, dirty);
	}
	_orderOfArrival = 0;
	//_EndCommand.init(_globalZOrder);
	_EndCommand.func = CC_CALLBACK_0(Layer3D::onEndDraw, this);
	renderer->addCommand(&_EndCommand);
	renderer->popGroup();
	director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}
void Layer3D::onBeginDraw()
{
	Director *director = Director::getInstance();
	_directorProjection = director->getProjection();
}
void Layer3D::onEndDraw()
{
	Director *director = Director::getInstance();
	director->setProjection(_directorProjection);
}
