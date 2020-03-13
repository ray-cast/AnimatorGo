#ifndef RABBIT_MATERIAL_MODULE_H_
#define RABBIT_MATERIAL_MODULE_H_

#include <rabbit_model.h>

namespace rabbit
{
	class MaterialModule final : public RabbitModule
	{
	public:
		MaterialModule() noexcept;
		virtual ~MaterialModule() noexcept;

		virtual void reset() noexcept override;

		virtual void load(octoon::runtime::json& reader) noexcept override;
		virtual void save(octoon::runtime::json& reader) noexcept override;

	private:
		MaterialModule(const MaterialModule&) = delete;
		MaterialModule& operator=(const MaterialModule&) = delete;

	public:
		bool offlineEnable;
	};
}

#endif