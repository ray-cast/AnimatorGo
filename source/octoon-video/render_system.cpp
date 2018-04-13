#include <octoon/video/render_system.h>
#include <octoon/video/camera.h>
#include <octoon/video/geometry.h>
#include <octoon/video/render_scene.h>
#include <octoon/video/text_system.h>
#include <octoon/video/mesh.h>

#include <octoon/runtime/except.h>

#include <GL/GL.h>

namespace octoon
{
	namespace video
	{
		RenderSystem::RenderSystem() noexcept
			: width_(0)
			, height_(0)
		{
			TextSystem::instance()->setup();
		}

		RenderSystem::~RenderSystem() noexcept
		{
		}

		void
		RenderSystem::setWindowSize(std::uint32_t w, std::uint32_t h) noexcept
		{
			width_ = w;
			height_ = h;
		}

		void
		RenderSystem::getWindowSize(std::uint32_t& w, std::uint32_t& h) const noexcept
		{
			w = width_;
			h = height_;
		}

		void
		RenderSystem::render() noexcept
		{
			glViewport(0, 0, width_, height_);

			for (auto& camera : video::RenderScene::instance()->getCameraList())
			{
				auto& clearColor = camera->getClearColor();
				glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				glMatrixMode(GL_PROJECTION);
				glLoadMatrixf(camera->getViewProjection().ptr());

				glEnable(GL_DEPTH_TEST);

				for (auto& it : video::RenderScene::instance()->getRenderObjects())
				{
					auto geometry = it->downcast<video::Geometry>();
					auto mesh = geometry->getMesh();
					if (!mesh)
						continue;

					glMatrixMode(GL_MODELVIEW);
					glLoadMatrixf(geometry->getTransform().ptr());

					glEnableClientState(GL_COLOR_ARRAY);
					glEnableClientState(GL_VERTEX_ARRAY);

					glColorPointer(3, GL_FLOAT, 0, mesh->getNormalArray().data());
					glVertexPointer(3, GL_FLOAT, 0, mesh->getVertexArray().data());

					switch (geometry->getDrawType())
					{
					case video::DrawType::Points:
						glDrawArrays(GL_POINTS, 0, mesh->getVertexArray().size());
						break;
					case video::DrawType::Lines:
						glDrawArrays(GL_LINES, 0, mesh->getVertexArray().size());
						break;
					case video::DrawType::Triangles:
						glDrawArrays(GL_TRIANGLES, 0, mesh->getVertexArray().size());
						break;
					default:
						break;
					}
				}
			}
		}
	}
}