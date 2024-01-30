#include <Mod/CppUserModBase.hpp>
#include <DynamicOutput/DynamicOutput.hpp>
#include <Unreal/UObjectGlobals.hpp>
#include <Unreal/UObject.hpp>
#include <Unreal/FProperty.hpp>
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

    bool hasTriedLoadGameInstance = false;
    bool hasUnrealLoaded = false;

    UObject* GameInstance = nullptr;

    std::vector <std::function<void(UObject*)>> onGameInstanceLoaded_callbacks;

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

    void hook_LoadPlayerData()
    {
        UFunction* saveLoopFunction = GameInstance->GetFunctionByName(STR("LoadPlayerData"));

        UnrealScriptFunctionCallable preHook = hooked_PreLoadPlayerData;
        UnrealScriptFunctionCallable postHook = hooked_PostLoadPlayerData;

        UObjectGlobals::RegisterHook(saveLoopFunction, preHook, postHook, GameInstance);

    }

    void try_loading_game_instance()
    {

        if (!hasUnrealLoaded) return;

        hasTriedLoadGameInstance = true;

        Output::send<LogLevel::Verbose>(STR("Trying loading game instance\n"));

        GameInstance = UObjectGlobals::FindFirstOf(STR("BP_PersistantGameInstance_C"));

        if (GameInstance != nullptr) {
            Output::send<LogLevel::Verbose>(STR("Object Name: {}\n"), GameInstance->GetFullName());

            auto LoadCompleted = GameInstance->GetFunctionByNameInChain(STR("Set Hotbar Numbers"));
            auto MoneyProperty = GameInstance->GetPropertyByName(STR("Money"));

            if (LoadCompleted != nullptr) {
                Output::send<LogLevel::Verbose>(STR("Success to get the function \"Set Hotbar Numbers()\"\n"));
            }
            else {
                Output::send<LogLevel::Verbose>(STR("Failed to get the function \"Set Hotbar Numbers()\"\n"));
            }

            if (MoneyProperty != nullptr) {
                Output::send<LogLevel::Verbose>(STR("Success to get the property \"Money\"\n"));
            }
            else {
                Output::send<LogLevel::Verbose>(STR("Failed to get the property \"Money\"\n"));
            }

            call_onGameInstanceLoaded();
        }
        else {
            Output::send<LogLevel::Verbose>(STR("Failed to get object\n"));
            Output::send<LogLevel::Verbose>(STR("Retry in 5 seconds..\n"));
            sleep_for(5s);
            try_loading_game_instance();
        }
    }

    /* Register an callback that will be called once the mod has found the gameinstance
    The callback receive the gameinstance in return    
    
    */
    void register_onGameInstanceLoaded(std::function<void(UObject* GI)> callback)
    {
        onGameInstanceLoaded_callbacks.push_back(callback);
    }

    void call_onGameInstanceLoaded()
    {
        hook_LoadPlayerData();
    }

    // Launch after game load
    auto on_update() -> void override
    {
        if(!hasTriedLoadGameInstance) try_loading_game_instance();
    }

    auto on_unreal_init() -> void override
    {
        hasUnrealLoaded = true;
    }


    static void hooked_PreLoadPlayerData(UnrealScriptFunctionCallableContext& Context, void* VGameInstance)
    {

    }
    static void hooked_PostLoadPlayerData(UnrealScriptFunctionCallableContext& Context, void* VGameInstance)
    {
        Output::send<LogLevel::Verbose>(STR("hooked_LoadPlayerData...\n"));

        UObject* gameInstance = StaticCast<UObject*>(VGameInstance);

        float* MaxHealth1 = gameInstance->GetValuePtrByPropertyName<float>(STR("Max Health"));

        if (MaxHealth1 != nullptr) {

            Output::send<LogLevel::Verbose>(STR("{}\n"), *MaxHealth1);
            *MaxHealth1 = float(5000);
            
        }

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