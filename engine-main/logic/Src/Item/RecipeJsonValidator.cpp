#include "RecipeJsonValidator.h"
#include "rapidjson/schema.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "Core.h"

#ifdef GetObject
#undef GetObject
#endif

#define RETURN_ON_FAIL(expr, msg, ...) if (!(expr)) { LordLogError(msg, ##__VA_ARGS__); return false; }

namespace BLOCKMAN
{
	using namespace LORD;
	using namespace rapidjson;
	static const char* schemaJsonString =
		"{                                                                      "
		"    \"type\": \"object\",                                              "
		"    \"description\": \"schema for Blockman crafting recipe\",         "
		"    \"properties\": {                                                  "
		"        \"type\": {                                                    "
		"            \"enum\": [\"crafting_shaped\", \"crafting_shapeless\"]    "
		"        },                                                             "
		"        \"group\": {                                                   "
		"            \"type\": \"string\"                                       "
		"        },                                                             "
		"        \"pattern\": {                                                 "
		"            \"type\": \"array\",                                       "
		"            \"minItems\": 1,                                           "
		"            \"maxItems\": 3,                                           "
		"            \"items\": {                                               "
		"                \"type\": \"string\",                                  "
		"                \"minLength\": 1,                                      "
		"                \"maxLength\": 3                                       "
		"            }                                                          "
		"        },                                                             "
		"        \"key\": {                                                     "
		"            \"type\": \"object\",                                      "
		"            \"minProperties\": 1,                                      "
		"            \"patternProperties\": {                                   "
		"                \"^[^ ]$\": {                                          "
		"                    \"$ref\": \"#/definitions/item_or_item_array\"     "
		"                }                                                      "
		"            },                                                         "
		"            \"additionalProperties\": false                            "
		"        },                                                             "
		"        \"ingredients\": {                                             "
		"            \"type\": \"array\",                                       "
		"            \"items\": {                                               "
		"                \"$ref\": \"#/definitions/ingredient_item\"            "
		"            }                                                          "
		"        },                                                             "
		"        \"result\": {                                                  "
		"            \"$ref\": \"#/definitions/result_item\"                    "
		"        }                                                              "
		"    },                                                                 "
		"    \"required\": [                                                    "
		"        \"type\",                                                      "
		"        \"result\"                                                     "
		"    ],                                                                 "
		"    \"additionalProperties\": false,                                   "
		"    \"definitions\": {                                                 "
		"        \"ingredient_item\": {                                         "
		"            \"type\": \"object\",                                      "
		"            \"properties\": {                                          "
		"                \"item\": {                                            "
		"                    \"type\": \"string\"                               "
		"                },                                                     "
		"                \"data\": {                                            "
		"                    \"type\": \"integer\"                              "
		"                }                                                      "
		"            },                                                         "
		"            \"additionalProperties\": false,                           "
		"            \"required\": [\"item\"]                                   "
		"        },                                                             "
		"        \"result_item\": {                                             "
		"            \"type\": \"object\",                                      "
		"            \"properties\": {                                          "
		"                \"item\": {                                            "
		"                    \"type\": \"string\"                               "
		"                },                                                     "
		"                \"data\": {                                            "
		"                    \"type\": \"integer\"                              "
		"                },                                                     "
		"                \"count\": {                                           "
		"                    \"type\": \"integer\",                             "
		"                    \"minimum\": 1                                     "
		"                }                                                      "
		"            },                                                         "
		"            \"additionalProperties\": false,                           "
		"            \"required\": [\"item\"]                                   "
		"        },                                                             "
		"        \"item_or_item_array\": {                                      "
		"            \"type\": [\"object\", \"array\"],                         "
		"            \"properties\": {                                          "
		"                \"item\": {                                            "
		"                    \"type\": \"string\"                               "
		"                },                                                     "
		"                \"data\": {                                            "
		"                    \"type\": \"integer\"                              "
		"                }                                                      "
		"            },                                                         "
		"            \"items\": {                                               "
		"                \"$ref\": \"#/definitions/item_object\"                "
		"            },                                                         "
		"            \"minItems\": 2,                                           "
		"            \"additionalProperties\": false,                           "
		"            \"required\": [\"item\"]                                   "
		"        }                                                              "
		"    }                                                                  "
		"}                                                                      ";

	static Document createSchemaJson()
	{
		Document doc;
		doc.Parse(schemaJsonString);
		LordAssert(!doc.HasParseError());
		return doc;
	}

	bool validateRecipeJson(const Document & doc)
	{
		static SchemaDocument schema(createSchemaJson());
		//LordLogError("json schema:\n%s", schemaJsonString);
		SchemaValidator validator(schema);
		if (!doc.Accept(validator))
		{
			StringBuffer sb;
			validator.GetInvalidSchemaPointer().StringifyUriFragment(sb);
			LordLogError("Invalid schema: %s", sb.GetString());
			LordLogError("Invalid keyword: %s", validator.GetInvalidSchemaKeyword());
			sb.Clear();
			validator.GetInvalidDocumentPointer().StringifyUriFragment(sb);
			LordLogError("Invalid document: %s", sb.GetString());
			return false;
		}
		String type = doc["type"].GetString();
		if (type == "crafting_shaped")
		{
			RETURN_ON_FAIL(doc.HasMember("pattern"), "recipe with type \"crafting_shaped\" must have \"pattern\" field");
			RETURN_ON_FAIL(doc.HasMember("key"), "recipe with type \"crafting_shaped\" must have \"key\" field");
		}
		else if (type == "crafting_shapeless")
		{
			RETURN_ON_FAIL(doc.HasMember("pattern"), "recipe with type \"crafting_shapeless\" must have \"ingredients\" field");
		}
		if (doc.HasMember("key"))
		{
			for (const auto& pair : doc["key"].GetObject())
			{
				if (pair.value.IsArray())
				{
					String itemName;
					bool firstLoop = true;
					for (const auto& item : pair.value.GetArray())
					{
						if (firstLoop)
						{
							firstLoop = false;
							itemName = item["item"].GetString();
						}
						RETURN_ON_FAIL(itemName == item["item"].GetString(), "canditate items for a key has different itemId");
					}
				}
			}
		}
		return true;
	}
}
