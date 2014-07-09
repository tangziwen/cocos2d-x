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

#ifndef __CC_AABB_H__
#define __CC_AABB_H__

#include "base/CCRef.h"
#include "base/CCPlatformMacros.h"
#include "base/CCConsole.h"

NS_CC_BEGIN

/**
 * Defines a class describe 3-dimensional axis-aligned bounding box.
 */
class AABB
{
public:

    Vec3 _min;

    Vec3 _max;

	/**
     * Constructor.
     */
    AABB();

	/**
     * Constructor.
     */
    AABB(const Vec3& min, const Vec3& max);

	/**
     * Constructor.
     */
	AABB(const AABB& box);

	/**
     * Destructor.
     */
    ~AABB();

	/**
     * Gets the center point of the bounding box.
	 */
    Vec3 getCenter();

	Vec3 getSize();

	float getLength();

	 /**
     * Gets the corners of the bounding box in the specified array.
     */
	void getCorners(Vec3* dst) const;

	 /**
     * Tests whether this bounding box intersects the specified bounding object.
     */
    bool intersects(const AABB& box) const;

	bool containPoint( const Vec3& point) const;
	bool containSphere( const Vec3 &center,float radius ) const;

	float distance( const Vec3& vPoint);

	 /**
     * Sets this bounding box to the smallest bounding box
     * that contains both this bounding object and the specified bounding box.
     */
    void merge(const AABB& box);

	 /**
     * Sets this bounding box to the specified values.
     */
    void set(const Vec3& min, const Vec3& max);

	void reset();

	/**
     * Transforms the bounding box by the given transformation matrix.
     */
    void transform(const C3DMatrix& matrix);
};

NS_CC_END

#endif
