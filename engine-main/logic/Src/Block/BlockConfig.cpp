#include "BlockConfig.h"
#include "rapidjson/schema.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/stringbuffer.h"
#include "Trigger/TriggerConfigParser.h"
#include "Util/RapidJsonObject.h"

#define RETURN_ON_FAIL(expr, msg, ...) if(!(expr)) { LordLogError(msg, ##__VA_ARGS__); return false; }

namespace BLOCKMAN
{
	using namespace LORD;
	using namespace rapidjson;

	static char blockJsonSchema[] =
		"{                                                                             "
		"    \"type\": \"object\",                                                     "
		"    \"description\": \"schema for Blockman block config json\",               "
		"    \"additionalProperties\": false,                                          "
		"    \"required\": [\"texture\"],                                              "
		"    \"properties\": {                                                         "
		"        \"texture\": {                                                        "
		"            \"type\": [\"string\", \"object\"],                               "
		"            \"additionalProperties\": false,                                  "
		"            \"required\": [                                                   "
		"                \"top\",                                                      "
		"                \"bottom\",                                                   "
		"                \"left\",                                                     "
		"                \"right\",                                                    "
		"                \"front\",                                                    "
		"                \"back\"                                                      "
		"            ],                                                                "
		"            \"properties\": {                                                 "
		"                \"top\": {                                                    "
		"                    \"type\": \"string\"                                      "
		"                },                                                            "
		"                \"bottom\": {                                                 "
		"                    \"type\": \"string\"                                      "
		"                },                                                            "
		"                \"left\": {                                                   "
		"                    \"type\": \"string\"                                      "
		"                },                                                            "
		"                \"right\": {                                                  "
		"                    \"type\": \"string\"                                      "
		"                },                                                            "
		"                \"front\": {                                                  "
		"                    \"type\": \"string\"                                      "
		"                },                                                            "
		"                \"back\": {                                                   "
		"                    \"type\": \"string\"                                      "
		"                }                                                             "
		"            }                                                                 "
		"        },                                                                    "
		"        \"color\": {                                                          "
		"            \"type\": \"string\",                                             "
		"            \"pattern\": \"[0-9a-fA-F]{6}\"                                   "
		"        },                                                                    "
		"        \"state\": {                                                          "
		"            \"enum\": [\"GAS\", \"SOLID\"]                                    "
		"        },                                                                    "
		"        \"transparent\": {                                                    "
		"            \"type\": \"boolean\"                                             "
		"        },                                                                    "
		"        \"collisionBox\": {                                                   "
		"            \"type\": [\"object\", \"null\"],                                 "
		"            \"additionalProperties\": false,                                  "
		"            \"required\": [\"min\", \"max\"],                                 "
		"            \"properties\": {                                                 "
		"                \"min\": {                                                    "
		"                    \"$ref\": \"#/definitions/coordinate\"                    "
		"                },                                                            "
		"                \"max\": {                                                    "
		"                    \"$ref\": \"#/definitions/coordinate\"                    "
		"                }                                                             "
		"            }                                                                 "
		"        },                                                                    "
		"        \"resistance\": {                                                     "
		"            \"type\": \"integer\",                                            "
		"            \"minimum\": 0,                                                   "
		"            \"maximum\": 10000                                                "
		"        },                                                                    "
		"        \"flammable\": {                                                      "
		"            \"type\": \"boolean\"                                             "
		"        },                                                                    "
		"        \"triggers\": {                                                       "
		"            \"type\": \"array\",                                              "
		"            \"items\": {                                                      "
		"                \"type\": \"object\",                                         "
		"                \"additionalProperties\": true,                               "
		"                \"required\": [\"type\", \"actions\"],                        "
		"                \"properties\": {                                             "
		"                    \"type\": {                                               "
		"                        \"type\": \"string\"                                  "
		"                    },                                                        "
		"                    \"actions\": {                                            "
		"                        \"type\": \"array\",                                  "
		"                        \"items\": {                                          "
		"                            \"type\": \"object\",                             "
		"                            \"additionalProperties\": true,                   "
		"                            \"required\": [\"type\"],                         "
		"                            \"properties\": {                                 "
		"                                \"type\": {                                   "
		"                                    \"type\": \"string\"                      "
		"                                }                                             "
		"                            }                                                 "
		"                        }                                                     "
		"                    }                                                         "
		"                }                                                             "
		"            }                                                                 "
		"        }                                                                     "
		"    },                                                                        "
		"    \"definitions\": {                                                        "
		"        \"coordinate\": {                                                     "
		"            \"type\": \"object\",                                             "
		"            \"additionalProperties\": false,                                  "
		"            \"required\": [\"x\", \"y\", \"z\"],                              "
		"            \"properties\": {                                                 "
		"                \"x\": {                                                      "
		"                    \"type\": \"number\"                                      "
		"                },                                                            "
		"                \"y\": {                                                      "
		"                    \"type\": \"number\"                                      "
		"                },                                                            "
		"                \"z\": {                                                      "
		"                    \"type\": \"number\"                                      "
		"                }                                                             "
		"            }                                                                 "
		"        }                                                                     "
		"    }                                                                         "
		"}                                                                             ";

