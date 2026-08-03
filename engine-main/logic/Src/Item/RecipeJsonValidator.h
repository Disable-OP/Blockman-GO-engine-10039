#ifndef __RECIPE_JSON_VALIDATOR_HEADER__
#define __RECIPE_JSON_VALIDATOR_HEADER__

#include "rapidjson/document.h"

namespace BLOCKMAN
{
	bool validateRecipeJson(const rapidjson::Document& doc);
}

#endif // !__RECIPE_JSON_VALIDATOR_HEADER__
