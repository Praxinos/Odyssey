// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include <ULIS>

namespace ULISUtils
{
    typedef TFunction<TArray<::ULIS::FEvent>(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>, const ::ULIS::FRectI&, const ::ULIS::FVec2I&, const TArray<::ULIS::FEvent>&)> tConvertAndExecuteFunction;

    /**
     * @brief Converts the destination block to the given format if needed, keeing only the given rect and execute the goven function on it
     * 
     * @param ioDest 
     * @param iFormat 
     * @param iRect 
     * @param iPos 
     * @param oWaitList 
     * @param iNumWaitList 
     * @param iFunction 
     * @return ::ULIS::FEvent 
     */
    ULISLOADER_API TArray<::ULIS::FEvent> ConvertAndExecute(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioDest, ::ULIS::eFormat iFormat, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList, tConvertAndExecuteFunction iFunction);
}