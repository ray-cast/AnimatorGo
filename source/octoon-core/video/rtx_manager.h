#ifndef OCTOON_VIDEO_CONFIG_MANAGER_H_
#define OCTOON_VIDEO_CONFIG_MANAGER_H_

#include <vector>
#include <memory>

#include <CLW.h>
#include <RadeonProRender.h>

#include <GL/glew.h>

#include "clw_output.h"
#include "clw_render_factory.h"

namespace octoon::video
{
    enum DeviceType
    {
        kPrimary,
        kSecondary
    };

    enum Mode
    {
        kUseAll,
        kUseGpus,
        kUseSingleGpu,
        kUseSingleCpu,
        kUseCpus
    };

    class RtxManager
    {
    public:
        RtxManager();

    private:
        void CreateFrameBufferFromGLTexture(GLenum target, GLint miplevel, GLuint texture);

    private:
        struct Config
        {
            DeviceType type;
            //std::unique_ptr<octoon::Renderer> renderer;
            //std::unique_ptr<octoon::SceneController<Baikal::ClwScene>> controller;
            std::unique_ptr<octoon::video::RenderFactory> factory;
            CLWContext context;
            bool caninterop;
        };

        std::vector<Config> configs_;
    };
}

#endif
