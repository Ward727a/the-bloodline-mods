/*
	This file is needed so we can use the native UE function of the TextBox class
	This file is to put inside the "src" filter in the "Unreal" solution in your VS

	Don't ask too much of what do what, I just look and tried has much as I can to make
	this thing work by looking and copying the file that was already included with RE-UE4SS
*/
#include "UTextBlock.h"
#include <Unreal/UClass.hpp>
#include <FText.hpp>
#include <Unreal/UFunction.hpp>

#include <Unreal/BPMacros.hpp>

// This is really important:
// Type all of this, IDK why but if you don't type this perfectly it's doesn't work
namespace RC::Unreal {
	// Needed, don't ask, put this, IDK why, it was in each file of RE-UE4SS
	IMPLEMENT_EXTERNAL_OBJECT_CLASS(TextBlock)

		auto TextBlock::SetText(FText InText) -> void
	{
		// We start to prepare the function that we will use (you can find the path of the function in the GUI Console->live-viewer)
		UE_BEGIN_NATIVE_FUNCTION_BODY("/Script/UMG.TextBlock:SetText")

		// Then we set the first arguments of the function
		UE_COPY_PROPERTY(InText, FText)

		// And we call the function that we prepared
		UE_CALL_FUNCTION()
	}
}