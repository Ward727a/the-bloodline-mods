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
#include <include/dllmain.hpp>

#include <Mod/CppUserModBase.hpp>
#include <DynamicOutput/DynamicOutput.hpp>
#include <Unreal/UObjectGlobals.hpp>
#include <Unreal/UObject.hpp>
#include <Unreal/UClass.hpp>
#include <Unreal/UDataTable.hpp>
#include <chrono>
#include <thread>
#include <iostream>
// To here

#include <UE4SSProgram.hpp>

#include <include/struct/FoodRecipes.hpp>

// Don't delete from here
using namespace RC;
using namespace RC::Unreal;
using namespace std::this_thread;
using namespace std::chrono_literals;
using std::chrono::system_clock;
namespace fs = std::filesystem;
// To here

class SimpleModLoader : public RC::CppUserModBase
{
private:

    bool hasTriedLoadGameInstance = false;

    int watchdog_limit = 35;
    int watchdog = 0;

    bool hasUnrealLoaded = false;

    bool debug = true;

    UObject* GameInstance = nullptr;
    UObject* MainMenu = nullptr;

    // Array of function that will be called once the game instance is loaded
    std::vector<std::function<void(UObject* GameInstance)>> onGameInstanceLoaded;

    // Array of function that will be called once the main menu is loaded
    std::vector<std::function<void(UObject* MainMenu)>> onMainMenuLoaded;

public:
    SimpleModLoader() : CppUserModBase()
    {
        ModName = STR("this will work!");
        ModVersion = STR("1.0");
        ModDescription = STR("A simple mod loader that other mod can use for simplicity");
        ModAuthors = STR("Ward727");

        Output::send<LogLevel::Verbose>(STR("{}[{}] is loaded!\n"), ModName, ModVersion);
    }

    ~SimpleModLoader() override
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

