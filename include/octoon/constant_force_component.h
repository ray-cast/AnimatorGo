#ifndef OCTOON_CONSTANT_FORCE_COMPONENT_H_
#define OCTOON_CONSTANT_FORCE_COMPONENT_H_

#include <memory>
#include <octoon/game_component.h>
#include <octoon/math/math.h>


namespace octoon
{
    class OCTOON_EXPORT ConstantForce : public GameComponent
	{
        OctoonDeclareSubInterface(ConstantForce, GameComponent)
        public:
            ConstantForce() noexcept;
            ~ConstantForce();
            virtual GameComponentPtr clone() const noexcept;
        
        protected:
    
        private:

            friend class Rigidbody;
    };
}


#endif // OCTOON_CONSTANT_FORCE_COMPONENT_H_