#include "common.h"

lua_State* g_lua_state = NULL;
int g_erro_func = 0;

int g_buff_len = 10240; // 10k
char* g_buff = new char[g_buff_len];
int g_loop_interval = 100;

#if LUA_VERSION_NUM < 502
int lua_isinteger(lua_State* L, int idx)
{
	lua_Integer x = lua_tointeger(L, idx);
	lua_Number n = lua_tonumber(L, idx);
	if (n == (lua_Number)x)
		return 1;
	return 0;
}

void luaL_requiref(lua_State* L, const char* modname, lua_CFunction openf, int glb)
{
	lua_getfield(L, LUA_REGISTRYINDEX, "_LOADED");
	lua_getfield(L, -1, modname);
	if (!lua_toboolean(L, -1))
	{
		lua_pop(L, 1);
		lua_pushcfunction(L, openf);
		lua_pushstring(L, modname);
		lua_call(L, 1, 1);
		lua_pushvalue(L, -1);
		lua_setfield(L, -3, modname);
	}
	lua_remove(L, -2);
	if (glb)
	{
		lua_pushvalue(L, -1);
		lua_setglobal(L, modname);
	}
}
#endif

uint64 SwapEndian(uint8* buff, uint8 len)
{
	uint64 v = 0;
	uint8 i = 0;
	while (len > 0)
	{
		v = ((v << 8) | *(buff + i));
		len--;
		i++;
	}

	return v;
}

int32 CreateDir(const std::string& directoryPath)
{
	int32 dirPathLen = directoryPath.length();
	if (dirPathLen > MAX_PATH_LEN)
	{
		return -1;
	}
	char tmpDirPath[MAX_PATH_LEN] = { 0 };
	for (int32_t i = 0; i < dirPathLen; ++i)
	{
		tmpDirPath[i] = directoryPath[i];
		if (tmpDirPath[i] == '\\' || tmpDirPath[i] == '/')
		{
			if (ACCESS(tmpDirPath, 0) != 0)
			{
				int32_t ret = MKDIR(tmpDirPath);
				if (ret != 0)
				{
					return ret;
				}
			}
		}
	}
	return 0;
}