	static Document createSchemaJson()
	{
		Document doc;
		doc.Parse(blockJsonSchema);
		LordAssert(!doc.HasParseError());
		return doc;
	}

	static bool validateJson(const Document & doc)
	{
		static SchemaDocument schema(createSchemaJson());
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
		return true;
	}

	bool BlockConfig::readJson(BlockConfig & output, const String& configPath)
	{
		RETURN_ON_FAIL(PathUtil::IsFileExist(configPath), "cannot read block.json: json file does not exist: %s", configPath.c_str());
		std::ifstream ifs(configPath.c_str());
		IStreamWrapper is(ifs);
		Document json;
		RETURN_ON_FAIL(!json.ParseStream(is).HasParseError(), "cannot read block.json: failed to parse json");
		RETURN_ON_FAIL(validateJson(json), "cannot read block.json: block json is invalid");

		if (json.HasMember("texture"))
		{
			if (json["texture"].IsString())
			{
				output.texture.top
					= output.texture.bottom
					= output.texture.left
					= output.texture.right
					= output.texture.front
					= output.texture.back 
					= json["texture"].GetString();
			}
			else
			{
				const auto& textureJson = json["texture"].GetObject();
				output.texture.top = textureJson["top"].GetString();
				output.texture.bottom = textureJson["bottom"].GetString();
				output.texture.left = textureJson["left"].GetString();
				output.texture.right = textureJson["right"].GetString();
				output.texture.front = textureJson["front"].GetString();
				output.texture.back = textureJson["back"].GetString();
				if (!validateTextureConfig(output.texture))
				{
					LordLogError("cannot read block.json: texture config is invalid");
					return false;
				}
			}
		}
		if (json.HasMember("color"))
		{
			String colorString = json["color"].GetString();
			int r = StringUtil::ParseHex<int>(colorString.substr(0, 2));
			int g = StringUtil::ParseHex<int>(colorString.substr(2, 2));
			int b = StringUtil::ParseHex<int>(colorString.substr(4, 2));
			output.color = {
				static_cast<Real>(r) / 255,
				static_cast<Real>(g) / 255,
				static_cast<Real>(b) / 255,
			};
		}
		if (json.HasMember("state"))
		{
			String stateString = json["state"].GetString();
			if (stateString == "SOLID")
			{
				output.state = BlockState::SOLID;
			}
			else if (stateString == "GAS")
			{
				output.state = BlockState::GAS;
			}
			else
			{
				LordLogError("cannot read block.json: unknown state value %s", stateString.c_str());
				return false;
			}
		}
		if (json.HasMember("transparent"))
		{
			output.transparent = json["transparent"].GetBool();
		}
		if (json.HasMember("collisionBox"))
		{
			const auto& boxMinJson = json["collisionBox"].GetObject()["min"].GetObject();
			const auto& boxMaxJson = json["collisionBox"].GetObject()["max"].GetObject();
			output.collisionBox = {
				{
					boxMinJson["x"].GetFloat(),
					boxMinJson["y"].GetFloat(),
					boxMinJson["z"].GetFloat()
				},
				{
					boxMaxJson["x"].GetFloat(),
					boxMaxJson["y"].GetFloat(),
					boxMaxJson["z"].GetFloat()
				}
			};
		}
		if (json.HasMember("resistance"))
		{
			output.resistance = json["resistance"].GetInt();
		}
		if (json.HasMember("flammable"))
		{
			output.flammable = json["flammable"].GetBool();
		}
		if (json.HasMember("triggers"))
		{
			for (const auto& triggerJson : json["triggers"].GetArray())
			{
				if (auto trigger = TriggerConfigParser::Instance()->parse({ triggerJson.GetObject() }))
				{
					output.triggers.push_back(trigger);
				}
			}
		}
		return true;
	}

	bool BlockConfig::validateTextureConfig(const TextureConfig & texture)
	{
		auto pos = texture.top.find_last_of(':');
		if (pos == String::npos)
		{
			return texture.bottom.find(':') == String::npos
				&& texture.left.find(':') == String::npos
				&& texture.right.find(':') == String::npos
				&& texture.front.find(':') == String::npos
				&& texture.back.find(':') == String::npos;
		}
		else
		{
			const auto& nameSpace = texture.top.substr(0, pos);
			return texture.bottom.substr(0, texture.bottom.find_last_of(':')) == nameSpace
				&& texture.left.substr(0, texture.left.find_last_of(':')) == nameSpace
				&& texture.right.substr(0, texture.left.find_last_of(':')) == nameSpace
				&& texture.front.substr(0, texture.left.find_last_of(':')) == nameSpace
				&& texture.back.substr(0, texture.left.find_last_of(':')) == nameSpace;
		}
	}
}