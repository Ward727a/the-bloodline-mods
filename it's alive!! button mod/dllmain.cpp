#include <Mod/CppUserModBase.hpp>
#include <DynamicOutput/DynamicOutput.hpp>
#include <Unreal/UObjectGlobals.hpp>
#include <Unreal/UObject.hpp>
#include <Unreal/FProperty.hpp>
#include <Unreal/UClass.hpp>
#include <Unreal/CanvasPanel.hpp>
#include <Unreal/FText.hpp>
#include <string>
#include <chrono>
#include <thread>
#include <Property/FObjectProperty.hpp>
#include <iostream>
#include <UTextBlock.h>
#include <Widget.h>
#include <Property/FTextProperty.hpp>

using namespace RC;
using namespace RC::Unreal;
using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono_literals; // ns, us, ms, s, h, etc.
using std::chrono::system_clock;




class MyAwesomeMod : public RC::CppUserModBase
{
public:

    bool hasTriedLoadGameInstance = false;

    int watchdog_limit = 35;
    int watchdog = 0;

    bool hasUnrealLoaded = false;

    UObject* GameInstance = nullptr;
    UObject* MainMenu = nullptr;

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

        // We check if we can have access to unreal (just by security)
        if (!hasUnrealLoaded) return;

        // Then we set this to true, so the "on_update" don't launch the function until the game crash
        hasTriedLoadGameInstance = true;

        // We send a message to the console and GUI Console
        Output::send<LogLevel::Verbose>(STR("Trying loading game instance\n"));

        // We find the first game instance existing - The name can be found in the file of the BP in FModel
        GameInstance = UObjectGlobals::FindFirstOf(STR("BP_PersistantGameInstance_C"));

