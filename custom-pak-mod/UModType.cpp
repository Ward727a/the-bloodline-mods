#include "UModType.h"


void ModType::TestFunction()
{
    UE_BEGIN_NATIVE_FUNCTION_BODY("/Game/Mods/MyAwesomeMod/ModActor.ModActor_C:TestFunction");

    //UE_COPY_PROPERTY(text, FText);

    UE_CALL_FUNCTION();
}

auto ModType::AddModToList(FString TitleMod, FString VersionMod) -> void
{
	UE_BEGIN_NATIVE_FUNCTION_BODY("/Game/Mods/MyAwesomeMod/ModActor.ModActor_C:AddModToList");

	UE_COPY_PROPERTY(TitleMod, FString);
	UE_COPY_PROPERTY(VersionMod, FString);

	UE_CALL_FUNCTION();
}
