#include <octoon/h264_component.h>
#include <octoon/offline_camera_component.h>
#include <octoon/timer_feature.h>
#include <octoon/game_scene.h>

#include <fstream>
#define MINIH264_IMPLEMENTATION
#include <minih264e.h>

namespace octoon
{
	OctoonImplementSubClass(H264Component, GameComponent, "H264")

	H264Component::H264Component() noexcept
		: time_(0.0f)
		, timeStep_(1000.0f / 24.0f)
	{
	}

	H264Component::H264Component(const GameObjectPtr& camera) noexcept
		: H264Component()
	{
		camera_ = camera;
	}

	void
	H264Component::setTimeStep(float timeStep) noexcept
	{
		timeStep_ = timeStep;
	}

	float
	H264Component::getTimeStep() const noexcept
	{
		return timeStep_;
	}

	void
	H264Component::setCamera(const GameObjectPtr& camera) noexcept
	{
		camera_  = camera;
	}
	
	const GameObjectPtr&
	H264Component::getCamera() const noexcept
	{
		return camera_.lock();
	}

	GameComponentPtr
	H264Component::clone() const noexcept
	{
		auto instance = std::make_shared<H264Component>();
		instance->setName(this->getName());

		return instance;
	}

	void
	H264Component::onActivate() noexcept
	{
	}

	void
	H264Component::onDeactivate() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::FrameBegin);
	}

	void
	H264Component::capture(const std::shared_ptr<std::ofstream>& stream) noexcept
	{
		if (!camera_.lock()) return;

		auto camera = camera_.lock();
		if (camera)
		{
			auto framebuffer = this->camera_.lock()->getComponent<OfflineCameraComponent>()->getFramebuffer();

			H264E_create_param_t create_param;
			std::memset(&create_param, 0, sizeof(create_param));
			create_param.enableNEON = 1;
			create_param.num_layers = 1;
			create_param.inter_layer_pred_flag = 1;
			create_param.inter_layer_pred_flag = 0;
			create_param.gop = 120;
			create_param.width = framebuffer->getGraphicsFramebufferDesc().getWidth();
			create_param.height = framebuffer->getGraphicsFramebufferDesc().getHeight();
			create_param.max_long_term_reference_frames = 0;
			create_param.fine_rate_control_flag = 0;
			create_param.const_input_flag = 0;
			create_param.vbv_size_bytes = 100000 / 8;
			create_param.temporal_denoise_flag = false;
			create_param.max_threads = 0;

			int sizeof_persist = 0, sizeof_scratch = 0;
			auto error = H264E_sizeof(&create_param, &sizeof_persist, &sizeof_scratch);

			this->enc_ = std::make_unique<char[]>(sizeof_persist);
			this->scratch_ = std::make_unique<char[]>(sizeof_scratch);
			this->buf_ = std::make_unique<unsigned char[]>(create_param.width * create_param.height * 3 / 2);

			error = H264E_init((H264E_persist_t*)this->enc_.get(), &create_param);

			this->width_ = create_param.width;
			this->height_ = create_param.height;
			this->ostream_ = stream;

			this->addComponentDispatch(GameDispatchType::FrameBegin);
		}
	}

	bool rgb2yuv(unsigned char* rgb, int w, int h, unsigned char* yuvBuf)
	{
		unsigned char* ptrY, * ptrU, * ptrV;
		memset(yuvBuf, 0, w * h * 3 / 2);
		ptrY = yuvBuf;
		ptrU = yuvBuf + w * h;
		ptrV = ptrU + (w * h * 1 / 4);
		unsigned char y, u, v, r, g, b;

		for (int j = 0; j < h;j++)
		{
			for (int i = 0;i < w;i++)
			{
				auto data = rgb + ((h - j - 1) * w + i) * 4;
				r = data[0];
				g = data[1];
				b = data[2];

				y = (unsigned char)((66 * r + 129 * g + 25 * b + 128) >> 8) + 16;
				u = (unsigned char)((-38 * r - 74 * g + 112 * b + 128) >> 8) + 128;
				v = (unsigned char)((112 * r - 94 * g - 18 * b + 128) >> 8) + 128;

				*(ptrY++) = std::clamp<unsigned char>(y, 0, 255);

				if (j % 2 == 0 && i % 2 == 0)
				{
					*(ptrU++) = std::clamp<unsigned char>(u, 0, 255);
				}
				else
				{
					if (i % 2 == 0)
					{
						*(ptrV++) = std::clamp<unsigned char>(v, 0, 255);
					}
				}
			}
		}
		return true;
	}

	void
	H264Component::onFrameBegin() except
	{
		auto feature = this->getGameObject()->getGameScene()->getFeature<TimerFeature>();
		if (feature)
		{
			time_ += feature->delta() * CLOCKS_PER_SEC;

			if (time_ > timeStep_)
			{
				this->capture();
				this->sendMessageDownwards("octoon:H264:capture");

				time_ -= timeStep_;
			}
		}
	}

	void
	H264Component::capture() noexcept
	{
		H264E_io_yuv_t yuv;
		yuv.yuv[0] = (unsigned char*)this->buf_.get(); yuv.stride[0] = this->width_;
		yuv.yuv[1] = (unsigned char*)this->buf_.get() + this->width_ * this->height_; yuv.stride[1] = this->width_ / 2;
		yuv.yuv[2] = (unsigned char*)this->buf_.get() + this->width_ * this->height_ * 5 / 4; yuv.stride[2] = this->width_ / 2;

		H264E_run_param_t run_param;
		std::memset(&run_param, 0, sizeof(run_param));
		run_param.frame_type = 0;
		run_param.encode_speed = 24.0;
		run_param.qp_min = run_param.qp_max = 33;

		auto camera = camera_.lock();
		if (camera)
		{
			auto framebuffer = this->camera_.lock()->getComponent<OfflineCameraComponent>()->getFramebuffer();
			auto colorTexture = framebuffer->getGraphicsFramebufferDesc().getColorAttachments().front().getBindingTexture();

			void* data;
			if (colorTexture->map(0, 0, this->width_, this->height_, 0, &data))
			{
				rgb2yuv((unsigned char*)data, this->width_, this->height_, this->buf_.get());
				colorTexture->unmap();
			}
		}

		int sizeof_coded_data;
		std::uint8_t* coded_data;
		if (H264E_STATUS_SUCCESS == H264E_encode((H264E_persist_t*)this->enc_.get(), (H264E_scratch_t*)this->scratch_.get(), &run_param, &yuv, &coded_data, &sizeof_coded_data))
			ostream_->write((char*)coded_data, sizeof_coded_data);
	}
}