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
#include <dllmain.h>

#include <Mod/CppUserModBase.hpp>
#include <DynamicOutput/DynamicOutput.hpp>
#include <Unreal/UObjectGlobals.hpp>
#include <Unreal/UObject.hpp>
#include <Unreal/UClass.hpp>
#include <chrono>
#include <thread>
#include <iostream>
// To here

#include <Color.h>

#include <UWidget.h>
#include <UTextBlock.h>
#include <UButton.h>
#include <UContentWidget.h>
#include <UPanelWidget.h>

// Include custom slot type
#include <UBackgroundBlurSlot.h>
#include <UPanelSlot.h>
#include <UButtonSlot.h>
#include <UCanvasPanelSlot.h>
#include <UGridSlot.h>
#include <UBorderSlot.h>
#include <UHorizontalBoxSlot.h>
#include <UOverlaySlot.h>
#include <USafeZoneSlot.h>
#include <UScaleBoxSlot.h>
#include <UScrollBoxSlot.h>
#include <USizeBoxSlot.h>
#include <UUniformGridSlot.h>
#include <UVerticalBoxSlot.h>
#include <UWidgetSwitcherSlot.h>
#include <UWindowTitleBarAreaSlot.h>
#include <UWrapBoxSlot.h>
#include <UCanvasPanel.h>
#include <UE4SSProgram.hpp>


#include <UAssetRegistry.hpp>
#include <UAssetRegistryHelpers.hpp>
#include <UModType.h>

// Don't delete from here
using namespace RC;
using namespace RC::Unreal;
using namespace std::this_thread;
using namespace std::chrono_literals;
using std::chrono::system_clock;
namespace fs = std::filesystem;
// To here

class CustomMod : public RC::CppUserModBase
{
public:

    bool hasTriedLoadGameInstance = false;

    int watchdog_limit = 35;
    int watchdog = 0;

    bool hasUnrealLoaded = false;

    bool debug = true;

    UObject* GameInstance = nullptr;
    UObject* MainMenu = nullptr;

    // Make these variables static
    static std::string AssetPath;
    static std::string ObjectPath;

