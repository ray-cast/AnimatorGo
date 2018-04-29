#ifndef OCTOON_PHYSICS_H_
#define OCTOON_PHYSICS_H_

#include <octoon/math/math.h>

namespace octoon
{
    class Physics
    {
        public:
        static void setGravity(const math::Vector2& g);
        static math::Vector2 getGravity();

        private:
        static math::Vector2 gravity;
    };
}



#endif // OCTOON_PHYSICS_H_