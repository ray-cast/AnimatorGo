#ifndef OCTOON_IO_FEATURE_H_
#define OCTOON_IO_FEATURE_H_

#include <octoon/game_feature.h>

namespace octoon
{
	class OCTOON_EXPORT IOFeature final : public GameFeature
	{
		OctoonDeclareSubClass(IOFeature, GameFeature)
	public:
		IOFeature() noexcept;
		~IOFeature() noexcept;

	private:
		void on_activate() except override;
		void on_deactivate() noexcept override;

		void on_frame_begin() noexcept override;
		void on_frame() noexcept override;
		void on_frame_end() noexcept override;

	private:
		std::string system_path_;
	};
}

#endif