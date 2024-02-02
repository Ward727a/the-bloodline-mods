/*
    This is template, so you can do as you want in it, but be aware that some
    thing will break if you delete them, I reduce the size of this mod
    until there is only the strict necessary.

    The only thing that you can freely delete is the content inside
    "importCustomTypeList"
    as it's used to import the class UTextBlock, I keep that here to show
    you how we can do it
*/


// Do not delete, it's what make the mod get the game instance, and the main menu 
// (and it's what make you're mods run on the way)
// From here
#include <Mod/CppUserModBase.hpp>
#include <DynamicOutput/DynamicOutput.hpp>
#include <Unreal/UObjectGlobals.hpp>
#include <Unreal/UObject.hpp>
#include <Unreal/UClass.hpp>
#include <chrono>
#include <thread>
#include <iostream>
// To here

#include <UTextBlock.h>

// Don't delete from here
using namespace RC;
using namespace RC::Unreal;
using namespace std::this_thread;
using namespace std::chrono_literals;
using std::chrono::system_clock;
// To here



class CustomMod : public RC::CppUserModBase
{
public:

    bool hasTriedLoadGameInstance = false;

    int watchdog_limit = 35;
    int watchdog = 0;

    bool hasUnrealLoaded = false;

    UObject* GameInstance = nullptr;
    UObject* MainMenu = nullptr;

    CustomMod() : CppUserModBase()
    {
        // Edit these field to set you're own information!
        ModName = STR("CustomMod");
        ModVersion = STR("1.0");
        ModDescription = STR("This is my custom mod");
        ModAuthors = STR("UE4SS Team & Ward");
        // Do not change this unless you want to target a UE4SS version
        // other than the one you're currently building with somehow.
        //ModIntendedSDKVersion = STR("2.5.2");
        Output::send<LogLevel::Verbose>(STR("{}[{}] is loaded!\n"), ModName, ModVersion);
    }

    ~CustomMod() override
    {
        Output::send<LogLevel::Verbose>(STR("{}[{}] is stopping!\n"), ModName, ModVersion);
    }

    // Launch after game load then it's launched at EACH TICK so be carefull
    auto on_update() -> void override
    {
        // We check if we didn't already launch the function, 
        // if it's not the case, then we launch it (the bool is set to true in the function)
        if (!hasTriedLoadGameInstance) tryGetGameInstance();
    }

    // Launch only when the mod can use unreal
    auto on_unreal_init() -> void override
    {
        // We set the boolean to true, so the mod can continue
        hasUnrealLoaded = true;

        // We import the types that we created
        // We call this here because we need to use some engine function
        importCustomTypeList();
    }

    // Function to get the loaded game instance
    void tryGetGameInstance()
    {

        // We check if we can have access to unreal (just by security)
        if (!hasUnrealLoaded) return;

        // Then we set this to true, so the "on_update" don't launch the function until the game crash
        hasTriedLoadGameInstance = true;

        // We send a message to the console and GUI Console
        Output::send<LogLevel::Default>(STR("{}[{}] Trying loading game instance\n"), ModName, ModVersion);

        // We find the first game instance existing - The name can be found in the file of the BP in FModel
        GameInstance = UObjectGlobals::FindFirstOf(STR("BP_PersistantGameInstance_C"));

        // By security we check if the pointer is not null (or it will crash the game if we use it if it's null)
        // At the start of the game, it will be null until we see the main menu so it's REALLY needed that we do this check!
        if (GameInstance != nullptr) {
            // We reset the watchdogs
            // The watchdogs is just here by security, so the function doesn't loop without limit
            watchdog = 0;

            Output::send<LogLevel::Verbose>(STR("{}[{}] Loaded game instance with success!\n"), ModName, ModVersion);

            // We then call this function to call each function found in it that need the GameInstance to work
            call_onGameInstanceLoaded();
        }
        else { // if it's null, we increment the watchdogs by 1
            watchdog++;

            // Then we check if the watchdogs is at his limit, if he is at his limit, we stop the function by security
            if (watchdog >= watchdog_limit) {
                Output::send<LogLevel::Error>(STR("{}[{}] Failed to get Game Instance after 35 tries, the mod can't continue!"), ModName, ModVersion);
                return;
            }

            Output::send<LogLevel::Warning>(STR("{}[{}] Failed to get the Game Instance\n"), ModName, ModVersion);
            Output::send<LogLevel::Warning>(STR("{}[{}] Retry in 5 seconds.. ({}/35 tries)\n"), ModName, ModVersion, watchdog);

            // Between each try we set a timer of 5 seconds, so the watchdogs isn't consummed in 2 seconds
            sleep_for(5s);

            // The we make a new try
            tryGetGameInstance();
        }
    }

