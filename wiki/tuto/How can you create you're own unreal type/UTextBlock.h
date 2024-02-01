/*
    This file is needed so we can use the native UE function of the TextBox class
    This file is to put inside the "include" filter in the "Unreal" solution in your VS

    Don't ask too much of what do what, I just looked and tried has much as I can to make
    this thing work by looking and copying the file that was already included with RE-UE4SS


    !!!VERY ULTRA IMPORTANT!!!
    Once you created the UTextBlock.h and UTextBlock.cpp, don't forget to add these line in the file "TypeChecker.cpp" in the "src" filter in the "Unreal" solution:

    UClass* text_block_ptr = UObjectGlobals::StaticFindObject<UClass*>(nullptr, nullptr, L"/Script/UMG.TextBlock");
    TextBlock::StaticClassStorage = text_block_ptr;

    in the function: TypeChecker::store_all_object_types() just under: 

    UClass* canvas_panel_ptr = UObjectGlobals::StaticFindObject<UClass*>(nullptr, nullptr, L"/Script/UMG.CanvasPanel");
    UCanvasPanel::StaticClassStorage = canvas_panel_ptr;

    From what I've been able to understand these line are used to make the link between
    the class we created and the class that Unreal Engine have.
*/
#pragma once


#include <UObject.hpp>
#include <FText.hpp>


using namespace RC;

// This is really important:
// Type all of this, IDK why but if you don't type this perfectly it's doesn't work
namespace RC::Unreal {

    // a little information but that make me crazy before I found it:
    // If you don't type the class exactly as Unreal Engine has it, then all you're class will not work 
    // (has long has you use it to call a native function, but.. well we create this file for that after all so..)
    class RC_UE_API TextBlock : public UObject
    {
        DECLARE_EXTERNAL_OBJECT_CLASS(TextBlock, UMG)

        // Important but not required: If it's a function that will be used for a native function, use the same name has Unreal Engine use
        // 
        // 
        // /!\ REALLY IMPORTANT: Use the SAME type has Unreal use, in the arguments and in the return
        // If it doesn't exist you have two choice: Create it if you can, or pray that someone, some where create and give it
        auto SetText(FText InText) -> void;
    };
}

