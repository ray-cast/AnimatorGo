#ifndef OCTOON_MYSTICLIT_MODEL_H_
#define OCTOON_MYSTICLIT_MODEL_H_

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

	private:
		MysticLitModule(const MysticLitModule&) = delete;
		MysticLitModule& operator=(const MysticLitModule&) = delete;

	private:
		bool enable_;
	};
}

#endif