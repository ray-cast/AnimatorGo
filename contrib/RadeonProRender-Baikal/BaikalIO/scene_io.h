/**********************************************************************
 Copyright (c) 2016 Advanced Micro Devices, Inc. All rights reserved.
 
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
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ********************************************************************/

/**
 \file scene_io.h
 \author Dmitry Kozlov
 \version 1.0
 \brief Contains an interface for scene loading
 */
#pragma once

#include <string>
#include <memory>
#include <map>
#include "SceneGraph/texture.h"
#include "SceneGraph/scene1.h"

#ifdef WIN32
#ifdef BAIKAL_EXPORT_API
#define BAIKAL_API_ENTRY __declspec(dllexport)
#else
#define BAIKAL_API_ENTRY __declspec(dllimport)
#endif
#else
#define BAIKAL_API_ENTRY __attribute__((visibility ("default")))
#endif


namespace Baikal
{
    class Scene1;
    class Texture;
    class ImageIo;
    
    /**
     \brief Interface for scene loading
     
     SceneIO implementation is responsible for translation of various scene formats into Baikal.
     */
    class SceneIo
    {
    public:
        /**
        \brief Interface for file format handler

        SceneIo::Loader is responsible for translation of various scene formats into Baikal.
        */
        class Loader
        {
        public:
            // Load the scene from file using resourse base path
            virtual Scene1::Ptr LoadScene(const std::string &filename, const std::string &basepath) const = 0;

            virtual void SaveScene(const Scene1 &scene, const std::string &filename, const std::string &basepath) const {};

            Loader(const std::string& ext, SceneIo::Loader *loader);
            virtual ~Loader();

        protected:
            Texture::Ptr LoadTexture(ImageIo const& io, Scene1& scene, std::string const& basepath, std::string const& name) const;

        private:
            Loader(const Loader &) = delete;
            Loader& operator= (const Loader &) = delete;

            std::string m_ext;
            mutable std::map<std::string, Texture::Ptr> m_texture_cache;
        };

        // Registers extension handler
        static void BAIKAL_API_ENTRY RegisterLoader(const std::string& ext, SceneIo::Loader *loader);
        // Deregisters extension handler
        static void BAIKAL_API_ENTRY UnregisterLoader(const std::string& ext);
      
        // Load the scene from file using resourse base path
        static Scene1::Ptr BAIKAL_API_ENTRY LoadScene(std::string const& filename, std::string const& basepath);
        // Saves scene to file using resource base path
        static void BAIKAL_API_ENTRY SaveScene(Scene1 const& scene, std::string const& filename, std::string const& basepath);


    private:
        static SceneIo* GetInstance();
        
        // Constructor
        SceneIo() = default;
        // Destructor
        virtual ~SceneIo() = default;

        // Disallow copying
        SceneIo(SceneIo const&) = delete;
        SceneIo& operator = (SceneIo const&) = delete;

        std::map<std::string, SceneIo::Loader*> m_loaders;
    };
}
