#include "Runner3DTest.h"

#define MAX_COIN_NUM 10
#define MAX_COIN_SPACE 20
#define MIN_COLLISION_SQUAREDIS 25.0
#define MIN_OUT_OF_BOUND_DIS 100
#define PROBABILITY_ARRAY_SIZE 10
#define RADIUS_TO_DEGREE(val) (val * 180.0f / 3.1415926535f)
static const float coinInitOffset = 20.0f;
static const unsigned int probabilityArray[PROBABILITY_ARRAY_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1};
static const std::string modelPathList[] = {"Sprite3DTest/coin.c3b"
                                           ,"Sprite3DTest/box.c3b" };

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
    , _coinLayer(nullptr)
    , _collectedCoinNum(0)
    , _newIconPos(cocos2d::Vec3(10.0f, 0.0f, 0.0f))
    , _isRunningAction(false)
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

    _scoreLabel->release();
    _coinLayer->release();
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
    {
        auto keyListener = cocos2d::EventListenerKeyboard::create();
        keyListener->onKeyPressed = CC_CALLBACK_2(Runner3DTestDemo::onKeyPressedEvent, this);
        keyListener->onKeyReleased = CC_CALLBACK_2(Runner3DTestDemo::onKeyReleasedEvent, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(keyListener, this);
    }

    {
        auto keyListener = cocos2d::EventListenerTouchOneByOne::create();
        keyListener->onTouchBegan = CC_CALLBACK_2(Runner3DTestDemo::onTouchBeganEvent, this);
        keyListener->onTouchMoved = CC_CALLBACK_2(Runner3DTestDemo::onTouchMovedEvent, this);
        keyListener->onTouchEnded = CC_CALLBACK_2(Runner3DTestDemo::onTouchEndedEvent, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(keyListener, this);
    }

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

    _coinLayer = Layer::create();
    _coinLayer->retain();
    for (unsigned int i = 0; i < MAX_COIN_NUM; ++i)
    {
        auto coin = CreateCoin(_newIconPos);
        _coinLayer->addChild(coin);
        _newIconPos += cocos2d::Vec3(MAX_COIN_SPACE, 0.0, 0.0);
    }
    parent->addChild(_coinLayer);


    cocos2d::Size sz = cocos2d::Director::getInstance()->getWinSize();
    TTFConfig ttfConfig("fonts/arial.ttf", 10, GlyphCollection::DYNAMIC,nullptr,true);
    _scoreLabel = cocos2d::Label::createWithTTF(ttfConfig,"0 : Score",TextHAlignment::CENTER, sz.width);
    _scoreLabel->retain();
    _scoreLabel->setAnchorPoint(cocos2d::Vec2::ANCHOR_TOP_RIGHT);
    _scoreLabel->setPosition(cocos2d::Vec2(sz.width - 10.0f, sz.height - 10.0f));
    _scoreLabel->setColor(cocos2d::Color3B::WHITE);
    parent->addChild(_scoreLabel);

    ttfConfig.fontSize = 6;
    auto lab = cocos2d::Label::createWithTTF(ttfConfig,"PC : UP - 'W'\nDOWN - 'S'\nLEFT - 'A'\nRIGHT - 'D'",TextHAlignment::CENTER, sz.width);
    lab->setAnchorPoint(cocos2d::Vec2::ANCHOR_TOP_LEFT);
    lab->setPosition(cocos2d::Vec2(0.0f, sz.height));
    lab->setColor(cocos2d::Color3B::WHITE);
    parent->addChild(lab);
}

void Runner3DTestDemo::onKeyPressedEvent( cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event )
{
    if (_keyCode == cocos2d::EventKeyboard::KeyCode::KEY_NONE)
    {
        _keyCode = keyCode;
        if (!_isRunningAction)
        {
            if (_sprite)
            {
                _sprite->runAction(_spriteAnim);
                _isRunningAction = true;
            }
        }
    }
}

void Runner3DTestDemo::onKeyReleasedEvent( cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event )
{
    _keyCode = cocos2d::EventKeyboard::KeyCode::KEY_NONE;
    if (_isRunningAction)
    {
        if (_sprite)
        {
            _sprite->stopAction(_spriteAnim);
            _isRunningAction = false;
        }
    }
}

void Runner3DTestDemo::update( float dt )
{
    if (_sprite)
    {
        switch (_keyCode)
        {
        case cocos2d::EventKeyboard::KeyCode::KEY_W:
            {
                _sprite->setPositionZ(_sprite->getPositionZ() - _moveSpeed * dt);
                _sprite->setRotation3D(Vec3(0.0f, 180.0f + _modelOffsetRot, 0.0f));
                _touchPos = _sprite->getPosition3D();
            }
            break;

        case cocos2d::EventKeyboard::KeyCode::KEY_S:
            {
                _sprite->setPositionZ(_sprite->getPositionZ() + _moveSpeed * dt);
                _sprite->setRotation3D(Vec3(0.0f, 0.0f + _modelOffsetRot, 0.0f));
                _touchPos = _sprite->getPosition3D();
            }
            break;

        case cocos2d::EventKeyboard::KeyCode::KEY_A:
            {
                _sprite->setPositionX(_sprite->getPositionX() - _moveSpeed * dt);
                _sprite->setRotation3D(Vec3(0.0f, -90.0f + _modelOffsetRot, 0.0f));
                _touchPos = _sprite->getPosition3D();
            }
            break;

        case cocos2d::EventKeyboard::KeyCode::KEY_D:
            {
                _sprite->setPositionX(_sprite->getPositionX() + _moveSpeed * dt);
                _sprite->setRotation3D(Vec3(0.0f, 90.0f + _modelOffsetRot, 0.0f));
                _touchPos = _sprite->getPosition3D();
            }
            break;

        default:
            break;
        }

        if (!vec3equals(_touchPos, _sprite->getPosition3D()))
        {
            cocos2d::Vec3 dir = _touchPos - _sprite->getPosition3D();
            dir.normalize();
            cocos2d::Vec3 dp = dir * _moveSpeed * dt;
            cocos2d::Vec3 endPos = _sprite->getPosition3D() + dp;
            if ((endPos - _touchPos).lengthSquared() <= dp.lengthSquared())
            {
				if (0.0f < (endPos - _touchPos).dot(dir))
				{
					endPos = _touchPos;
					_sprite->stopAction(_spriteAnim);
					_isRunningAction = false;
				}
            }
            _sprite->setPosition3D(endPos);

            float aspect = dir.dot(cocos2d::Vec3(0.0, 0.0, 1.0));
            aspect = acosf(aspect);
            if (dir.x < 0.0f) aspect = -aspect;
            _sprite->setRotation3D(cocos2d::Vec3(0.0f, RADIUS_TO_DEGREE(aspect) + _modelOffsetRot, 0.0f));
        }
    }

    updateCoins(dt);
    updateCamera();
}

bool Runner3DTestDemo::checkCollectedCoin(cocos2d::Node *coin)
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

cocos2d::Sprite3D* Runner3DTestDemo::CreateCoin(const cocos2d::Vec3 &pos)
{
    unsigned int coinID = probabilityArray[(rand() % PROBABILITY_ARRAY_SIZE)];
    auto coin = cocos2d::Sprite3D::create(modelPathList[coinID]);
    coin->setPosition3D(pos);
    coin->setPositionY(coinInitOffset);
    coin->setCameraMask(2);
    return coin;
}

void Runner3DTestDemo::updateCoins(float dt)
{
    const cocos2d::Vector<cocos2d::Node *> &childList = _coinLayer->getChildren();
    if (!childList.empty())
    {
        auto firtst = childList.at(0);
        if (checkCoinOutOfBound(firtst))
        {
            _coinLayer->removeChild(firtst);
            _coinLayer->addChild(CreateCoin(_newIconPos));
            _newIconPos += cocos2d::Vec3(MAX_COIN_SPACE, 0.0, 0.0);
        }


        for (ssize_t i = 0; i < childList.size(); ++i)
        {
            auto coin = childList.at(i);
            cocos2d::Vec3 rot = coin->getRotation3D();
            rot.y += dt * _coinRotSpeed;
            if (360.0f < rot.y) 
                rot.y -= 360.0f;
            coin->setRotation3D(rot);

            if (coin->isVisible() && checkCollectedCoin(coin))
            {
                coin->setVisible(false);
                updateScore();
            }
        }
    }
}

void Runner3DTestDemo::updateCamera()
{
    if (_camera)
    {
        _camera->lookAt(_sprite->getPosition3D(), cocos2d::Vec3(0.0f, 1.0f, 0.0f));
        _camera->setPositionX(_sprite->getPositionX());
        _camera->setPositionY(_sprite->getPositionY() + 50.0f);
        _camera->setPositionZ(_sprite->getPositionZ() + 100.0f);
       // _camera->setPosition3D(_sprite->getPosition3D() + cocos2d::Vec3(0.0f, 50.0f, 100.0f));
    }
}

bool Runner3DTestDemo::checkCoinOutOfBound( cocos2d::Node *coin )
{
    cocos2d::Vec3 spritePos = _sprite->getPosition3D();
    spritePos.y += coinInitOffset;
    float dis = _sprite->getPositionX() - coin->getPositionX();

    if (MIN_OUT_OF_BOUND_DIS <= dis) 
        return true;

    return false;
}

bool Runner3DTestDemo::onTouchBeganEvent( cocos2d::Touch *touch, cocos2d::Event *event )
{
    if (touch)
    {
        if (!_isRunningAction)
        {
            if (_sprite)
            {
                _sprite->runAction(_spriteAnim);
                _isRunningAction = true;
            }
        }
    }

    return true;
}

void Runner3DTestDemo::onTouchMovedEvent( cocos2d::Touch *touch, cocos2d::Event *event )
{
    if (touch)
    {
    }
}

void Runner3DTestDemo::onTouchEndedEvent( cocos2d::Touch *touch, cocos2d::Event *event )
{
    if (touch)
    {
        auto location = touch->getLocationInView();
        cocos2d::Vec3 nearP(location.x, location.y, -1.0f), farP(location.x, location.y, 1.0f);
        auto size = cocos2d::Director::getInstance()->getWinSize();
        _camera->unproject(size, &nearP, &nearP);
        _camera->unproject(size, &farP, &farP);
        cocos2d::Vec3 dir(farP - nearP);
        float dist=0.0f;
        float ndd = cocos2d::Vec3::dot(cocos2d::Vec3(0,1,0),dir);
        if(ndd == 0)
            dist=0.0f;
        float ndo = cocos2d::Vec3::dot(cocos2d::Vec3(0,1,0),nearP);
        dist= (0 - ndo) / ndd;
        _touchPos =   nearP + dist *  dir;
    }
}

bool Runner3DTestDemo::vec3equals( const cocos2d::Vec3 &lvec3, const cocos2d::Vec3 &rvec3 )
{
    return (fabs(lvec3.x - rvec3.x) < FLT_EPSILON)
        && (fabs(lvec3.y - rvec3.y) < FLT_EPSILON)
        && (fabs(lvec3.z - rvec3.z) < FLT_EPSILON);
}

void Runner3DTestScene::runThisTest()
{
    auto layer = nextRunner3DTestAction();
    addChild(layer);
    cocos2d::Director::getInstance()->replaceScene(this);
}