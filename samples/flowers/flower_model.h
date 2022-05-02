#ifndef FLOWER_MODEL_H_
#define FLOWER_MODEL_H_

#include <octoon/runtime/json.h>

namespace flower
{
	class FlowerModule
	{
	public:
		FlowerModule() noexcept;
		virtual ~FlowerModule() noexcept;

		void setEnable(bool enable) noexcept;
		bool getEnable() const noexcept;

		virtual void reset() noexcept = 0;
		virtual void onValidate() noexcept;

		virtual void load(octoon::runtime::json& reader) noexcept(false) = 0;
		virtual void save(octoon::runtime::json& reader) noexcept(false) = 0;

	private:
		FlowerModule(const FlowerModule&) = delete;
		FlowerModule& operator=(const FlowerModule&) = delete;

	private:
		bool enable_;
	};
}

#endif