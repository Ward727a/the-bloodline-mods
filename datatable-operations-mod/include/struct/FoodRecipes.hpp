#pragma once
#include <UDataTable.hpp>
#include <AActor.hpp>
#include <TObjectPtr.hpp>

using namespace RC::Unreal;


// This is a struct that represents a row in the FoodRecipes datatable
// For now it's a test to see if I can convert the ptr returned by the function in UDataTable.hpp to this structure
// --
// Well, it's not a test anymore, it's a total failure, the size of the struct is 0x2C, but the size of the ptr returned by the function is 0x20
// The "RequiredItems" array is the problem, it's a TArray of size 0x16, but the ptr returned by the function is of size 0x10
// --
// To fix this I need to create a variable of this structure and use the function to get the ptr, 
// then I can use the ptr to access each data member then assign it to the variable member by member
struct FFoodRecipes : public FTableRowBase
{
	FName RecipeName; // 0x0000 size: 0x8
	TArray<AActor> RequiredItems; // 0x0008 size: 0x16
	TSubclassOf<AActor> CompletedMeal; // 0x0020 size: 0x8
	int32 EXPReward; // 0x0028 size: 0x4
}; // Size: 0x2C