    static bool GameThread;



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
        startTestButton();
    }

    // This function is used so you can import all of you're custom type
    void importCustomTypeList() 
    {

        // Math
        importCustomType<LinearColor>(L"/Script/CoreUObject.LinearColor");

        // UMG
        importCustomType<Widget>(L"/Script/UMG.Widget");
        importCustomType<TextBlock>(L"/Script/UMG.TextBlock");
        importCustomType<Button>(L"/Script/UMG.Button");
        importCustomType<ContentWidget>(L"/Script/UMG.ContentWidget");
        importCustomType<PanelWidget>(L"/Script/UMG.PanelWidget");
        importCustomType<CanvasPanel>(L"/Script/UMG.CanvasPanel");

        // Slot Widgets
        importCustomType<BackgroundBlurSlot>(L"/Script/UMG.BackgroundBlurSlot");
        importCustomType<PanelSlot>(L"/Script/UMG.PanelSlot");
        importCustomType<ButtonSlot>(L"/Script/UMG.ButtonSlot");
        importCustomType<CanvasPanelSlot>(L"/Script/UMG.CanvasPanelSlot");
        importCustomType<GridSlot>(L"/Script/UMG.GridSlot");
        importCustomType<BorderSlot>(L"/Script/UMG.BorderSlot");
        importCustomType<HorizontalBoxSlot>(L"/Script/UMG.HorizontalBoxSlot");
        importCustomType<OverlaySlot>(L"/Script/UMG.OverlaySlot");
        importCustomType<SafeZoneSlot>(L"/Script/UMG.SafeZoneSlot");
        importCustomType<ScaleBoxSlot>(L"/Script/UMG.ScaleBoxSlot");
        importCustomType<ScrollBoxSlot>(L"/Script/UMG.ScrollBoxSlot");
        importCustomType<SizeBoxSlot>(L"/Script/UMG.SizeBoxSlot");
        importCustomType<UniformGridSlot>(L"/Script/UMG.UniformGridSlot");
        importCustomType<VerticalBoxSlot>(L"/Script/UMG.VerticalBoxSlot");
        importCustomType<WidgetSwitcherSlot>(L"/Script/UMG.WidgetSwitcherSlot");
        importCustomType<WindowTitleBarAreaSlot>(L"/Script/UMG.WindowTitleBarAreaSlot");
        importCustomType<WrapBoxSlot>(L"/Script/UMG.WrapBoxSlot");
    }

    // This is a custom I created to easily import an custom class
    // T: we set our custom type we want to import
    // pathToTheObject: we set the path where we can find the object once loaded in the engine (use the GUI to know this!)
    template<typename T>
    void importCustomType(const wchar_t* pathToTheObject) 
    {
        try {
            // So to put it simply:
            // We get the address where the object is in the engine via the path
            // The we assign this to the StaticClassStorage (that is a HARD condition to create you're type)
            UClass* customClass_ptr = UObjectGlobals::StaticFindObject<UClass*>(nullptr, nullptr, (pathToTheObject));

            if (customClass_ptr == nullptr) {
                Output::send<LogLevel::Error>(STR("{}[{}] Error when trying to import a custom type, the pointer to the path \"{}\" is a nullptr!\n"), ModName, ModVersion, pathToTheObject);
                error();
                return;
            }

            T::StaticClassStorage = customClass_ptr;

            if (debug) {
                Output::send<LogLevel::Default>(STR("{}[{}] [DEBUG] Imported custom type: {}\n"), ModName, ModVersion, pathToTheObject);
            }
        }
        catch (std::runtime_error err) {
            std::cout << err.what() << std::endl;

            error();
        }
    }

    void startTestButton()
    {
        //UObject* customAsset = addCustomAsset("/Game/Mods/mod01/ModActor");
        
        // This timer is just for the test, you can remove it
        sleep_for(2s);

        Test();
        

    }

    // All function from here --

    void loadCustomPAK() {

        std::filesystem::path module_directory = UE4SSProgram::get_program().get_module_directory();
        auto game_content_dir = module_directory.parent_path().parent_path().parent_path() / "Content" / "Paks" / "LogicMods";



        try {
            for (const auto& entry : fs::directory_iterator(game_content_dir)) {
                if (entry.is_regular_file()) {

                    std::cout << "File: " << entry.path().stem() << std::endl;


                    addCustomAsset("/Game/Mods/" + entry.path().stem().string() + "/ModActor", "/Game/Mods/" + entry.path().stem().string() + "/ModActor.ModActor_C");
                }
            }
        }
        catch (const fs::filesystem_error& e) {
            std::cerr << "Error accessing directory: " << e.what() << std::endl;
        }
    }

    auto static LoadAssetCustom(UObject* Context, UFunction* Function, void* Parms) -> void {

        if (!CustomMod::GameThread) {
			return;
		}
        CustomMod::GameThread = false;

        Output::send(STR("LoadAssetCustom - GameThread\n"));

        if (CustomMod::AssetPath.empty() || CustomMod::ObjectPath.empty())
        {
			Output::send(STR("AssetPath or ObjectPath is empty\n"));
			return;
		}

        std::filesystem::path module_directory = UE4SSProgram::get_program().get_module_directory();
        auto game_content_dir = module_directory.parent_path().parent_path().parent_path() / "Content" / "Paks" / "LogicMods";

        std::cout << game_content_dir.string() << std::endl;

        auto asset_path_and_name = Unreal::FName(to_wstring(CustomMod::AssetPath), Unreal::FNAME_Add);
        auto ObjectPathFName = Unreal::FName(to_wstring(CustomMod::ObjectPath), Unreal::FNAME_Add);

        auto* asset_registry = static_cast<Unreal::UAssetRegistry*>(Unreal::UAssetRegistryHelpers::GetAssetRegistry().ObjectPointer);
        if (!asset_registry)
        {
            throw std::runtime_error{ "Did not load assets because asset_registry was nullptr\n" };
        }

        Unreal::UObject* loaded_asset{};
        bool was_asset_found{};
        bool did_asset_load{};

        Unreal::FAssetData asset_data = asset_registry->GetAssetByObjectPath(ObjectPathFName);

        asset_data.SetAssetName(FName(STR("ModActor_C")));
        asset_data.SetObjectPath(ObjectPathFName);

        Output::send(STR("--- AssetData ---\n"));
        Output::send(STR("AssetName: {}\n"), asset_data.AssetName().ToString());
        Output::send(STR("ObjectPath: {}\n"), asset_data.ObjectPath().ToString());
        Output::send(STR("PackageName: {}\n"), asset_data.PackageName().ToString());
        Output::send(STR("--- --------- ---\n"));

        sleep_for(5s);
        if ((Unreal::Version::IsAtMost(5, 0) && asset_data.ObjectPath().GetComparisonIndex()) || asset_data.PackageName().GetComparisonIndex())
        {
            was_asset_found = true;
            loaded_asset = Unreal::UAssetRegistryHelpers::GetAsset(asset_data);
            if (loaded_asset)
            {
                did_asset_load = true;
                Output::send(STR("Asset loaded\n"));
            }
            else
            {
                Output::send(STR("Asset was found but not loaded, could be a package\n"));
                Output::send(STR("Object path: {}\n"), ObjectPathFName.ToString());
            }
        }

        if (was_asset_found && did_asset_load)
        {
			Output::send(STR("Asset was found and loaded\n"));
		}
        else
        {
			Output::send(STR("Asset was not found or not loaded\n"));
		}
        
		CustomMod::AssetPath.clear();
		CustomMod::ObjectPath.clear();
    }

    static void setAssetPath(std::string_view _AssetPath) {
        AssetPath = _AssetPath;
    }
    static void setObjectPath(std::string_view _ObjectPath) {
        ObjectPath = _ObjectPath;
    }

    static void setGameThread(bool _GameThread) {
		GameThread = _GameThread;
	}

    UObject* addCustomAsset(std::string_view _AssetPath, std::string_view _ObjectPath) {

        // Assign the Asset path to the static variable
        
        // Assign the Object path to the static variable
        CustomMod::setAssetPath(_AssetPath);
        CustomMod::setObjectPath(_ObjectPath);
        CustomMod::setGameThread(true);

        Unreal::Hook::RegisterProcessEventPreCallback(&LoadAssetCustom);

        return nullptr;
    }

    // --> To here are still in development, don't use it for now
    // I'm still working on it, so it's not ready to be used
    // Once it's finished, I want them to be used to load custom assets from the PAK files
    // Without the need to use the "BPModLoader" from RE-UE4SS
    // But I don't know for now if I will do it or not, it's more a test than anything else

    // This function is used to test the custom function from the custom PAK actor that was created in Unreal Engine 4.26
    // It's used to add a mod to the list of mods that you can open in the game
    void Test() {
        UObject* ModActor = UObjectGlobals::FindFirstOf(STR("ModActor_C"));

        if (ModActor == nullptr) {
			Output::send<LogLevel::Error>(STR("{}[{}] ModActor is nullptr!\n"), ModName, ModVersion);
			error();
			return;
		}

        if (ModActor->IsA<ModType>()) {
			Output::send<LogLevel::Verbose>(STR("{}[{}] ModActor is a ModType!\n"), ModName, ModVersion);

            ModType* ModActorType = StaticCast<ModType*>(ModActor);
            try{
                ModActorType->AddModToList(FString(STR("MyAwesomeMod")), FString(STR("1.0")));
			}
            catch (std::runtime_error err) {
				std::cout << err.what() << std::endl;
				error();
			}

		}
        else {
			Output::send<LogLevel::Error>(STR("{}[{}] ModActor is not a ModType!\n"), ModName, ModVersion);
			error();
			return;
		}

    }

    // Used to print an error message before stopping the mod by it-self so you're game doesn't crash!
    void error() {
        Output::send<LogLevel::Error>(STR("{}[{}] The mod as encountered an error, check the console!\n"), ModName, ModVersion);
        Output::send<LogLevel::Error>(STR("{}[{}] By security the mod will stop it-self!\n"), ModName, ModVersion);

        delete this;
    }
};
std::string CustomMod::AssetPath;
std::string CustomMod::ObjectPath;
bool CustomMod::GameThread;



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