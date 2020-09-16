#ifndef RABBIT_COMPONENT_H_
#define RABBIT_COMPONENT_H_

#include <rabbit_model.h>
#include <rabbit_context.h>
#include <any>

namespace rabbit
{
	class IRabbitComponent
	{
	public:
		virtual const std::type_info& type_info() const noexcept = 0;

		virtual void setActive(bool active) noexcept = 0;
		virtual bool getActive() const noexcept = 0;

		virtual std::shared_ptr<RabbitModule> model() const noexcept = 0;

		virtual void onEnable() noexcept(false);
		virtual void onDisable() noexcept(false);

		virtual void onFixedUpdate() noexcept(false);
		virtual void onUpdate() noexcept(false);
		virtual void onLateUpdate() noexcept(false);

		virtual void onPostProcess() noexcept(false);
	};

	template<typename T>
	class RabbitComponent : public IRabbitComponent
	{
	public:
		RabbitComponent() noexcept = default;
		virtual ~RabbitComponent() noexcept = default;

		virtual void setActive(bool active) noexcept override
		{
		}

		virtual bool getActive() const noexcept
		{
			return true;
		}

		virtual void setContext(const std::shared_ptr<RabbitContext>& context) noexcept
		{
			context_ = context;
		}

		virtual const std::shared_ptr<RabbitContext>& getContext() const noexcept
		{
			return context_;
		}

		virtual void setModel(const std::shared_ptr<T>& model) noexcept
		{
			model_ = model;
		}

		virtual const std::shared_ptr<T>& getModel() const noexcept
		{
			return model_;
		}

		virtual std::shared_ptr<RabbitModule> model() const noexcept
		{
			return model_;
		}

		virtual void init(const std::shared_ptr<RabbitContext>& context, const std::shared_ptr<T>& model) noexcept
		{
			this->setModel(model);
			this->setContext(context);
		}

		template<typename T>
		IRabbitComponent* getComponent() const noexcept
		{
			return context_->behaviour->getComponent<T>();
		}

		IRabbitComponent* getComponent(const std::type_info& type) const noexcept
		{
			return context_->behaviour->getComponent(type);
		}

		template<typename T, typename = std::enable_if_t<std::is_base_of<octoon::GameFeature, T>::value>>
		T* getFeature() const noexcept
		{
			return dynamic_cast<T*>(context_->behaviour->getFeature(T::RTTI));
		}

		void sendMessage(const std::string& event, const std::any& data = std::any()) noexcept
		{
			context_->behaviour->sendMessage(event, data);
		}

		void addMessageListener(const std::string& event, std::function<void(const std::any&)> listener) noexcept
		{
			context_->behaviour->addMessageListener(event, listener);
		}

		void removeMessageListener(const std::string& event, std::function<void(const std::any&)> listener) noexcept
		{
			context_->behaviour->removeMessageListener(event, listener);
		}
	
	private:
		std::shared_ptr<T> model_;
		std::shared_ptr<RabbitContext> context_;
	};
}

#endif