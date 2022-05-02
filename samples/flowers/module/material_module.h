#ifndef FLOWER_MATERIAL_MODULE_H_
#define FLOWER_MATERIAL_MODULE_H_

#include <flower_model.h>

namespace flower
{
	class MaterialModule final : public FlowerModule
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
		std::string path;
	};
}

#endif