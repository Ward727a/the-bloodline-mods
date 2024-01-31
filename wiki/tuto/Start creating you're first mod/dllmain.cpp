#include <Mod/CppUserModBase.hpp>
#include <DynamicOutput/DynamicOutput.hpp>
#include <Unreal/UObjectGlobals.hpp>
#include <Unreal/UObject.hpp>
#include <Unreal/FProperty.hpp>
#include <Unreal/UClass.hpp>
#include <Unreal/FText.hpp>
#include <string>
#include <chrono>
#include <thread>

using namespace RC;
using namespace RC::Unreal;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono_literals; // ns, us, ms, s, h, etc.
using std::chrono::system_clock;




class MyAwesomeMod : public RC::CppUserModBase
{
public:

    int watchdog_limit = 35;
    int watchdog = 0;

    bool HasUnrealLoaded = false;
    bool hasTriedLoadGameInstance = false;

    UObject* GameInstance = nullptr;

    MyAwesomeMod() : CppUserModBase()
    {
        ModName = STR("MyAwesomeMod");
        ModVersion = STR("1.0");
        ModDescription = STR("This is my awesome mod");
        ModAuthors = STR("UE4SS Team");
        // Do not change this unless you want to target a UE4SS version
        // other than the one you're currently building with somehow.
        //ModIntendedSDKVersion = STR("2.5.2");

        Output::send<LogLevel::Verbose>(STR("MyAwesomeMod says hello\n"));

    }

    ~MyAwesomeMod() override
    {
    }

    // Launch after game load then it's launched at EACH TICK so be carefull
    auto on_update() -> void override
    {
        // We check if we didn't already launch the function, 
        // if it's not the case, then we launch it (the bool is set to true in the function)
        if (!hasTriedLoadGameInstance) try_loading_game_instance();
    }

    // Launch only when the mod can use unreal
    auto on_unreal_init() -> void override
    {
        // We set the boolean to true, so the mod can continue
        hasUnrealLoaded = true;
    }

    // Function to get the loaded game instance
    void try_loading_game_instance()
    {
        if (!hasUnrealLoaded) return;

        hasTriedLoadGameInstance = true;

        GameInstance = UObjectGlobals::FindFirstOf(STR("BP_PersistantGameInstance_C"));

        if (GameInstance != nullptr) {
            call_onGameInstanceLoaded();
        }
        else {
            watchdog++;

            if (watchdog >= watchdog_limit) {
                return;
            }

            sleep_for(5s);
            try_loading_game_instance();
        }
    }




    void call_onGameInstanceLoaded()
    {
        hook_LoadPlayerData();
    }


    void hook_LoadPlayerData()
    {
        UFunction* saveLoopFunction = GameInstance->GetFunctionByName(STR("LoadPlayerData"));

        UnrealScriptFunctionCallable preHook = hooked_PreLoadPlayerData;
        UnrealScriptFunctionCallable postHook = hooked_PostLoadPlayerData;

        UObjectGlobals::RegisterHook(saveLoopFunction, preHook, postHook, GameInstance);
    }

    static void hooked_PreLoadPlayerData(UnrealScriptFunctionCallableContext& Context, void* VGameInstance)
    {

    }




    static void hooked_PostLoadPlayerData(UnrealScriptFunctionCallableContext& Context, void* VGameInstance)
    {

        UObject* gameInstance = StaticCast<UObject*>(VGameInstance);

        float* MaxHealth = gameInstance->GetValuePtrByPropertyName<float>(STR("Max Health"));

    }





};


#define MY_AWESOME_MOD_API __declspec(dllexport)
extern "C"
{
    MY_AWESOME_MOD_API RC::CppUserModBase* start_mod()
    {
        return new MyAwesomeMod();
    }

    MY_AWESOME_MOD_API void uninstall_mod(RC::CppUserModBase* mod)
    {
        delete mod;
    }
}