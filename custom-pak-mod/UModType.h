#pragma once

#include <dllmain.h>

#include <UPanelWidget.h>
#include <Unreal/UClass.hpp>
#include <Unreal/UFunction.hpp>
#include <Unreal/BPMacros.hpp>
#include <UCanvasPanelSlot.h>
#include <Unreal/AActor.hpp>

using namespace RC;
using namespace RC::Unreal;


class MY_AWESOME_MOD_API ModType : public AActor
{
private:
    friend class CustomMod;
public:
    auto TestFunction() -> void;
    auto AddModToList(FString modName, FString modVersion) -> void;
};

