#ifndef FLOWER_CONTEXT_H_
#define FLOWER_CONTEXT_H_

namespace flower
{
	class RabbitContext
	{
	public:
		class FlowerProfile* profile = nullptr;
		class FlowerBehaviour* behaviour = nullptr;
	};
}

#endif