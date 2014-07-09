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

#include "3d/CCSprite3D.h"
#include "3d/CCMesh.h"
#include "3d/CCObjLoader.h"
#include "3d/CCMeshSkin.h"
#include "3d/CCBundle3D.h"
#include "3d/CCSprite3DMaterial.h"
#include "3d/CCSubMesh.h"

#include "base/CCDirector.h"
#include "base/CCPlatformMacros.h"
#include "base/ccMacros.h"
#include "platform/CCFileUtils.h"
#include "renderer/CCTextureCache.h"
#include "renderer/CCRenderer.h"
#include "renderer/CCGLProgramState.h"
#include "renderer/CCGLProgramCache.h"

#include "deprecated/CCString.h" // For StringUtils::format

NS_CC_BEGIN

std::string s_attributeNames[] = {GLProgram::ATTRIBUTE_NAME_POSITION, GLProgram::ATTRIBUTE_NAME_COLOR, GLProgram::ATTRIBUTE_NAME_TEX_COORD, GLProgram::ATTRIBUTE_NAME_NORMAL, GLProgram::ATTRIBUTE_NAME_BLEND_WEIGHT, GLProgram::ATTRIBUTE_NAME_BLEND_INDEX};

Sprite3D* Sprite3D::create(const std::string &modelPath)
{
    if (modelPath.length() < 4)
        CCASSERT(false, "improper name specified when creating Sprite3D");
    
    auto sprite = new Sprite3D();
    if (sprite && sprite->initWithFile(modelPath))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

Sprite3D* Sprite3D::create(const std::string &modelPath, const std::string &texturePath)
{
    auto sprite = create(modelPath);
    if (sprite)
    {
        sprite->setTexture(texturePath);
    }
    
    return sprite;
}

//.mtl file should at the same directory with the same name if exist
bool Sprite3D::loadFromObj(const std::string& path)
{
    std::string fullPath = FileUtils::getInstance()->fullPathForFilename(path);
    
    //find from the cache
    std::string key = fullPath + "#";
    auto mesh = MeshCache::getInstance()->getMesh(key);
    if (mesh)
    {
        _mesh = mesh;
        _mesh->retain();
        
        auto tex = Sprite3DMaterialCache::getInstance()->getSprite3DMaterial(key);
        setTexture(tex);
        
        genGLProgramState();
        
        return true;
    }
    
    //.mtl file directory
    std::string dir = "";
    auto last = fullPath.rfind("/");
    if (last != -1)
        dir = fullPath.substr(0, last + 1);
    
    ObjLoader::shapes_t shapes;
    std::string errstr = ObjLoader::LoadObj(shapes, fullPath.c_str(), dir.c_str());
    if (!errstr.empty())
        return false;
    
    //convert to mesh and material
    std::vector<std::vector<unsigned short> > submeshIndices;
    std::vector<std::string> matnames;
    std::string texname;
    for (auto it = shapes.shapes.begin(); it != shapes.shapes.end(); it++)
    {
        submeshIndices.push_back((*it).mesh.indices);
        
        texname = (*it).material.diffuse_texname;
        if (!texname.empty())
            texname = dir + (*it).material.diffuse_texname;
        matnames.push_back(texname);
    }
    _mesh = Mesh::create(shapes.positions, shapes.normals, shapes.texcoords, submeshIndices);
    
    _mesh->retain();
    if (_mesh == nullptr)
        return false;
    
    if (matnames.size())
    {
        setTexture(matnames[0]);
    }
    genGLProgramState();
    
    //add to cache
    
    if (_texture)
    {
        Sprite3DMaterialCache::getInstance()->addSprite3DMaterial(key, _texture);
    }
    
    MeshCache::getInstance()->addMesh(key, _mesh);

    return true;
}

bool Sprite3D::loadFromC3x(const std::string& path)
{
    std::string fullPath = FileUtils::getInstance()->fullPathForFilename(path);
    //find from the cache
    std::string key = fullPath + "#";
    auto mesh = MeshCache::getInstance()->getMesh(key);
    if (mesh)
    {
        _mesh = mesh;
        _mesh->retain();
        
        auto tex = Sprite3DMaterialCache::getInstance()->getSprite3DMaterial(key);
        setTexture(tex);
        
        _skin = MeshSkin::create(fullPath, "");
        CC_SAFE_RETAIN(_skin);
        
        genGLProgramState();
        
        return true;
    }
    
    //load from .c3b or .c3t
    auto bundle = Bundle3D::getInstance();
    if (!bundle->load(fullPath))
        return false;
    
    MeshData meshdata;
    bool ret = bundle->loadMeshData("", &meshdata);
    if (!ret)
    {
        return false;
    }
    
    _mesh = Mesh::create(meshdata.vertex, meshdata.vertexSizeInFloat, meshdata.subMeshIndices, meshdata.numIndex, meshdata.attribs, meshdata.attribCount);
    CC_SAFE_RETAIN(_mesh);
    
    _skin = MeshSkin::create(fullPath, "");
    CC_SAFE_RETAIN(_skin);
    
    MaterialData materialdata;
    ret = bundle->loadMaterialData("", &materialdata);
    if (ret)
    {
        setTexture(materialdata.texturePath);
    }
    
    genGLProgramState();
    
    //add to cache
    auto cache = Director::getInstance()->getTextureCache();
    auto tex = cache->addImage(materialdata.texturePath);
    if (tex)
        Sprite3DMaterialCache::getInstance()->addSprite3DMaterial(key, tex);
    
    MeshCache::getInstance()->addMesh(key, _mesh);
    
    return true;
}

Sprite3D::Sprite3D()
: _mesh(nullptr)
, _skin(nullptr)
, _texture(nullptr)
, _blend(BlendFunc::ALPHA_NON_PREMULTIPLIED)
{
}

Sprite3D::~Sprite3D()
{
    CC_SAFE_RELEASE_NULL(_texture);
    CC_SAFE_RELEASE_NULL(_mesh);
    CC_SAFE_RELEASE_NULL(_skin);
}

bool Sprite3D::initWithFile(const std::string &path)
{
    CC_SAFE_RELEASE_NULL(_mesh);
    CC_SAFE_RELEASE_NULL(_skin);
    CC_SAFE_RELEASE_NULL(_texture);
    
    //load from file
    std::string ext = path.substr(path.length() - 4, 4);
    std::transform(ext.begin(), ext.end(), ext.begin(), tolower);
    
    if (ext == ".obj")
    {
        return loadFromObj(path);
    }
    else if (ext == ".c3b" || ext == ".c3t")
    {
        return loadFromC3x(path);
    }
    
    return false;
}

void Sprite3D::genGLProgramState()
{
    auto programstate = GLProgramState::getOrCreateWithGLProgram(getDefaultGLProgram(_mesh->hasVertexAttrib(GLProgram::VERTEX_ATTRIB_TEX_COORD)));
    long offset = 0;
    auto attributeCount = _mesh->getMeshVertexAttribCount();
    for (auto k = 0; k < attributeCount; k++) {
        auto meshattribute = _mesh->getMeshVertexAttribute(k);
        programstate->setVertexAttribPointer(s_attributeNames[meshattribute.vertexAttrib],
                                             meshattribute.size,
                                             meshattribute.type,
                                             GL_FALSE,
                                             _mesh->getVertexSizeInBytes(),
                                             (GLvoid*)offset);
        offset += meshattribute.attribSizeBytes;
    }
    
    setGLProgramState(programstate);
    auto count = _mesh->getSubMeshCount();
    _meshCommands.resize(count);
    for (int i = 0; i < count; i++) {
        auto submesh = _mesh->getSubMesh(i);

        GLuint texID = _texture ? _texture->getName() : 0;
        _meshCommands[i].genMaterialID(texID, programstate, submesh, _blend);
    }
    
}

GLProgram* Sprite3D::getDefaultGLProgram(bool textured)
{
    bool hasSkin = _skin && _mesh->hasVertexAttrib(GLProgram::VERTEX_ATTRIB_BLEND_INDEX)
    && _mesh->hasVertexAttrib(GLProgram::VERTEX_ATTRIB_BLEND_WEIGHT);
    
    if(textured)
    {
        if (hasSkin)
            return GLProgramCache::getInstance()->getGLProgram(GLProgram::SHADER_3D_SKINPOSITION_TEXTURE);
        
        return GLProgramCache::getInstance()->getGLProgram(GLProgram::SHADER_3D_POSITION_TEXTURE);
    }
    else
    {
        return GLProgramCache::getInstance()->getGLProgram(GLProgram::SHADER_3D_POSITION);
    }
}

void Sprite3D::setTexture(const std::string& texFile)
{
    auto tex = Director::getInstance()->getTextureCache()->addImage(texFile);
//    if( tex && _texture != tex ) {
//        CC_SAFE_RETAIN(tex);
//        CC_SAFE_RELEASE_NULL(_texture);
//        _texture = tex;
//    }
    setTexture(tex);
}

void Sprite3D::setTexture(Texture2D* texture)
{
    if(_texture != texture) {
        CC_SAFE_RETAIN(texture);
        CC_SAFE_RELEASE_NULL(_texture);
        _texture = texture;
        if (getGLProgramState() && _mesh)
        {
            GLuint texID = _texture ? _texture->getName() : 0;
            _meshCommands[0].genMaterialID(texID, getGLProgramState(), _mesh, _blend);
        }
    }
}

void Sprite3D::setSubMeshVisible(int index, bool visible)
{
    CCASSERT(index < _subMeshVisible.size(), "invalid parameter");
    if (index < (int)_subMeshVisible.size())
         _subMeshVisible[index] = visible;
}

bool Sprite3D::getSubMeshVisible(int index) const
{
    CCASSERT(index < _subMeshVisible.size(), "invalid parameter");
    if (index < (int)_subMeshVisible.size())
        return _subMeshVisible[index];
    
    return false;
}

ssize_t Sprite3D::getSubMeshCount() const
{
    return _mesh->getSubMeshCount();
}

void Sprite3D::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags)
{
    GLProgramState* programstate = getGLProgramState();
    Color4F color(getDisplayedColor());
    color.a = getDisplayedOpacity() / 255.0f;
    
    for (ssize_t i = 0; i < _mesh->getSubMeshCount(); i++) {
        auto submesh = _mesh->getSubMesh((int)i);
        auto& meshCommand = _meshCommands[i];
        GLuint textureID = _texture ? _texture->getName() : 0;
        meshCommand.init(_globalZOrder, textureID, programstate, _blend, _mesh->getVertexBuffer(), submesh->getIndexBuffer(), (GLenum)submesh->getPrimitiveType(), (GLenum)submesh->getIndexFormat(), submesh->getIndexCount(), transform);
        
        meshCommand.setCullFaceEnabled(true);
        meshCommand.setDepthTestEnabled(true);
        if (_skin)
        {
            meshCommand.setMatrixPaletteSize((int)_skin->getMatrixPaletteSize());
            meshCommand.setMatrixPalette(_skin->getMatrixPalette());
        }
        //support tint and fade
        meshCommand.setDisplayColor(Vec4(color.r, color.g, color.b, color.a));
        Director::getInstance()->getRenderer()->addCommand(&meshCommand);
    }
}

void Sprite3D::setBlendFunc(const BlendFunc &blendFunc)
{
    if(_blend.src != blendFunc.src || _blend.dst != blendFunc.dst)
    {
        _blend = blendFunc;
    }
}

const BlendFunc& Sprite3D::getBlendFunc() const
{
    return _blend;
}

NS_CC_END
