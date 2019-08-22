#ifndef MYSTICLIT_ENTITIES_MODULE_H_
#define MYSTICLIT_ENTITIES_MODULE_H_

#include <mysticLit_model.h>
#include <octoon/game_object.h>

namespace MysticLit
{
	class EntitiesModule final : public MysticLitModule
	{
	public:
		EntitiesModule() noexcept;
		virtual ~EntitiesModule() noexcept;

		virtual void reset() noexcept override;

	private:
		EntitiesModule(const EntitiesModule&) = delete;
		EntitiesModule& operator=(const EntitiesModule&) = delete;

	public:
		octoon::GameObjects objects;
		octoon::GameObjectPtr camera;
		octoon::GameObjectPtr enviromentLight;
	};
}

#endif