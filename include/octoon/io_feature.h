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
		void onActivate() except override;
		void onDeactivate() noexcept override;

		void onFrameBegin() noexcept override;
		void onFrame() noexcept override;
		void onFrameEnd() noexcept override;

	private:
		std::string diskPath_;
	};
}

#endif