#ifndef OCTOON_MYSTICLIT_CONTEXT_H_
#define OCTOON_MYSTICLIT_CONTEXT_H_

namespace MysticLit
{
	class MysticLitContext
	{
	public:
		class MysticLitProfile* profile = nullptr;
		class MysticlitBehaviour* behaviour = nullptr;
	};
}

#endif