#include "Runner3DTest.h"

#define MAX_COIN_NUM 10
#define MAX_COIN_SPACE 20
#define MIN_COLLISION_SQUAREDIS 9.0
static const float coinInitOffset = 20.0f;


static int sceneIdx = -1;

static std::function<Layer*()> createFunctions[] =
{
    CL(Runner3DTestDemo),
};
#define MAX_LAYER    (sizeof(createFunctions) / sizeof(createFunctions[0]))

static Layer* nextRunner3DTestAction()
{
    sceneIdx++;
    sceneIdx = sceneIdx % MAX_LAYER;
    auto layer = (createFunctions[sceneIdx])();
    return layer;
}

static Layer* backRunner3DTestAction()
{
    sceneIdx--;
    int total = MAX_LAYER;
    if( sceneIdx < 0 )
        sceneIdx += total;

    auto layer = (createFunctions[sceneIdx])();
    return layer;
}

static Layer* restartRunner3DTestAction()
{
    auto layer = (createFunctions[sceneIdx])();
    return layer;
}

Runner3DTestDemo::Runner3DTestDemo()
    : _layer3D(nullptr)
    , _camera(nullptr)
    , _sprite(nullptr)
    , _scoreLabel(nullptr)
    , _spriteAnim(nullptr)
    , _keyCode(cocos2d::EventKeyboard::KeyCode::KEY_NONE)
    , _moveSpeed(50.0f)
    , _coinRotSpeed(50.0f)
    , _modelOffsetRot(0.0f)
    , _collectedCoinNum(0)
{
    scheduleUpdate();
}


Runner3DTestDemo::~Runner3DTestDemo()
{
}

std::string Runner3DTestDemo::title() const
{
    return "Runner3D Demo";
}

void Runner3DTestDemo::restartCallback( Ref* sender )
{
    auto s = new Runner3DTestScene();
    s->addChild(restartRunner3DTestAction());

    Director::getInstance()->replaceScene(s);
    s->release();
}

void Runner3DTestDemo::nextCallback( Ref* sender )
{
    auto s = new Runner3DTestScene();
    s->addChild(nextRunner3DTestAction());

    Director::getInstance()->replaceScene(s);
    s->release();
}

void Runner3DTestDemo::backCallback( Ref* sender )
{
    auto s = new Runner3DTestScene();
    s->addChild(backRunner3DTestAction());

    Director::getInstance()->replaceScene(s);
    s->release();
}

void Runner3DTestDemo::onEnter()
{
    BaseTest::onEnter();
    initDemo();
}

void Runner3DTestDemo::onExit()
{
    BaseTest::onExit();

    _createdCoinList.clear();
    _scoreLabel->release();
    _spriteAnim->release();
    _sprite->release();

}

void Runner3DTestDemo::initDemo()
{
    _layer3D = cocos2d::Layer::create();
    addChild(_layer3D);

    initCamera(_layer3D);
    init3DScene(_layer3D);
    initEvent();

    //important
    _layer3D->setCameraMask(2);
    //
}

void Runner3DTestDemo::initEvent()
{
    auto keyListener = cocos2d::EventListenerKeyboard::create();
    keyListener->onKeyPressed = CC_CALLBACK_2(Runner3DTestDemo::onKeyPressed, this);
    keyListener->onKeyReleased = CC_CALLBACK_2(Runner3DTestDemo::onKeyReleased, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyListener, this);
}

void Runner3DTestDemo::initCamera(cocos2d::Node *parent)
{
    auto s = Director::getInstance()->getWinSize();
    _camera = cocos2d::Camera::createPerspective(60.0f, s.width / s.height, 1.0f, 10000.0f);
    _camera->setCameraFlag(cocos2d::CameraFlag::USER1);
    _camera->setPosition3D(cocos2d::Vec3(0.0f, 50.0f, 100.0f));
    _camera->lookAt(cocos2d::Vec3(0.0f, 0.0f, 0.0f), Vec3(0,1,0));
    parent->addChild(_camera);
}

