// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Misc/OdysseyHandle.h"
#include <ULIS>

class ODYSSEYIMAGING_API IOdysseyImageRenderer
{
public:
    virtual ~IOdysseyImageRenderer() {};

public:
    /**
     * @brief Returns the full rect that can be rendered
     * 
     * @return ::ULIS::FRect 
     */
    virtual TArray<::ULIS::FRectI> GetRects() const = 0;
    
    /**
     * @brief Renders over (by blending for example) the given block
     * By default does the same thing as RenderInBlock
     *
     * @param ioBlock
     * @param iRect
     * @param iPos
     * @param iWaitList
     * @return TArray<::ULIS::FEvent>
     */
    virtual TArray<::ULIS::FEvent> RenderOverBlock(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList);
    TArray<::ULIS::FEvent> RenderOverBlock(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FEvent>& iWaitList);
    TArray<::ULIS::FEvent> RenderOverBlock(TSharedPtr<::ULIS::FBlock> ioBlock, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList);
    TArray<::ULIS::FEvent> RenderOverBlock(TSharedPtr<::ULIS::FBlock> ioBlock, const ::ULIS::FRectI& iRect, const TArray<::ULIS::FEvent>& iWaitList);
    TArray<::ULIS::FEvent> RenderOverBlock(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FEvent>& iWaitList);

    /**
     * @brief Renders in (without blending for example) the given block
     *
     * @param ioBlock
     * @param iRect
     * @param iPos
     * @param iWaitList
     * @return TArray<::ULIS::FEvent>
     */
    virtual TArray<::ULIS::FEvent> RenderInBlock(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FVec2I>& iPos, const TArray<::ULIS::FEvent>& iWaitList) = 0;
    TArray<::ULIS::FEvent> RenderInBlock(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FRectI>& iRects, const TArray<::ULIS::FEvent>& iWaitList);
    TArray<::ULIS::FEvent> RenderInBlock(TSharedPtr<::ULIS::FBlock> ioBlock, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList);
    TArray<::ULIS::FEvent> RenderInBlock(TSharedPtr<::ULIS::FBlock> ioBlock, const ::ULIS::FRectI& iRect, const TArray<::ULIS::FEvent>& iWaitList);
    TArray<::ULIS::FEvent> RenderInBlock(TSharedPtr<::ULIS::FBlock> ioBlock, const TArray<::ULIS::FEvent>& iWaitList);

    /**
     * @brief Renders a new block
     *
     * @param iRect
     * @param oEvents
     * @return TSharedPtr<::ULIS::FBlock>
     */
    TSharedPtr<::ULIS::FBlock> RenderInNewBlock(::ULIS::eFormat iFormat, const ::ULIS::FRectI& iRect, TArray<::ULIS::FEvent>& oEvents);

public:
    void AddHandle(TSharedPtr<IOdysseyHandle> iHandle);

private:
    TArray<TSharedPtr<IOdysseyHandle>> mHandles;
};
