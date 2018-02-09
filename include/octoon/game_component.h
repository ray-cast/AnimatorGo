#ifndef OCTOON_COMPONENT_H_
#define OCTOON_COMPONENT_H_

#include <octoon/game_object.h>

namespace octoon
{
	class OCTOON_EXPORT GameComponent : public runtime::RttiInterface
	{
		OctoonDeclareSubInterface(GameComponent, runtime::RttiInterface)
	public:
		GameComponent() noexcept;
		virtual ~GameComponent() noexcept;

		void setActive(bool active) except;
		bool getActive() const noexcept;

		void setName(const std::string& name) noexcept;
		void setName(std::string&& name) noexcept;
		const std::string& getName() const noexcept;

		GameObjectPtr getGameObject() const noexcept;

		GameComponentPtr getComponent(const runtime::Rtti* type) const noexcept;
		GameComponentPtr getComponent(const runtime::Rtti& type) const noexcept;
		const GameComponents& getComponents() const noexcept;

		template<typename T>
		std::shared_ptr<T> getComponent() const noexcept
		{
			return std::dynamic_pointer_cast<T>(this->getComponent(T::RTTI));
		}

		virtual GameComponentPtr clone() const noexcept = 0;

	protected:
		void addComponentDispatch(GameDispatchType type, GameComponent* component) noexcept;
		void addComponentDispatch(GameDispatchType type, const GameComponentPtr& component) noexcept;

		void removeComponentDispatch(GameDispatchType type, GameComponent* component) noexcept;
		void removeComponentDispatch(GameDispatchType type, const GameComponentPtr& component) noexcept;

	private:
		virtual void onAttach() except;
		virtual void onDetach() noexcept;

		virtual void onAttachComponent(const GameComponentPtr& component) except;
		virtual void onDetachComponent(const GameComponentPtr& component) noexcept;

		virtual void onActivate() except;
		virtual void onDeactivate() noexcept;

		virtual void onFrameBegin() except;
		virtual void onFrame() except;
		virtual void onFrameEnd() except;

		virtual void onMoveBefore() except;
		virtual void onMoveAfter() except;

		virtual void onLayerChangeBefore() except;
		virtual void onLayerChangeAfter() except;

	private:
		friend GameObject;
		void _setGameObject(GameObject* gameobj) noexcept;

	private:
		bool active_;

		std::string name_;

		GameObject* gameObject_;
	};
}

#endif