void Runner3DTestDemo::init3DScene(cocos2d::Node *parent)
{
    _sprite = cocos2d::Sprite3D::create("Sprite3DTest/girl.c3b");
    _sprite->retain();
    _sprite->setScale(0.2f);
    _modelOffsetRot = 45.0f;
    auto animation = cocos2d::Animation3D::create("Sprite3DTest/girl.c3b","Take 001");
    auto animate = cocos2d::Animate3D::create(animation);
    //animate->setSpeed(_moveSpeed);
    _spriteAnim = cocos2d::RepeatForever::create(animate);
    _spriteAnim->retain();
    parent->addChild(_sprite);

    auto coinLayer = Layer::create();
    for (unsigned int i = 0; i < MAX_COIN_NUM; ++i)
    {
        auto coin = CreateIcon(cocos2d::Vec3(MAX_COIN_SPACE * i + 10.0f, 0.0, 0.0));
        _createdCoinList.push_back(coin);
        coinLayer->addChild(coin);
    }
    parent->addChild(coinLayer);


    cocos2d::Size sz = cocos2d::Director::getInstance()->getWinSize();
    TTFConfig ttfConfig("fonts/arial.ttf", 10, GlyphCollection::DYNAMIC,nullptr,true);
    _scoreLabel = cocos2d::Label::createWithTTF(ttfConfig,"0 : Score",TextHAlignment::CENTER, sz.width);
    _scoreLabel->retain();
    _scoreLabel->setAnchorPoint(cocos2d::Vec2::ANCHOR_TOP_RIGHT);
    _scoreLabel->setPosition(cocos2d::Vec2(sz.width - 10.0f, sz.height - 10.0f));
    _scoreLabel->setColor(cocos2d::Color3B::WHITE);
    parent->addChild(_scoreLabel);

    ttfConfig.fontSize = 6;
    auto lab = cocos2d::Label::createWithTTF(ttfConfig,"UP - 'W'\nDOWN - 'S'\nLEFT - 'A'\nRIGHT - 'D'",TextHAlignment::CENTER, sz.width);
    lab->setAnchorPoint(cocos2d::Vec2::ANCHOR_TOP_LEFT);
    lab->setPosition(cocos2d::Vec2(0.0f, sz.height - 10.0f));
    lab->setColor(cocos2d::Color3B::WHITE);
    parent->addChild(lab);
}

void Runner3DTestDemo::onKeyPressed( cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event )
{
    if (_keyCode == cocos2d::EventKeyboard::KeyCode::KEY_NONE)
    {
        _keyCode = keyCode;
        if (_sprite)
            _sprite->runAction(_spriteAnim);
    }
}

void Runner3DTestDemo::onKeyReleased( cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event )
{
    _keyCode = cocos2d::EventKeyboard::KeyCode::KEY_NONE;
    if (_sprite)
        _sprite->stopAction(_spriteAnim);
}

void Runner3DTestDemo::update( float dt )
{
    bool needUpdateCam = false;
    if (_sprite)
    {
        switch (_keyCode)
        {
        case cocos2d::EventKeyboard::KeyCode::KEY_W:
            {
                _sprite->setPositionZ(_sprite->getPositionZ() - _moveSpeed * dt);
                _sprite->setRotation3D(Vec3(0.0f, 180.0f + _modelOffsetRot, 0.0f));
                needUpdateCam = true;
            }
            break;

        case cocos2d::EventKeyboard::KeyCode::KEY_S:
            {
                _sprite->setPositionZ(_sprite->getPositionZ() + _moveSpeed * dt);
                _sprite->setRotation3D(Vec3(0.0f, 0.0f + _modelOffsetRot, 0.0f));
                needUpdateCam = true;
            }
            break;

        case cocos2d::EventKeyboard::KeyCode::KEY_A:
            {
                _sprite->setPositionX(_sprite->getPositionX() - _moveSpeed * dt);
                _sprite->setRotation3D(Vec3(0.0f, -90.0f + _modelOffsetRot, 0.0f));
                needUpdateCam = true;
            }
            break;

        case cocos2d::EventKeyboard::KeyCode::KEY_D:
            {
                _sprite->setPositionX(_sprite->getPositionX() + _moveSpeed * dt);
                _sprite->setRotation3D(Vec3(0.0f, 90.0f + _modelOffsetRot, 0.0f));
                needUpdateCam = true;
            }
            break;

        default:
            break;
        }
    }

	updateCoins(dt);
	if (needUpdateCam)
		updateCamera();
}

