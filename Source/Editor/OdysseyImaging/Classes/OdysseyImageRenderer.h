// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Misc/OdysseyHandle.h"
#include <ULIS>

class ODYSSEYIMAGING_API IOdysseyImageRenderer
{
public:
    enum class eRenderType
    {
        Render, //renders only the expected final render result
        Editor //renders what is expected in an editor (can render the animation lighttable for example)
    };

    virtual ~IOdysseyImageRenderer() {};
    IOdysseyImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, const TArray<::ULIS::FRectI>& iDefaultRects);

public:
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
    IOdysseyImageRenderer::eRenderType GetRenderType() const;
    void AddHandle(TSharedPtr<IOdysseyHandle> iHandle);

private:
    IOdysseyImageRenderer::eRenderType mRenderType;
    TArray<TSharedPtr<IOdysseyHandle>> mHandles;
    TArray<::ULIS::FRectI> mDefaultRects;
};
