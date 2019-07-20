#include "scene_io.h"
#include "image_io.h"
#include "SceneGraph/scene1.h"
#include "SceneGraph/shape.h"
#include "SceneGraph/material.h"
#include "SceneGraph/light.h"
#include "SceneGraph/texture.h"
#include "math/mathutils.h"

#include <string>
#include <map>
#include <set>
#include <cassert>

#include "Utils/log.h"

namespace Baikal
{
    SceneIo* SceneIo::GetInstance()
    {
        static SceneIo instance;
        return &instance;
    }

    void SceneIo::RegisterLoader(const std::string& ext, SceneIo::Loader *loader)
    {
        GetInstance()->m_loaders[ext] = loader;
    }

    void SceneIo::UnregisterLoader(const std::string& ext)
    {
        GetInstance()->m_loaders.erase(ext);
    }

    Scene1::Ptr SceneIo::LoadScene(std::string const& filename, std::string const& basepath)
    {
        auto ext = filename.substr(filename.rfind(".") + 1);

        SceneIo *instance = GetInstance();
        auto loader_it = instance->m_loaders.find(ext);
        if (loader_it == instance->m_loaders.end())
        {
            throw std::runtime_error("No loader for \"" + filename + "\" has been found.");
        }

        return loader_it->second->LoadScene(filename, basepath);
    }

    void SceneIo::SaveScene(Scene1 const& scene, std::string const& filename, std::string const& basepath)
    {
        auto ext = filename.substr(filename.rfind("."));

        SceneIo *instance = GetInstance();
        auto loader_it = instance->m_loaders.find(ext);
        if (loader_it == instance->m_loaders.end())
        {
            throw std::runtime_error("No serializer for \"" + filename + "\" has been found.");
        }

        return loader_it->second->SaveScene(scene, filename, basepath);
    }

    Texture::Ptr SceneIo::Loader::LoadTexture(ImageIo const& io, Scene1& scene, std::string const& basepath, std::string const& name) const
    {
        std::string fname = basepath + name;
        std::replace(fname.begin(), fname.end(), '\\', '/');

        auto iter = m_texture_cache.find(name);

        if (iter != m_texture_cache.cend())
        {
            return iter->second;
        }
        else
        {
            try
            {
                LogInfo("Loading ", name, "\n");
                auto texture = io.LoadImage(fname);
                texture->SetName(name);
                m_texture_cache[name] = texture;
                return texture;
            }
            catch (std::runtime_error &)
            {
                LogInfo("Missing texture: ", name, "\n");
                return nullptr;
            }
        }
    }

    SceneIo::Loader::Loader(const std::string& ext, SceneIo::Loader *loader) :
        m_ext(ext)

    {
        SceneIo::RegisterLoader(m_ext, loader);
    }

    SceneIo::Loader::~Loader()
    {
        SceneIo::UnregisterLoader(m_ext);
    }

}