    // This function is here to try loading the main menu, it's working like the "tryGetGameInstance" with some adjustement
    void tryGetMainMenu()
    {

        // By security we check if Unreal is loaded even if it should be, has this function can't be launch without finding the GameInstance
        // And to find the GameInstance we need to have loaded Unreal, but security before all
        if (!hasUnrealLoaded) return;

        Output::send<LogLevel::Default>(STR("{}[{}] Trying loading main menu\n"), ModName, ModVersion);

        // We find the first instance that we can found of the Menu
        MainMenu = UObjectGlobals::FindFirstOf(STR("BloodlineMenuWidget_C"));

        // We then check if it's not a null pointer
        if (MainMenu != nullptr) { // If it's not
            // We reset the watchdogs
            watchdog = 0;
            Output::send<LogLevel::Verbose>(STR("{}[{}] Loaded Main Menu with success!\n"), ModName, ModVersion);

            // Then we call the function that need the menu before being launched
            call_onMainMenuLoaded();
        }
        else { // If it's null
            // We increment the watchdog
            watchdog++;

            // Then we check if the watchdog is at his limit
            if (watchdog >= watchdog_limit) { // If he his at it's limit, we stop the function by security
                Output::send<LogLevel::Error>(STR("{}[{}] Failed to get Main Menu after 35 tries, the mods can't continue!"), ModName, ModVersion);
                return;
            }

            Output::send<LogLevel::Warning>(STR("{}[{}] Failed to get main menu\n"), ModName, ModVersion);
            Output::send<LogLevel::Warning>(STR("{}[{}] Retry in 5 seconds.. ({}/35 tries)\n"), ModName, ModVersion, watchdog);
            
            // We then wait 5 seconds before trying a new time
            sleep_for(5s);

            // We try again to get the main menu
            tryGetMainMenu();
        }
    }    

    // Called once the gameinstance is hooked in - You can start to use GameInstance here
    void call_onGameInstanceLoaded()
    {
        tryGetMainMenu();
    }

    // Called once the MainMenu is hooked in - You can start to use MainMenu here
    void call_onMainMenuLoaded()
    {
        
    }

    // This function is used so you can import all of you're custom type
    void importCustomTypeList() {
        importCustomType<TextBlock>(L"/Script/UMG.TextBlock");
    }

    // This is a custom I created to easily import an custom class
    // T: we set our custom type we want to import
    // pathToTheObject: we set the path where we can find the object once loaded in the engine (use the GUI to know this!)
    template<typename T>
    void importCustomType(const wchar_t* pathToTheObject) {


        try {
            // So to put it simply:
            // We get the address where the object is in the engine via the path
            // The we assign this to the StaticClassStorage (that is a HARD condition to create you're type)
            UClass* customClass_ptr = UObjectGlobals::StaticFindObject<UClass*>(nullptr, nullptr, (pathToTheObject));
            T::StaticClassStorage = customClass_ptr;
        }
        catch (std::runtime_error err) {
            std::cout << err.what() << std::endl;

            error();
        }
    }

    // Used to print an error message before stopping the mod by it-self so you're game doesn't crash!
    void error() {
        Output::send<LogLevel::Error>(STR("{}[{}] The mod as encountered an error, check the console!\n"), ModName, ModVersion);
        Output::send<LogLevel::Error>(STR("{}[{}] By security the mod will stop it-self!\n"), ModName, ModVersion);

        delete this;
    }
};


// You can change the "MY_AWESOME_MOD_API" to the name you want, but don't forget to replace ANYWHERE you can find it,
// Or you're plugin will not be build!
#define MY_AWESOME_MOD_API __declspec(dllexport)
extern "C"
{
    // Those 2 functions are HARD Requirement by UE4SS, for the love of god, DON'T DELETE OR EDIT THEM,
    // If you delete them, and come asking for help, I WILL NOT HELP YOU
    // !!! I REPEAT - DO NOT DELETE OR EDIT !!!
    MY_AWESOME_MOD_API RC::CppUserModBase* start_mod()
    {
        return new CustomMod();
    }

    MY_AWESOME_MOD_API void uninstall_mod(RC::CppUserModBase* mod)
    {
        delete mod;
    }
}