#include <GarrysMod/Lua/Interface.h>
#include <GarrysMod/Interfaces.hpp>
#include <ws2tcpip.h>

bool debugging_enabled;

LUA_FUNCTION_STATIC(EnableNetworkDebugging) {
	LUA->CheckType(1, GarrysMod::Lua::Type::BOOL);
	debugging_enabled = LUA->GetBool(1);
	if (debugging_enabled) {
		printf("enabled debugging\n");
	}
	else {
		printf("disable debugging\n");
	}
	return 0;
}

namespace global {
	typedef int32_t(*hook_recvfrom_t)(int32_t s, char* buf, int32_t buflen, int32_t flags, sockaddr* from, int32_t* fromlen);
	hook_recvfrom_t hook_recvfrom = nullptr;

	void dbg_hex_buffer(const void* data, int len) {
		const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data);
		printf("(%d bytes): ", len);
		for (int i = 0; i < len; ++i)
			printf("%02X ", bytes[i]);
		printf("\n");
	}

	static int32_t hook_recvfrom_d(int32_t s, char* buf, int32_t buflen, int32_t flags, sockaddr* from, int32_t* fromlen) {
		sockaddr_in& infrom = *reinterpret_cast<sockaddr_in*>(from);
		int32_t len = hook_recvfrom(s, buf, buflen, flags, from, fromlen);
		if (len == -1)
			return -1;

		uint8_t type = *reinterpret_cast<const uint8_t*>(buf + 4);

		char ipaddr_s[32];
		inet_ntop(AF_INET, &(((struct sockaddr_in *)from)->sin_addr), ipaddr_s, sizeof(ipaddr_s));
		if (debugging_enabled) {
			printf("got '%c' packet from %s. buf: \n", type, ipaddr_s);
			dbg_hex_buffer(buf, len);
		}

		return len;
	}

	static void preinitialize(GarrysMod::Lua::ILuaBase* LUA) {
		LUA->CreateTable();

		LUA->PushString("voltagegmodnetworkanalyzer");
		LUA->SetField(-2, "Version");

		LUA->PushNumber(010000);
		LUA->SetField(-2, "VersionNum");
	}

	static void initialize(GarrysMod::Lua::ILuaBase* LUA) {
		LUA->PushCFunction(EnableNetworkDebugging);
		LUA->SetField(-2, "EnableNetworkDebugging");
		LUA->SetField(GarrysMod::Lua::INDEX_GLOBAL, "voltagegmodnetworkanalyzer");
		hook_recvfrom = VCRHook_recvfrom;
		VCRHook_recvfrom = hook_recvfrom_d;
		printf("voltagegmodnetworkanalyzer // by voltage. vcrhook_recvfrom: 0x%p\n", VCRHook_recvfrom);
	}

	static void deinitialize(GarrysMod::Lua::ILuaBase* LUA) {
		VCRHook_recvfrom = hook_recvfrom;
		LUA->PushNil();
		LUA->SetField(GarrysMod::Lua::INDEX_GLOBAL, "voltagegmodnetworkanalyzer");
	}

}

GMOD_MODULE_OPEN() {
	global::preinitialize(LUA);
	global::initialize(LUA);
	return 1;
}

GMOD_MODULE_CLOSE() {
	global::deinitialize(LUA);
	return 0;
}