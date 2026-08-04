#ifndef __STATIC_CAST_OP_HEADER__
#define __STATIC_CAST_OP_HEADER__

namespace BLOCKMAN
{
	template<typename _ResultType>
	struct static_cast_op
	{
		template<typename _OriginalType>
		inline _ResultType operator()(const _OriginalType &num)
		{
			return static_cast<_ResultType>(num);
		}
	};
}

#endif
