#ifndef UNREAL_UTRANSLATABLE_H_
#define UNREAL_UTRANSLATABLE_H_

#include <QWidget>
#include <QEvent>
#include <QWidget>

namespace unreal
{	
	class UTranslatable
	{
	public:
		explicit UTranslatable();
		~UTranslatable();

        virtual void retranslate();
	};
}

#endif