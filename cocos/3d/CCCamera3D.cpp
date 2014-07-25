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
#include "3d/CCCamera3D.h"
#include "base/CCDirector.h"
NS_CC_BEGIN
Camera3D* Camera3D::_activeCamera = nullptr;

Camera3D* Camera3D::createPerspective(float fieldOfView, float aspectRatio, float nearPlane, float farPlane)
{
	auto ret = new Camera3D();
	if (ret)
	{
        ret->_fieldOfView = fieldOfView;
        ret->_aspectRatio = aspectRatio;
        ret->_nearPlane = nearPlane;
        ret->_farPlane = farPlane;
        Mat4::createPerspective(ret->_fieldOfView, ret->_aspectRatio, ret->_nearPlane, ret->_farPlane, &ret->_projection);
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}
Camera3D*  Camera3D::getActiveCamera()
{
	return _activeCamera;
}
Camera3D* Camera3D::createOrthographic(float zoomX, float zoomY, float nearPlane, float farPlane)
{
	auto ret = new Camera3D();
	if (ret)
	{
        ret->_zoom[0] = zoomX;
        ret->_zoom[1] = zoomY;
        ret->_nearPlane = nearPlane;
        ret->_farPlane = farPlane;
        Mat4::createOrthographic(ret->_zoom[0], ret->_zoom[1], ret->_nearPlane, ret->_farPlane, &ret->_projection);
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}
Camera3D::Type Camera3D::getCameraType() const
{
	return _type;
}

Camera3D::Camera3D()
{
    
}

Camera3D::~Camera3D()
{
    if (_activeCamera == this)
    {
        _activeCamera = nullptr;
        CCLOG("release active camera");
    }
}

void Camera3D::setPosition3D(const Vec3& position)
{
	Node::setPosition3D(position);
	_transformUpdated = _transformDirty = _inverseDirty = true;
}
//set active camera
void Camera3D::setActiveCamera(Camera3D* camera)
{
    if (_activeCamera != camera)
    {
        CC_SAFE_RETAIN(camera);
        CC_SAFE_RELEASE(_activeCamera);
        _activeCamera = camera;
    }
}

void Camera3D::lookAt(const Vec3& position, const Vec3& up, const Vec3& target)
{
	//_center = center;
	//Mat4 matRotate;
 //   Vec3 upv = up;
 //   upv.normalize();

 //   Vec3 zaxis;
 //   Vec3::subtract(position, target, &zaxis);
 //   zaxis.normalize();

 //   Vec3 xaxis;
 //   Vec3::cross(upv, zaxis, &xaxis);
 //   xaxis.normalize();

 //   Vec3 yaxis;
 //   Vec3::cross(zaxis, xaxis, &yaxis);
 //   yaxis.normalize();

 //   matRotate.m[0] = xaxis.x;
 //   matRotate.m[1] = xaxis.y;
 //   matRotate.m[2] = xaxis.z;
 //   matRotate.m[3] = 0;

 //   matRotate.m[4] = yaxis.x;
 //   matRotate.m[5] = yaxis.y;
 //   matRotate.m[6] = yaxis.z;
 //   matRotate.m[7] = 0;

 //   matRotate.m[8] = zaxis.x;
 //   matRotate.m[9] = zaxis.y;
 //   matRotate.m[10] = zaxis.z;
 //   matRotate.m[11] = 0;
	//Quaternion rotationQuat;
 //   Quaternion::createFromRotationMatrix(matRotate, &rotationQuat);
 //   _rotation.set(rotationQuat);
	Node::setPosition3D(position);
	_transformUpdated = _transformDirty = _inverseDirty = true;
}
Mat4& Camera3D::getProjectionMatrix() 
{
	return _projection;
}
Mat4& Camera3D::getViewMatrix()
{
    //FIX ME
	_view=getNodeToWorldTransform().getInversed();
	return _view;
}

const Mat4& Camera3D::getViewProjectionMatrix()
{
    //FIX ME
    getViewMatrix();
    Mat4::multiply(_view, _projection, &_viewProjection);
    return _viewProjection;
}

void Camera3D::setAdditionalProjection(const Mat4& mat)
{
    _projection = mat * _projection;
    getViewProjectionMatrix();
}

void Camera3D::applyProjection()
{
	getProjectionMatrix();
	getViewMatrix();
	Director* director = Director::getInstance();
	director->loadIdentityMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
	director->loadIdentityMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
#if CC_TARGET_PLATFORM == CC_PLATFORM_WP8
	//if needed, we need to add a rotation for Landscape orientations on Windows Phone 8 since it is always in Portrait Mode
	GLView* view = getOpenGLView();
	if(getOpenGLView() != nullptr)
	{
		multiplyMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION, getOpenGLView()->getOrientationMatrix());
	}
#endif
	// issue #1334
	//Mat4::createPerspective(60, (GLfloat)size.width/size.height, 10, 1000, &matrixPerspective);
	director->multiplyMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION, _projection);
	director->multiplyMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION, _view);
	director->loadIdentityMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	GL::setProjectionMatrixDirty();
}
NS_CC_END