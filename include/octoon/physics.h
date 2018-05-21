#ifndef OCTOON_PHYSICS_H_
#define OCTOON_PHYSICS_H_

#include <octoon/math/math.h>

namespace octoon
{
    class Physics
    {
        public:
        static void setGravity(const math::Vector3& g);
        static math::Vector3 getGravity();

        private:
        static math::Vector3 gravity;
    };
}



#endif // OCTOON_PHYSICS_H_