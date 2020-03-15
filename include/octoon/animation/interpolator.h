#ifndef OCTOON_INTERPOLATOR_H_
#define OCTOON_INTERPOLATOR_H_

namespace octoon::animation
{
	template<typename T>
	class Interpolator
	{
	public:
		Interpolator() = default;
		virtual ~Interpolator() = default;

		virtual T interpolator(T t) const noexcept = 0;
	};
}

#endif