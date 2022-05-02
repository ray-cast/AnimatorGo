#ifndef RABBIT_MODEL_H_
#define RABBIT_MODEL_H_

#include <octoon/runtime/json.h>

namespace rabbit
{
	class RabbitModule
	{
	public:
		RabbitModule() noexcept;
		virtual ~RabbitModule() noexcept;

		void setEnable(bool enable) noexcept;
		bool getEnable() const noexcept;

		virtual void reset() noexcept = 0;
		virtual void onValidate() noexcept;

		virtual void load(octoon::runtime::json& reader) noexcept(false) = 0;
		virtual void save(octoon::runtime::json& reader) noexcept(false) = 0;

	private:
		RabbitModule(const RabbitModule&) = delete;
		RabbitModule& operator=(const RabbitModule&) = delete;

	private:
		bool enable_;
	};
}

#endif