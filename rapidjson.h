#ifndef NCS_RAPIDJSON_H
#define NCS_RAPIDJSON_H

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

static std::string JsonToString(const rapidjson::Value* node)
{
	if (node)
	{
		rapidjson::StringBuffer sb;
		rapidjson::Writer<rapidjson::StringBuffer> writer(sb); // PrettyWriter
		node->Accept(writer);
		return sb.GetString();
	}

	return "";
}

class MsgRevJson
{
public:
	MsgRevJson(rapidjson::Value& msgObj) noexcept
		: m_obj(msgObj)
	{}

	MsgRevJson(rapidjson::Document& dom, rapidjson::Document::AllocatorType& a) noexcept
		: m_obj(*(new rapidjson::Value()))
	{
		m_obj.CopyFrom(dom, a);
	}

	bool GetString(std::string& str, const char *key = "")
	{
		if (m_obj.IsObject())
		{
			if (m_obj.HasMember(key))
			{
				if (m_obj[key].IsString())
				{
					str = m_obj[key].GetString();
					return true;
				}
			}
		}

		if (m_obj.IsString())
		{
			str = m_obj.GetString();
			return true;
		}

		return false;
	}

	bool GetInt(int &i, const char *key = "")
	{
		if (m_obj.IsObject())
		{
			if (m_obj.HasMember(key))
			{
				if (m_obj[key].IsInt())
				{
					i = m_obj[key].GetInt();
					return true;
				}
			}
		}

		if (m_obj.IsInt())
		{
			i = m_obj.GetInt();
			return true;
		}

		return false;
	}

	bool GetDouble(double &d, const char *key = "")
	{
		if (m_obj.IsObject())
		{
			if (m_obj.HasMember(key))
			{
				if (m_obj[key].IsDouble())
				{
					d = m_obj[key].GetDouble();
					return true;
				}
			}
		}

		if (m_obj.IsDouble())
		{
			d = m_obj.GetDouble();
			return true;
		}

		return false;
	}

	MsgRevJson* GetArray(const char *key, rapidjson::SizeType index)
	{
		if (m_obj.IsObject())
		{
			if (m_obj.HasMember(key))
			{
				if (m_obj[key].IsArray())
				{
					if (index >= 0 && index < m_obj[key].Size())
					{
						return new MsgRevJson(m_obj[key][index]);
					}
				}
			}
		}

		if (m_obj.IsArray())
		{
			if (index >= 0 && index < m_obj.Size())
			{
				return new MsgRevJson(m_obj[index]);
			}
		}

		return nullptr;
	}

	bool GetArraySize(int &i, const char *key)
	{
		if (m_obj.IsObject())
		{
			if (m_obj.HasMember(key))
			{
				if (m_obj[key].IsArray())
				{
					i = m_obj[key].Size();
					return true;
				}
			}
		}

		if (m_obj.IsArray())
		{
			i = m_obj.Size();
			return true;
		}

		return false;
	}

	MsgRevJson* GetJObject(const char *key)
	{
		if (m_obj.IsObject())
		{
			if (m_obj.HasMember(key))
			{
				if (m_obj[key].IsObject())
				{
					return new MsgRevJson(m_obj[key]);
				}
			}
		}	

		return nullptr;
	}

	bool GetBool(bool &b, const char *key = "")
	{
		if (m_obj.IsObject())
		{
			if (m_obj.HasMember(key))
			{
				if (m_obj[key].IsBool())
				{
					b = m_obj[key].GetBool();
					return true;
				}
			}
		}

		if (m_obj.IsBool())
		{
			b = m_obj.GetBool();
			return true;
		}

		return false;
	}

	static MsgRevJson* GetFromHandle(cell_t hdl, IPluginContext* pContext);

	std::string GetJson()
	{
		return JsonToString(&m_obj);
	}

private:
	rapidjson::Value& m_obj;
};

class MsgSendJson
{
public:
	MsgSendJson(char* pattern)
		: m_sb()
		, m_writer(m_sb)
	{
		m_writer.StartObject();
		m_writer.Key("Router");
		m_writer.String(pattern);
		m_writer.Key("Msg");
		m_writer.StartObject();
	}

	static MsgSendJson* GetFromHandle(cell_t hdl, IPluginContext* pContext);

	void StartObject(const char* key)
	{
		m_writer.Key(key);
		m_writer.StartObject();
	}

	void StartObject()
	{
		m_writer.StartObject();
	}

	void EndObject()
	{
		m_writer.EndObject();
	}

	std::string GetMsg()
	{
		m_writer.EndObject();
		m_writer.EndObject();
		return m_sb.GetString();
	}

	void String(const char *key, const char *str)
	{
		m_writer.Key(key);
		m_writer.String(str);
	}

	void String(const char *str)
	{
		m_writer.String(str);
	}

	void Bool(const char *key, bool b)
	{
		m_writer.Key(key);
		m_writer.Bool(b);
	}

	void Bool(bool b)
	{
		m_writer.Bool(b);
	}

	void Null(const char *key)
	{
		m_writer.Key(key);
		m_writer.Null();
	}

	void Null()
	{
		m_writer.Null();
	}

	void Int(const char *key, int i)
	{
		m_writer.Key(key);
		m_writer.Int(i);
	}

	void Int(int i)
	{
		m_writer.Int(i);
	}

	void Double(const char *key, double d)
	{
		m_writer.Key(key);
		m_writer.Double(d);
	}

	void Double(double d)
	{
		m_writer.Double(d);
	}

	void StartArray(const char *key)
	{
		m_writer.Key(key);
		m_writer.StartArray();
	}

	void EndArray()
	{
		m_writer.EndArray();
	}

private:
	rapidjson::StringBuffer m_sb;
	rapidjson::Writer<rapidjson::StringBuffer> m_writer;
};

class MsgRevHandler : public IHandleTypeDispatch
{
public:
	void OnHandleDestroy(HandleType_t type, void* object)
	{
		if (object != nullptr)
		{
			MsgRevJson* obj = static_cast<MsgRevJson*>(object);
			delete obj;
		}
	}
};

class MsgSendHandler : public IHandleTypeDispatch
{
public:
	void OnHandleDestroy(HandleType_t type, void* object)
	{
		if (object != nullptr)
		{
			MsgSendJson* obj = static_cast<MsgSendJson*>(object);
			delete obj;
		}
	}
};

#endif