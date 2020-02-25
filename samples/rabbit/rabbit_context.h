#ifndef RABBIT_CONTEXT_H_
#define RABBIT_CONTEXT_H_

namespace rabbit
{
	class RabbitContext
	{
	public:
		class RabbitProfile* profile = nullptr;
		class RabbitBehaviour* behaviour = nullptr;
	};
}

#endif