        // By security we check if the pointer is not null (or it will crash the game if we use it if it's null)
        // At the start of the game, it will be null until we see the main menu so it's REALLY needed that we do this check!
        if (GameInstance != nullptr) {
            // We reset the watchdogs
            // The watchdogs is just here by security, so the function doesn't loop without limit
            watchdog = 0;

            Output::send<LogLevel::Verbose>(STR("Loaded game instance with success!\n"));
            
            // We then call this function to call each function found in it that need the GameInstance to work
            call_onGameInstanceLoaded();
        }
        else { // if it's null, we increment the watchdogs by 1
            watchdog++;

            // Then we check if the watchdogs is at his limit, if he is at his limit, we stop the function by security
            if (watchdog >= watchdog_limit) {
                Output::send<LogLevel::Error>(STR("Failed to get Game Instance after 35 tries, the mods can't continue!"));
                return;
            }

            Output::send<LogLevel::Verbose>(STR("Failed to get object\n"));
            Output::send<LogLevel::Verbose>(STR("Retry in 5 seconds.. ({}/35 tries)\n"), watchdog);

            // Between each try we set a timer of 5 seconds, so the watchdogs isn't consummed in 2 seconds
            sleep_for(5s);

            // The we make a new try
            try_loading_game_instance();
        }
    }

    // This function is here to try loading the main menu, it's working like the "try_loading_game_instance" with some adjustement
    void try_loading_main_menu()
    {

        // By security we check if Unreal is loaded even if it should be, has this function can't be launch without finding the GameInstance
        // And to find the GameInstance we need to have loaded Unreal, but security before all
        if (!hasUnrealLoaded) return;

        Output::send<LogLevel::Verbose>(STR("Trying loading main menu\n"));

        // We find the first instance that we can found of the Menu
        MainMenu = UObjectGlobals::FindFirstOf(STR("BloodlineMenuWidget_C"));

        // We then check if it's not a null pointer
        if (MainMenu != nullptr) { // If it's not
            // We reset the watchdogs
            watchdog = 0;
            Output::send<LogLevel::Verbose>(STR("Loaded Main Menu with success!\n"));
            Output::send<LogLevel::Verbose>(STR("{}"), MainMenu->GetFullName().c_str());

            // Then we call the function that need the menu before being launched
            call_onMainMenuLoaded();
        }
        else { // If it's null
            // We increment the watchdog
            watchdog++;

            // Then we check if the watchdog is at his limit
            if (watchdog >= watchdog_limit) { // If he his at it's limit, we stop the function by security
                Output::send<LogLevel::Error>(STR("Failed to get Main Menu after 35 tries, the mods can't continue!"));
                return;
            }

            Output::send<LogLevel::Verbose>(STR("Failed to get main menu\n"));
            Output::send<LogLevel::Verbose>(STR("Retry in 5 seconds.. ({}/35 tries)\n"), watchdog);
            
            // We then wait 5 seconds before trying a new time
            sleep_for(5s);

            // We try again to get the main menu
            try_loading_main_menu();
        }
    }

    

    // Called once the gameinstance is hooked in
    void call_onGameInstanceLoaded()
    {
        try_loading_main_menu();
    }

    // Called once the MainMenu is hooked in
    void call_onMainMenuLoaded()
    {
        test_main_menu();
    }


    // Main Menu function
    // Function that will edit the text in the button
    void test_main_menu()
    {

        // Get the pointer to the "object" we want to get
        void* textBlockVoid = MainMenu->GetValuePtrByPropertyName(STR("TextBlock_2"));

        // Get the property that contains the address to get the object
        FProperty* textBlockProp = MainMenu->GetPropertyByName(STR("TextBlock_2"));

        // We check if the pointer to the object is not null (or else we risk to get a crash)
        if (textBlockVoid == nullptr) {
            Output::send<LogLevel::Error>(STR("textBlockVoid is a nullptr\n"));
            return;
        }

        // Just by security, we check if the property that contains the address is REALLY containing an address to an object
        if (textBlockProp->IsA<FObjectProperty>()) {

            Output::send<LogLevel::Verbose>(STR("Is a ObjectProperty valid property\n"));

            // We convert the Property to an ObjectProperty
            FObjectProperty* objectProperty = StaticCast<FObjectProperty*>(textBlockProp);

            // With the converted value and the pointer to the "object", we can use this function to get the object
            UObject* strin = objectProperty->GetObjectPropertyValue(textBlockVoid);

            // Same has the last, we check if the pointer is not null
            if (strin == nullptr) {
                Output::send<LogLevel::Error>(STR("strin is nullptr\n"));
                return;
            }

            // Just for some debug info
            Output::send<LogLevel::Verbose>(STR("strin name: {}\n"),strin->GetName());

            // We check if the object is an TextBlock before converting it
            if (strin->IsA<TextBlock>()) {

                Output::send<LogLevel::Verbose>(STR("Is a TextBlock valid object\n"));

                // We convert the object to an TextBlock so we can manipulate the data
                TextBlock* textBlock = StaticCast<TextBlock*>(strin);

                // Same has the last, checking if not null
                if (textBlock == nullptr) {
                    Output::send<LogLevel::Error>(STR("textBlock is nullptr\n"));
                    return;
                }

                // Just for some debug info
                Output::send<LogLevel::Error>(STR("textBlock fullname: {}\n"), textBlock->GetFullName());

                // THIS IS REALLY IMPORTANT:
                // Please, every time that you call a native UE function like that, USE TRY AND CATCH
                // ---
                // It's used so the game doesn't crash on any error that it encounter, and then you can log it
                // to debug it
                try {
                    // We call the native function to edit the text of the TextBlock
                    textBlock->SetText(FText(STR("It's alive, it's alive!!")));
                }
                catch (std::runtime_error err) // If the function encouter an error, we "catch" it ourselve
                {
                    // Then we get what is the error
                    const auto error = err.what();
                    
                    // And we write it in the console, the console, not the GUI!
                    // I do this so I don't have to convert the error message to an string (Not a good example, NGL)
                    std::cout << std::string(error) << std::endl;
                }
            }
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