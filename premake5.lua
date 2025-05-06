PROJECT_GENERATOR_VERSION = 2

newoption({
    trigger = "gmcommon",
    description = "Sets the path to the garrysmod_common (https://github.com/danielga/garrysmod_common) directory",
    value = "path to garrysmod_common directory"
})

local gmcommon = assert(_OPTIONS.gmcommon or os.getenv("GARRYSMOD_COMMON"),
    "you didn't provide a path to your garrysmod_common (https://github.com/danielga/garrysmod_common) directory")
include(gmcommon)

CreateWorkspace({ name = "voltagegmodnetworkanalyzer", allow_debug = false, toolset = compiler })
CreateProject({serverside = true})

IncludeHelpersExtended()
IncludeLuaShared()
IncludeDetouring()
IncludeScanning()

IncludeSDKCommon()
IncludeSDKTier0()
IncludeSDKTier1()
IncludeSDKTier2()
IncludeSDKTier3()
IncludeSDKMathlib()
IncludeSDKRaytrace()
IncludeSteamAPI()