        // We call each function that was added to the vector
        for (auto& func : onGameInstanceLoaded) {
			func(GameInstance);
		}

    }

    // Called once the MainMenu is hooked in - You can start to use MainMenu here
    void call_onMainMenuLoaded()
    {

        Test_DataTable();

        // We call each function that was added to the vector
        for (auto& func : onMainMenuLoaded) {
            func(MainMenu);
        }
    }

    // This function is used to add a function to the vector that will be called once the game instance is loaded
    void add_onGameInstanceLoaded(std::function<void(UObject* GameInstance)> func)
    {
		onGameInstanceLoaded.push_back(func);
	}

    // This function is used to add a function to the vector that will be called once the main menu is loaded
    void add_onMainMenuLoaded(std::function<void(UObject* MainMenu)> func)
    {
        onMainMenuLoaded.push_back(func);
    }

    // Used to print an error message before stopping the mod by it-self so you're game doesn't crash!
    void error(RC::CppUserModBase* mod) {

        error(mod, STR("Unknown"));
    }
    void error(RC::CppUserModBase* mod, StringViewType at) {
        Output::send<LogLevel::Error>(STR("{}[{}] The mod as encountered an error at: {}\n"), mod->ModName, mod->ModVersion, at);
		Output::send<LogLevel::Error>(STR("{}[{}] By security the mod will stop it-self!\n"), mod->ModName, mod->ModVersion);

		delete mod;
    }

    // This function is to test if the mod can extract the data table from the game and return it to our mod
    void Test_GetDataTable() {
        // We try to find the data table
        UObject* DataTableObject = UObjectGlobals::StaticFindObject<UObject*>(nullptr, nullptr, (L"/Game/BloodlinesAlpha/UI/AllFoodRecipes.AllFoodRecipes"));

        // We check if the pointer is not null
        if (DataTableObject == nullptr) {
            Output::send<LogLevel::Error>(STR("{}[{}] DataTableObject is nullptr!\n"), ModName, ModVersion);
			error(this);
			return;
        }

        // We check if the pointer is a UDataTable compatible object
        if (DataTableObject->IsA<UDataTable>()) {
			Output::send<LogLevel::Verbose>(STR("{}[{}] DataTable is a UDataTable!\n"), ModName, ModVersion);

            // We cast the pointer to a UDataTable pointer
            UDataTable* DataTableType = StaticCast<UDataTable*>(DataTableObject);
            try {
				Output::send<LogLevel::Default>(STR("{}[{}] DataTable: {}\n"), ModName, ModVersion, DataTableType->GetName());
			}
            catch (std::runtime_error err) {
				std::cout << err.what() << std::endl;
				error(this);
			}
		}
        else {
			Output::send<LogLevel::Error>(STR("{}[{}] DataTable is not a UDataTable!\n"), ModName, ModVersion);
			error(this);
			return;
		}
    }

    void Test_DataTable() {

        // We try to find the data table
		UObject* DataTableObject = UObjectGlobals::StaticFindObject<UObject*>(nullptr, nullptr, (L"/Game/BloodlinesAlpha/UI/AllFoodRecipes.AllFoodRecipes"));

        // We check if the pointer is not null
        if (DataTableObject == nullptr) {
			Output::send<LogLevel::Error>(STR("{}[{}] DataTableObject is nullptr!\n"), ModName, ModVersion);
			error(this);
			return;
		}

        // We check if the pointer is a UDataTable compatible object
        if (DataTableObject->IsA<UDataTable>()) {
			Output::send<LogLevel::Verbose>(STR("{}[{}] DataTable is a UDataTable!\n"), ModName, ModVersion);

            // We cast the pointer to a UDataTable pointer
			UDataTable* DataTableType = StaticCast<UDataTable*>(DataTableObject);

            // We get all the row names of the data table
            TArray<FName> RowNames = TArray<FName>();

            try {
                DataTableType->GetAllRowNames(RowNames);
            }
            catch (std::runtime_error err) {
                Output::send<LogLevel::Error>(STR("{}[{}] Error when trying to get all row names!\n"), ModName, ModVersion);
                std::cout << err.what() << std::endl;
                error(this, STR("GetAllRowNames"));
            }

            // We loop through each row names
            for (FName RowName : RowNames) {
                Output::send<LogLevel::Normal>(STR("{}[{}] ------------------\n"), ModName, ModVersion);
				Output::send<LogLevel::Default>(STR("{}[{}] RowName: {}\n"), ModName, ModVersion, (RowName.ToString()).c_str());
                bool success = false;
                try {
                    // We check if the row exist
                    success = DataTableType->DoesDataTableRowExist(RowName);
                }
                catch (std::runtime_error err) {
                    Output::send<LogLevel::Error>(STR("{}[{}] Error when trying to check if row exist!\n"), ModName, ModVersion);
                    std::cout << err.what() << std::endl;
                    error(this, STR("DoesDataTableRowExist"));
                }

                if (success) {
                    Output::send<LogLevel::Normal>(STR("{}[{}] Row [{}] exist!\n"), ModName, ModVersion, RowName.ToString().c_str());

                    // We create a new object of the struct that represent the row, for now it's an empty object, but
                    // we will fill it (manually) with the data of the row later
                    auto TableRowBaseObject = FFoodRecipes{};

                    // We create a pointer to the struct (don't change the type of the pointer, it's important)
                    uint8* TableRowPTR = nullptr;
                    try {
                        // We get the row by the name and by a unchecked function
                        TableRowPTR = DataTableType->FindRowUnchecked(FName(RowName.ToString().c_str()));

                        if (TableRowPTR == nullptr) {
                            Output::send<LogLevel::Error>(STR("{}[{}] TableRowPTR is nullptr!\n"), ModName, ModVersion);
                            error(this);
                            return;
                        }

                        //std::cout << "TableRowPTR: " << static_cast<void*>(TableRowPTR) << std::endl;

                        // We get the address of the value of the row
                        auto valAddr = reinterpret_cast<std::uintptr_t> (TableRowPTR);

                        // Then we increment the address to get the value of the row, and we assign it to a variable
                        // 
                        // "Hey but how can I found the offset of the value of the row?"
                        //
                        // Simple, in the gui console of UE4SS, go in the "dumpers" tab, then click on "Dump CXX Headers" then in the folder were you're game.exe is,
                        // You will find a folder named "CXXHeaderDump" open it, then in all these file, search for the structure you need
                        // Here I searched for "FoodRecipes" and I found the structure in the file "FoodRecipes.hpp"
                        // Then I found the offset of the value of the row, and I used it here
                        auto recipeName = *reinterpret_cast<FName*>(valAddr + 0x0); // What I found in the file "FoodRecipes.hpp": RecipeName_2_65E36A93454B299006408E969B069314; // 0x0000 (size: 0x8) (so the offset is 0x0)
                        auto requiredItems = *reinterpret_cast<TArray<TSubclassOf<AActor>>*>(valAddr + 0x8); // What I found in the file "FoodRecipes.hpp": RequiredItems_6_B30C6C5C4380C77141BD40A4E8794332; // 0x0008 (size: 0x10) (so the offset is 0x8)
                        auto completedMeal = *reinterpret_cast<TSubclassOf<AActor>*>(valAddr + 0x18); // What I found in the file "FoodRecipes.hpp": CompletedMeal_9_8889B1BF40DC4BF0326193ABD58AC1E4; // 0x0018 (size: 0x8) (so the offset is 0x18)
                        auto expReward = *reinterpret_cast<int32*>(valAddr + 0x20); // What I found in the file "FoodRecipes.hpp": EXPReward_12_C4D7BF7F454D1ED70CB7B3B0F4E08CA2; // 0x0020 (size: 0x4) (so the offset is 0x20)

                        Output::send<LogLevel::Normal>(STR("{}[{}] RecipeName: {}\n"), ModName, ModVersion, recipeName.ToString().c_str());
                        Output::send<LogLevel::Normal>(STR("{}[{}] RequiredItems: {}\n"), ModName, ModVersion, requiredItems.Num());
                        
                        // For this part is a bit more complex as we need to get an object that is not really created in the game
                        // Even if it's not created, Unreal Engine created a pointer to the "template" of this class, so we can use it to get the name of the class
                        // But by security we check if the pointer to the "template" of this class is not null
                        if (completedMeal.Class != nullptr) {
                            Output::send<LogLevel::Normal>(STR("{}[{}] CompletedMeal: {}\n"), ModName, ModVersion, completedMeal.Class->GetName().c_str());
                        }
                        else {
                            Output::send<LogLevel::Normal>(STR("{}[{}] CompletedMeal: empty\n"), ModName, ModVersion);
                        }

                        Output::send<LogLevel::Normal>(STR("{}[{}] EXPReward: {}\n"), ModName, ModVersion, expReward);
                        Output::send<LogLevel::Normal>(STR("{}[{}] ------------------\n"), ModName, ModVersion);
                        Output::send<LogLevel::Normal>(STR("{}[{}]\n"), ModName, ModVersion);
                    }
                    catch (std::runtime_error err) {
                        Output::send<LogLevel::Error>(STR("{}[{}] Error when trying to find row unchecked!\n"), ModName, ModVersion);
                        std::cout << err.what() << std::endl;
                        error(this, STR("FindRowUnchecked"));
                    }

                }
                else
                {
                    Output::send<LogLevel::Error>(STR("{}[{}] Row [{}] doesn't exist!\n"), ModName, ModVersion, RowName.ToString().c_str());
                }
			}
		}
        else {
			Output::send<LogLevel::Error>(STR("{}[{}] DataTable is not a UDataTable!\n"), ModName, ModVersion);
			error(this);
			return;
		}
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
        return new SimpleModLoader();
    }

    MY_AWESOME_MOD_API void uninstall_mod(RC::CppUserModBase* mod)
    {
        delete mod;
    }
}