#ifndef MYSTICLIT_MODEL_H_
#define MYSTICLIT_MODEL_H_

#include <octoon/runtime/json.h>

namespace MysticLit
{
	class MysticLitModule
	{
	public:
		MysticLitModule() noexcept;
		virtual ~MysticLitModule() noexcept;

		void setEnable(bool enable) noexcept;
		bool getEnable() const noexcept;

		virtual void reset() noexcept = 0;
		virtual void onValidate() noexcept;

		virtual void load(octoon::runtime::json& reader) noexcept = 0;
		virtual void save(octoon::runtime::json& reader) noexcept = 0;

	private:
		MysticLitModule(const MysticLitModule&) = delete;
		MysticLitModule& operator=(const MysticLitModule&) = delete;

	private:
		bool enable_;
	};
}

#endif