bool Runner3DTestDemo::checkCollectedCoin(cocos2d::Sprite3D *coin)
{
    cocos2d::Vec3 spritePos = _sprite->getPosition3D();
    spritePos.y += coinInitOffset;
    float sqdis = (coin->getPosition3D() - spritePos).lengthSquared();

    if (sqdis <= MIN_COLLISION_SQUAREDIS) 
        return true;

    return false;
}

void Runner3DTestDemo::updateScore()
{
    ++_collectedCoinNum;
    char string[32] = {0};
    sprintf(string, "%d : Score", _collectedCoinNum);
    _scoreLabel->setString(string);
}

cocos2d::Sprite3D* Runner3DTestDemo::CreateIcon(const cocos2d::Vec3 &pos)
{
	auto coin = cocos2d::Sprite3D::create("Sprite3DTest/coin.obj");
	coin->setPosition3D(pos);
	coin->setPositionY(coinInitOffset);
	return coin;
}

void Runner3DTestDemo::updateCoins(float dt)
{
	if (!_createdCoinList.empty())
	{
		if (checkCoinOutOfBound(_createdCoinList.front()))
		{
			_needRemovCoinList.push_back(_createdCoinList.front());
			_createdCoinList.pop_front();
		}

		for (CoinList::iterator iter = _createdCoinList.begin(); iter != _createdCoinList.end(); )
		{
			auto coin = *iter;
			//if (sprite->isVisible())
			{
				cocos2d::Vec3 rot = coin->getRotation3D();
				rot.y += dt * _coinRotSpeed;
				if (360.0f < rot.y) 
					rot.y -= 360.0f;

				coin->setRotation3D(rot);
			}

			if (checkCollectedCoin(coin))
			{
				//coin->setVisible(false);
				updateScore();
				_needRemovCoinList.push_back(coin);
				iter = _createdCoinList.erase(iter);
			}
			else
			{
				++iter;
			}
		}

	}

	if (!_needRemovCoinList.empty())
	{
		cocos2d::Vec3 initPos;
		if (!_createdCoinList.empty())
			initPos = _createdCoinList.back()->getPosition3D() + cocos2d::Vec3(MAX_COIN_SPACE, 0.0f, 0.0f);

		unsigned int idx = 0;
		for (CoinList::iterator iter = _needRemovCoinList.begin(); iter != _needRemovCoinList.end(); ++iter)
		{
			auto coin = *iter;
			coin->setPosition3D(initPos + cocos2d::Vec3(idx * MAX_COIN_SPACE, 0.0f, 0.0f));
			//coin->setVisible(true);
			_createdCoinList.push_back(coin);
			++idx;
		}

		_needRemovCoinList.clear();
	}
}

void Runner3DTestDemo::updateCamera()
{
	if (_camera)
	{
		_camera->lookAt(_sprite->getPosition3D(), cocos2d::Vec3(0.0f, 1.0f, 0.0f));
		_camera->setPosition3D(_sprite->getPosition3D() + cocos2d::Vec3(0.0f, 50.0f, 100.0f));
	}
}

bool Runner3DTestDemo::checkCoinOutOfBound( cocos2d::Sprite3D *coin )
{
	return false;
}

void Runner3DTestScene::runThisTest()
{
    auto layer = nextRunner3DTestAction();
    addChild(layer);
    cocos2d::Director::getInstance()->replaceScene(this);
}
