/*
	This file is needed so we can use the native UE function of the TextBox class

	Don't ask too much of what do what, I just look and tried has much as I can to make
	this thing work by looking and copying the file that was already included with RE-UE4SS
*/

#include "UTextBlock.h"

#include <FText.hpp>

// This is really important:
// Type this, it will import some function that is really important!
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