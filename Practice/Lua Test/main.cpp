#include <iostream>
#include "lua.hpp"

#pragma comment (lib, "lua55.lib")
using namespace std;

int main()
{
	lua_State* L = luaL_newstate(); //루아를연다.
	luaL_openlibs(L); //루아표준라이브러리를연다.
	luaL_loadfile(L, "dragon.lua");

	int error = lua_pcall(L, 0, 0, 0);
	if (error) {
		cout << "Error:" << lua_tostring(L, -1);
		lua_pop(L, 1);
	}

	lua_pcall(L, 0, 0, 0);
	lua_getglobal(L, "pos_x");
	lua_getglobal(L, "pos_y");
	int pos_x = (int)lua_tonumber(L, -2);
	int pos_y = (int)lua_tonumber(L, -1);
	printf("pos_x %d, pos_y %d\n", pos_x, pos_y);
	lua_pop(L, 2);
	lua_close(L);
	return 0;
}