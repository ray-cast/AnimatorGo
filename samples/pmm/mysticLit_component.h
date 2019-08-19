#ifndef OCTOON_MYSTICLIT_COMPONENT_H_
#define OCTOON_MYSTICLIT_COMPONENT_H_

#include <memory>
#include <string>
#include <mysticLit_model.h>
#include <mysticLit_context.h>

namespace MysticLit
{
	class IMysticLitComponent
	{
	public:
		virtual const std::type_info& type_info() const noexcept = 0;

		virtual void setActive(bool active) noexcept = 0;
		virtual bool getActive() const noexcept = 0;

		virtual void onEnable() noexcept {}
		virtual void onDisable() noexcept {}

		virtual std::shared_ptr<MysticLitModule> model() const noexcept = 0;
	};

	template<typename T>
	class MysticLitComponent : public IMysticLitComponent
	{
	public:
		MysticLitComponent() noexcept = default;
		virtual ~MysticLitComponent() noexcept = default;

		virtual const std::type_info& type_info() const noexcept
		{
			return typeid(T);
		}

		virtual void setContext(const std::shared_ptr<MysticLitContext>& context) noexcept
		{
			context_ = context;
		}

		virtual std::shared_ptr<MysticLitContext> getContext() const noexcept
		{
			return context_;
		}

		virtual void setModel(const std::shared_ptr<T>& model) noexcept
		{
			model_ = model;
		}

		virtual std::shared_ptr<T> getModel() const noexcept
		{
			return model_;
		}

		virtual std::shared_ptr<MysticLitModule> model() const noexcept
		{
			return model_;
		}

		virtual void init(const std::shared_ptr<MysticLitContext>& context, const std::shared_ptr<T>& model) noexcept
		{
			this->setModel(model);
			this->setContext(context);
		}

		template<typename T>
		IMysticLitComponent* getComponent() const noexcept
		{
			return context_.behaviour->getComponent<T>();
		}

		IMysticLitComponent* getComponent(const std::type_info& type) const noexcept
		{
			return context_.behaviour->getComponent(type);
		}
	
	private:
		std::shared_ptr<T> model_;
		std::shared_ptr<MysticLitContext> context_;
	};
}

#endif