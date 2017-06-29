#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <stdio.h>

int luaopen_zip(lua_State *L);

extern const char* builtin_scripts[];
const char * lua_source = NULL;

static int
load_builtin_scripts(lua_State *L)
{
	int i;
	for (i = 0; builtin_scripts[i]; ++i) {
		if (i == 0) {
			lua_source = builtin_scripts[i];
			continue;
		}
		if (luaL_dostring(L, builtin_scripts[i]) != LUA_OK) {
			fprintf(stderr, "%s", lua_tostring(L, -1));
			return 1;
		}
	}

	return LUA_OK;
}

int
main(int argc, const char *argv[]) {
	lua_State *L = luaL_newstate();
	luaL_openlibs(L);
	luaL_requiref(L, "zip", luaopen_zip, 0);

	int err = load_builtin_scripts(L);
	if (err != LUA_OK) {
		fprintf(stderr, "failed to load script");
		return 1;
	}

	if (lua_source == NULL) {
		fprintf(stderr, "no lua_source");
		return 1;
	}

	err = luaL_loadstring(L, lua_source);
	if (err != LUA_OK) {
		fprintf(stderr, "%s", lua_tostring(L, -1));
		return 1;
	}

	int i;
	for (i=1;i<argc;i++) {
		lua_pushstring(L, argv[i]);
	}

	err = lua_pcall(L, argc-1, 0, 0);
	if (err != LUA_OK) {
		fprintf(stderr, "%s", lua_tostring(L, -1));
		return 1;
	}

	lua_close(L);

	return 0;
}
