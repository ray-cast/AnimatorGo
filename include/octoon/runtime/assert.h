#ifndef OCTOON_ASSERT_H_
#define OCTOON_ASSERT_H_

#ifdef _DEBUG
#	include <assert.h>
#else
#   undef assert
#	define	assert(expression)
#endif

#endif