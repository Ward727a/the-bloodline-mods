/*
    This file is needed so we can use the native UE function of the TextBox class

    Don't ask too much of what do what, I just looked and tried has much as I can to make
    this thing work by looking and copying the file that was already included with RE-UE4SS
*/
#pragma once

#include <dllmain.h>


// All these include are really necessary!
// From here
#include <Unreal/UClass.hpp>
#include <Unreal/UFunction.hpp>
#include <Unreal/BPMacros.hpp>
// To here


#include <FText.hpp>

// If you remove these, don't forget to add the RC:: and RC::Unreal::
using namespace RC;
using namespace RC::Unreal;

// a little information but that make me crazy before I found it:
// If you don't type the class exactly as Unreal Engine has it, then all you're class will not work 
// (has long has you use it to call a native function, but.. well we create this file for that after all so..)
class MY_AWESOME_MOD_API TextBlock : public UObject
{
private:
    friend class CustomMod;
    DECLARE_EXTERNAL_OBJECT_CLASS(TextBlock, UMG)
public:
    // Important but not required: If it's a function that will be used for a native function, use the same name has Unreal Engine use
    // 
    // /!\ REALLY IMPORTANT: Use the SAME type has Unreal use, in the arguments and in the return
    // If it doesn't exist you have two choice: Create it if you can, or pray that someone, some where create and give it
    auto SetText(FText InText) -> void;
};