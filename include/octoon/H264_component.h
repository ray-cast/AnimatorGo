#ifndef OCTOON_H264_COMPONENT_H_
#define OCTOON_H264_COMPONENT_H_

#include <octoon/game_component.h>
#include <octoon/io/iostream.h>

namespace octoon
{
	class OCTOON_EXPORT H264Component final : public GameComponent
	{
		OctoonDeclareSubClass(H264Component, GameComponent)
	public:
		H264Component() noexcept;
		H264Component(const GameObjectPtr& camera) noexcept;

		void setCamera(const GameObjectPtr& camera) noexcept;
		const GameObjectPtr& getCamera() const noexcept;

		void setTimeStep(float timeStep) noexcept;
		float getTimeStep() const noexcept;

		void capture(const std::shared_ptr<std::ofstream>& stream) noexcept;

		GameComponentPtr clone() const noexcept override;

	private:
		void onActivate() noexcept override;
		void onDeactivate() noexcept override;

		void onFixedUpdate() except override;

	private:
		void capture() noexcept;

		static void rgb2yuv(std::uint8_t* rgb, int w, int h, std::uint8_t* yuvBuf) noexcept;

	private:
		H264Component(const H264Component&) = delete;
		H264Component& operator=(const H264Component&) = delete;

	private:
		float time_;
		float timeStep_;

		std::size_t width_;
		std::size_t height_;

		std::unique_ptr<char[]> enc_;
		std::unique_ptr<char[]> scratch_;
		std::unique_ptr<std::uint8_t[]> buf_;

		GameObjectWeakPtr camera_;

		std::shared_ptr<std::ofstream> ostream_;
	};
}

#endif