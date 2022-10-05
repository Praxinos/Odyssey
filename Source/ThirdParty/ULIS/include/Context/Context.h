// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         Context.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the FContext class.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Image/Color.h"
#include "Image/Pixel.h"
#include "Image/Sample.h"
#include "Image/Block.h"
#include "Image/Gradient.h"
#include "Layer/Layer/LayerTypedefs.h"
#include "Math/Geometry/Rectangle.h"
#include "Math/Geometry/Vector.h"
#include "Scheduling/SchedulePolicy.h"
#include "System/CPUInfo/CPUInfo.h"
#include "Sparse/TiledBlock.h"
#include <functional>

ULIS_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @class      FContext
/// @brief      The FContext class provides a monolithic context for
///             calling image raster and composition functions for a given format.
/// @details    FContext works on FBlock and is bound to a specific format.
///             It prefetches dispatched implementation to select the fastest
///             method according to the format, its optional  specializations,
///             and the runtime device support for multi-threading or SIMD.
///
///             There can be multiple FContext instances for different formats.
///             Keep in mind this object is rather heavy as it caches paths to
///             the implementations of each feature it exposes.
///
///             FContext can be expected to reach a size in the Ko magnitude,
///             there is a small overhead during instanciation at runtime.
///
///             \sa FBlock
///             \sa FThreadPool
///             \sa FCPUInfo
///             \sa FCommandQueue
class ULIS_API FContext
{
private:
    struct FContextualDispatchTable;

public:
    /*!
        Destructor. Clean up the contextual dispatch table.
    */
    ~FContext();

    /*!
        Constructor. Builds the contextual dispatch table. Construction has
        significant overhead, preferably construct a context once at the beginning.
        A queue can be shared beetween multiple contexts with no issue.
    */
    FContext(
          FCommandQueue& iQueue
        , eFormat iFormat
        , ePerformanceIntent iPerfIntent = ePerformanceIntent::PerformanceIntent_Max
    );

    FContext( const FContext& ) = delete;
    FContext& operator=( const FContext& ) = delete;

public:
/////////////////////////////////////////////////////
// Control Flow
    /*!
        Issue all commands and return immediately.
    */
    void Flush();

    /*!
        Issue all commands and wait for completion
    */
    void Finish();

    /*!
        Wait for completion of all already issued commands
    */
    void Fence();

    /*!
        Wait for completion of a given event.

        \warning The event must be associated to a flushed command, otherwise
        the program will block forever in a loop.
    */
    void Wait( const FEvent& iEvent );

    /*!
        Getter for the context format
    */
    eFormat Format() const;

public:
/////////////////////////////////////////////////////
// Utils
    /*!
        Internal tool for notifying an user event the task is a no-op
    */
    ulError FinishEventNo_OP( uint32 iNumWait, const FEvent* iWaitList, FEvent* iEvent, ulError iError );

    /*!
        Utility tool for a dummy task with an asynchronous time constraint
    */
    ulError Dummy_OP( uint32 iNumWait, const FEvent* iWaitList, FEvent* iEvent );

    /*!
        Utility tool to mark an event finished without doing any work at all
    */
    static void MarkEventFinished( FEvent* iEvent );

/////////////////////////////////////////////////////
// Layers
    ///*!
    //    Perform a flattened blend of the whole layer stack
    //*/
    //ulError
    //Flatten(
    //      FLayerStack& iStack
    //    , FBlock& iDestination
    //    , const FRectI& iSourceRect = FRectI::Auto
    //    , const FVec2I& iPosition = FVec2I( 0 )
    //    , const FSchedulePolicy& iPolicy = FSchedulePolicy::MultiScanlines
    //    , uint32 iNumWait = 0
    //    , const FEvent* iWaitList = nullptr
    //    , FEvent* iEvent = nullptr
    //);
    //
    //ulError
    //RenderLayerFolder(
    //      FLayerFolder& iFolder
    //    , FEvent* iEvent = nullptr
    //);
    //
    //ulError
    //RenderLayerText(
    //      FLayerText& iText
    //    , FEvent* iEvent = nullptr
    //);

    /*!
        Collect metrics before a XLoadPSDFromDisk call
    */
    static
    void
    LoadPSDFromDiskMetrics(
        const std::string& iPath
        , bool *oFileExists
        , eFormat *oFormat
    );

    /*!
        Perform a load of a PSD file in a layer stack
    */
    ulError
    XLoadPSDFromDisk(
          FLayerStack& iStack
        , const std::string& iPath
        , const FRectI& iSourceRect = FRectI::Auto
        , const FVec2I& iPosition = FVec2I( 0 )
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MultiScanlines
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

/////////////////////////////////////////////////////
// Gradient
    /*!
        Raster a linear gradient on the destination block.
        The iDithering parameter is a normal param clamped beetween 0 and 1.
        0: 0%
        1: 100%
    */
    ulError
    RasterGradient(
          FBlock& iBlock
        , const FVec2F& iStart
        , const FVec2F& iEnd
        , const FSanitizedGradient& iGradient
        , float iDithering = 0.f
        , eGradientType iType = eGradientType::Gradient_Linear
        , const FRectI& iRect = FRectI::Auto
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MultiScanlines
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

/////////////////////////////////////////////////////
// Tile Block Specific
    /*!
        Dump tiled block into regular block.
    */
    ulError
    Dump(
          const FTiledBlock& iSource
        , FBlock& iDestination
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MonoChunk
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

/////////////////////////////////////////////////////
// Blend
    /*!
        Perform a blend operation with iSource composited on top of iBackdrop.
        iBackdrop is modified to receive the result of the operation, while
        iSource is left untouched.

        You can specify a sub-portion of the iSource image by specifying the
        iSourceRect to the desired part of the picture. If you want to blend the
        whole image, use the FBlock::Rect() method on the iSource block.
        You can also specify where in iBackdrop the iSource FBlock should be
        composited, in integer coordinates.

        If the iSourceRect and/or iPosition lead to a destination geometry that
        does not intersect the rectangular geometry of iBackdrop, the call will
        not perform any computation and will return safely, so it is safe to
        specify out-of-bounds positions.

        \sa BlendAA()
    */
    ulError
    Blend(
          const FBlock& iSource
        , FBlock& iBackdrop
        , const FRectI& iSourceRect = FRectI::Auto
        , const FVec2I& iPosition = FVec2I( 0 )
        , eBlendMode iBlendingMode = Blend_Normal
        , eAlphaMode iAlphaMode = Alpha_Normal
        , ufloat iOpacity = 1.0f
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MultiScanlines
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Perform a blend operation with iSource composited on top of iBackdrop.
        iBackdrop is modified to receive the result of the operation, while
        iSource is left untouched.

        You can specify a sub-portion of the iSource image by specifying the
        iSourceRect to the desired part of the picture. If you want to blend the
        whole image, use the FBlock::Rect() method on the iSource block.
        You can also specify where in iBackdrop the iSource FBlock should be
        composited, in integer coordinates.

        If the iSourceRect and/or iPosition lead to a destination geometry that
        does not intersect the rectangular geometry of iBackdrop, the call will
        not perform any computation and will return safely, so it is safe to
        specify out-of-bounds positions.

        \sa BlendAA()
    */
    ulError
    Blend(
          const FTiledBlock& iSource
        , FTiledBlock& iBackdrop
        , const FRectI& iSourceRect = FRectI::Auto
        , eBlendMode iBlendingMode = Blend_Normal
        , eAlphaMode iAlphaMode = Alpha_Normal
        , ufloat iOpacity = 1.0f
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MonoChunk
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Perform a blend operation with iSource composited on top of iBackdrop.
        iBackdrop is modified to receive the result of the operation, while
        iSource is left untouched.

        You can specify a sub-portion of the iSource image by specifying the
        iSourceRect to the desired part of the picture. If you want to blend the
        whole image, use the FBlock::Rect() method on the iSource block.
        You can also specify where in iBackdrop the iSource FBlock should be
        composited, in integer coordinates.

        If the iSourceRect and/or iPosition lead to a destination geometry that
        does not intersect the rectangular geometry of iBackdrop, the call will
        not perform any computation and will return safely, so it is safe to
        specify out-of-bounds positions.

        \sa BlendAA()
    */
    ulError
    Blend(
          const FTiledBlock& iSource
        , FTiledBlock& iBackdrop
        , const FRectI& iSourceRect = FRectI::Auto
        , const FVec2I& iPosition = FVec2I( 0 )
        , eBlendMode iBlendingMode = Blend_Normal
        , eAlphaMode iAlphaMode = Alpha_Normal
        , ufloat iOpacity = 1.0f
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MonoChunk
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Blend a bucket of the same source at different positions, with potential
        concurrency so an event dependency graph is automatically generated.
        May be used for particles drawing.
        There will be as many blends as there are elements in the positions array.

        \sa Blend()
    */
    ulError
    BlendBucket(
          const FBlock& iSource
        , FBlock& iBackdrop
        , const FRectI& iSourceRect = FRectI::Auto
        , const TArray< FVec2I >& iPosition = TArray< FVec2I >( 1 )
        , eBlendMode iBlendingMode = Blend_Normal
        , eAlphaMode iAlphaMode = Alpha_Normal
        , ufloat iOpacity = 1.0f
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MultiScanlines
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Perform an antialiased blend operation with iSource composited on top of
        iBackdrop. iBackdrop is modified to receive the result of the operation,
        while iSource is left untouched.

        An antialiasing blend accepts floating point coordinates and performs
        antialiasing at the same time as it performs the compositing, this is
        useful for drawing, sometimes also referred to as subpixel blending.

        You can specify a sub-portion of the iSource image by specifying the
        iSourceRect to the desired part of the picture. If you want to blend the
        whole image, use the FBlock::Rect() method on the iSource block.
        You can also specify where in iBackdrop the iSource FBlock should be
        composited, in floating point coordinates.

        If the iSourceRect and/or iPosition lead to a destination geometry that
        does not intersect the rectangular geometry of iBackdrop, the call will
        not perform any computation and will return safely, so it is safe to
        specify out-of-bounds positions.

        \sa Blend()
    */
    ulError
    BlendAA(
          const FBlock& iSource
        , FBlock& iBackdrop
        , const FRectI& iSourceRect = FRectI::Auto
        , const FVec2F& iPosition = FVec2F( 0.f, 0.f )
        , eBlendMode iBlendingMode = Blend_Normal
        , eAlphaMode iAlphaMode = Alpha_Normal
        , ufloat iOpacity = 1.0f
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MultiScanlines
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Perform an alpha blend operation with iSource composited on top of
        iBackdrop. iBackdrop is modified to receive the result of the
        operation, while iSource is left untouched.

        You can specify a sub-portion of the iSource image by specifying the
        iSourceRect to the desired part of the picture. If you want to blend the
        whole image, use the FBlock::Rect() method on the iSource block.
        You can also specify where in iBackdrop the iSource FBlock should be
        composited, in integer coordinates.

        If the iSourceRect and/or iPosition lead to a destination geometry that
        does not intersect the rectangular geometry of iBackdrop, the call will
        not perform any computation and will return safely, so it is safe to
        specify out-of-bounds positions.

        \sa AlphaBlendAA()
        \sa Blend()
    */
    ulError
    AlphaBlend(
          const FBlock& iSource
        , FBlock& iBackdrop
        , const FRectI& iSourceRect = FRectI::Auto
        , const FVec2I& iPosition = FVec2I( 0 )
        , ufloat iOpacity = 1.0f
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MultiScanlines
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Perform an antialiased alpha blend operation with iSource composited on
        top of iBackdrop. iBackdrop is modified to receive the result of the
        operation, while iSource is left untouched.

        An antialiasing blend accepts floating point coordinates and performs
        antialiasing at the same time as it performs the compositing, this is
        useful for drawing, sometimes also referred to as subpixel blending.

        You can specify a sub-portion of the iSource image by specifying the
        iSourceRect to the desired part of the picture. If you want to blend the
        whole image, use the FBlock::Rect() method on the iSource block.
        You can also specify where in iBackdrop the iSource FBlock should be
        composited, in floating point coordinates.

        If the iSourceRect and/or iPosition lead to a destination geometry that
        does not intersect the rectangular geometry of iBackdrop, the call will
        not perform any computation and will return safely, so it is safe to
        specify out-of-bounds positions.

        \sa AlphaBlend()
        \sa BlendAA()
    */
    ulError
    AlphaBlendAA(
          const FBlock& iSource
        , FBlock& iBackdrop
        , const FRectI& iSourceRect = FRectI::Auto
        , const FVec2F& iPosition = FVec2F( 0.f, 0.f )
        , ufloat iOpacity = 1.0f
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MultiScanlines
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Perform a tiled blend operation with iSource composited on top of
        iBackdrop. iBackdrop is modified to receive the result of the
        operation, while iSource is left untouched. iSource is tiled over the
        specified surface to cover iBackdrop.

        You can specify a sub-portion of the iSource image by specifying the
        iSourceRect to the desired part of the picture. If you want to blend the
        whole image, use the FBlock::Rect() method on the iSource block.
        You can also specify where in iBackdrop the iSource FBlock should be
        composited, in integer coordinates.

        If the iSourceRect and/or iPosition lead to a destination geometry that
        does not intersect the rectangular geometry of iBackdrop, the call will
        not perform any computation and will return safely, so it is safe to
        specify out-of-bounds positions.

        \sa Blend()
    */
    ulError
    BlendTiled(
          const FBlock& iSource
        , FBlock& iBackdrop
        , const FRectI& iSourceRect = FRectI::Auto
        , const FRectI& iBackdropRect = FRectI::Auto
        , const FVec2I& iPosition = FVec2I( 0 )
        , eBlendMode iBlendingMode = Blend_Normal
        , eAlphaMode iAlphaMode = Alpha_Normal
        , ufloat iOpacity = 1.0f
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MultiScanlines
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Perform a blend operation of a single plain color composited on top of
        iBackdrop. iBackdrop is modified to receive the result of the
        operation, while iColor is left untouched.

        If the specified rect and/or position lead to a destination geometry
        that does not intersect the rectangular geometry of iBackdrop, the call
        will not perform any computation and will return safely, so it is safe
        to specify out-of-bounds positions.

        \sa BlendTiled()
    */
    ulError
    BlendColor(
          const ISample& iColor
        , FBlock& iBackdrop
        , const FRectI& iBackdropRect = FRectI::Auto
        , eBlendMode iBlendingMode = Blend_Normal
        , eAlphaMode iAlphaMode = Alpha_Normal
        , ufloat iOpacity = 1.0f
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MultiScanlines
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

/////////////////////////////////////////////////////
// Clear
    /*!
        Perform a clear operation on the input block.
        iBlock is modified to be cleared.

        You can specify a sub-portion of the iBlock image by specifying the
        iRect to the desired part of the picture. If you want to clear the
        whole image, use the FBlock::Rect() method on the iBlock.

        If the iRect lead to a destination geometry that does not intersect the
        rectangular geometry of iBlock, the call will not perform any
        computation and will return safely, so it is safe to specify
        out-of-bounds positions.
    */
    ulError
    Clear(
          FBlock& iBlock
        , const FRectI& iRect = FRectI::Auto
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::CacheEfficient
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Perform a clear operation on the input block.
        iBlock is modified to be cleared.

        You can specify a sub-portion of the iBlock image by specifying the
        iRect to the desired part of the picture. If you want to clear the
        whole image, use the FBlock::Rect() method on the iBlock.

        If the iRect lead to a destination geometry that does not intersect the
        rectangular geometry of iBlock, the call will not perform any
        computation and will return safely, so it is safe to specify
        out-of-bounds positions.
    */
    ulError
    Clear(
          FTiledBlock& iBlock
        , const FRectI& iRect = FRectI::Auto
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MonoChunk
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

/////////////////////////////////////////////////////
// Conv
    /*!
        Perform a conv operation with iSource converted into iDestination.
        iDestination is modified to receive the result of the operation, while
        iSource is left untouched.

        You can specify a sub-portion of the iSource image by specifying the
        iSourceRect to the desired part of the picture. If you want to convert
        the whole image, use the FBlock::Rect() method on the iSource block.
        You can also specify where in iDestination the iSource FBlock should be
        converted, in integer coordinates.

        If the iSourceRect and/or iPosition lead to a destination geometry that
        does not intersect the rectangular geometry of iDestination, the call will
        not perform any computation and will return safely, so it is safe to
        specify out-of-bounds positions.
    */
    ulError
    ConvertFormat(
          const FBlock& iSource
        , FBlock& iDestination
        , const FRectI& iSourceRect = FRectI::Auto
        , const FVec2I& iPosition = FVec2I( 0 )
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MultiScanlines
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

/////////////////////////////////////////////////////
// Copy
    /*!
        Perform a copy operation with iSource copied into iDestination.
        iDestination is modified to receive the result of the operation, while
        iSource is left untouched.

        You can specify a sub-portion of the iSource image by specifying the
        iSourceRect to the desired part of the picture. If you want to convert
        the whole image, use the FBlock::Rect() method on the iSource block.
        You can also specify where in iDestination the iSource FBlock should be
        copied, in integer coordinates.

        If the iSourceRect and/or iPosition lead to a destination geometry that
        does not intersect the rectangular geometry of iDestination, the call will
        not perform any computation and will return safely, so it is safe to
        specify out-of-bounds positions.
    */
    ulError
    Copy(
          const FBlock& iSource
        , FBlock& iDestination
        , const FRectI& iSourceRect = FRectI::Auto
        , const FVec2I& iPosition = FVec2I( 0 )
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::CacheEfficient
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

/////////////////////////////////////////////////////
// Fill
    /*!
        Perform a fill operation in iBlock.
        iBlock is modified to receive the result of the operation.

        You can specify a sub-portion of the iBlock image by specifying the
        iRect to the desired part of the picture. If you want to fill
        the whole image, use the FBlock::Rect() method on the iBlock.

        If the iRect lead to a destination geometry that does not intersect the
        rectangular geometry of iBlock, the call will not perform any
        computation and will return safely, so it is safe to specify
        out-of-bounds positions.
    */
    ulError
    Fill(
          FBlock& iBlock
        , const ISample& iColor = FColor::RGBA8( 0, 0, 0 )
        , const FRectI& iRect = FRectI::Auto
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MonoChunk
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Perform a fill operation in iBlock.
        iBlock is modified to receive the result of the operation.

        You can specify a sub-portion of the iBlock image by specifying the
        iRect to the desired part of the picture. If you want to fill
        the whole image, use the FBlock::Rect() method on the iBlock.

        If the iRect lead to a destination geometry that does not intersect the
        rectangular geometry of iBlock, the call will not perform any
        computation and will return safely, so it is safe to specify
        out-of-bounds positions.
    */
    ulError
    Fill(
          FTiledBlock& iBlock
        , const ISample& iColor = FColor::RGBA8( 0, 0, 0 )
        , const FRectI& iRect = FRectI::Auto
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::CacheEfficient
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Perform a fill operation in iBlock while preserving alpha.
        The color will change but the alpha will remain intact.
        iBlock is modified to receive the result of the operation.

        You can specify a sub-portion of the iBlock image by specifying the
        iRect to the desired part of the picture. If you want to fill
        the whole image, use the FBlock::Rect() method on the iBlock.

        If the iRect lead to a destination geometry that does not intersect the
        rectangular geometry of iBlock, the call will not perform any
        computation and will return safely, so it is safe to specify
        out-of-bounds positions.
    */
    ulError
    FillPreserveAlpha(
          FBlock& iBlock
        , const ISample& iColor = FColor::RGBA8( 0, 0, 0 )
        , const FRectI& iRect = FRectI::Auto
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::CacheEfficient
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

/////////////////////////////////////////////////////
// Text
    /*!
        Perform a text raster operation in iBlock.
        iBlock is modified to receive the result of the operation.

        If the text leads to a destination geometry that does not intersect the
        rectangular geometry of iBlock, the call will not perform any
        computation and will return safely. It is also safe to specify
        out-of-bounds positions, but the text can be cropped.

        You can use TextMetrics() in order to retrieve the geometry before drawing.

        \warning the FFont input object must remain valid until the async command
        is complete. The other input objects don't have such constraints.

        \warning The same FFont input object cannot be used by multiple threads,
        so you cannot issue two concurrent RasterText commands with the same
        Font at the same time, you have to ensure a Fence or a wait event.

        \sa RasterTextAA()
        \sa TextMetrics()
    */
    ulError
    RasterText(
          FBlock& iBlock
        , const FWString& iText
        , const FFont& iFont
        , uint32 iFontSize = 12
        , const FMat3F& iTransform = FMat3F()
        , const ISample& iColor = FColor::RGBA8( 0, 0, 0 )
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MonoChunk
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Perform a text raster operation in iBlock with text antialiasing.
        iBlock is modified to receive the result of the operation.

        If the text leads to a destination geometry that does not intersect the
        rectangular geometry of iBlock, the call will not perform any
        computation and will return safely. It is also safe to specify
        out-of-bounds positions, but the text can be cropped.

        You can use TextMetrics() in order to retrieve the geometry before drawing.

        \warning the FFont input object must remain valid until the async command
        is complete. The other input objects don't have such constraints.

        \warning The same FFont input object cannot be used by multiple threads,
        so you cannot issue two concurrent RasterText commands with the same
        Font at the same time, you have to ensure a Fence or a wait event.

        \sa RasterText()
        \sa TextMetrics()
    */
    ulError
    RasterTextAA(
          FBlock& iBlock
        , const FWString& iText
        , const FFont& iFont
        , uint32 iFontSize = 12
        , const FMat3F& iTransform = FMat3F()
        , const ISample& iColor = FColor::RGBA8( 0, 0, 0 )
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MonoChunk
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Collect text metrics before a text raster operation.
        This can help you position your text geometry beforehand.
        Unlike the other methods of FContext, this doesn't work on images and
        returns immediately, it is not scheduled in the Queue.

        \sa RasterText()
        \sa TextMetrics()
    */
    static
    FRectI
    TextMetrics(
          const FWString& iText
        , const FFont& iFont
        , uint32 iFontSize = 12
        , const FMat3F& iTransform = FMat3F()
    );

/////////////////////////////////////////////////////
// Transform
    /*!
        Perform an affine transform operation using iSource as input. The result
        is written in the iDestination block.

        An affine transform is a linear transformation such as a translation,
        scale, rotation, skew, or any combination of these.

        You can use TransformAffineMetrics() in order to compute the geometry of
        the transformed output before actually scheduling the operation.

        You can specify a sub-rectangular area in the source input with the
        iSourceRect input parameter.

        \sa TransformAffine()
        \sa TransformPerspective()
        \sa TransformBezier()
        \sa TransformAffineMetrics()
        \sa TransformPerspectiveMetrics()
        \sa TransformBezierMetrics()
    */
    ulError
    TransformAffine(
          const FBlock& iSource
        , FBlock& iDestination
        , const FRectI& iSourceRect = FRectI::Auto
        , const FMat3F& iTransformMatrix = FMat3F()
        , eResamplingMethod iResamplingMethod = eResamplingMethod::Resampling_Bilinear
        , eBorderMode iBorderMode = eBorderMode::Border_Transparent
        , const ISample& iBorderValue = FColor::RGBA8( 0, 0, 0 )
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MultiScanlines
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Perform an affine transform operation using iSource as input. The result
        is written in the iDestination block.

        An affine transform is a linear transformation such as a translation,
        scale, rotation, skew, or any combination of these.

        You can use TransformAffineMetrics() in order to compute the geometry of
        the transformed output before actually scheduling the operation.

        You can specify a sub-rectangular area in the source input with the
        iSourceRect input parameter.

        This tiled version allows to cover the area specified by iDestinationRect
        by repeating the transformed source in a pattern inside the destination.

        \sa TransformAffine()
        \sa TransformPerspective()
        \sa TransformBezier()
        \sa TransformAffineMetrics()
        \sa TransformPerspectiveMetrics()
        \sa TransformBezierMetrics()
    */
    ulError
    TransformAffineTiled(
          const FBlock& iSource
        , FBlock& iDestination
        , const FRectI& iSourceRect = FRectI::Auto
        , const FRectI& iDestinationRect = FRectI::Auto
        , const FMat3F& iTransformMatrix = FMat3F()
        , eResamplingMethod iResamplingMethod = eResamplingMethod::Resampling_Bilinear
        , eBorderMode iBorderMode = eBorderMode::Border_Transparent
        , const ISample& iBorderValue = FColor::RGBA8( 0, 0, 0 )
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MultiScanlines
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Perform an homography or perspective transform operation using iSource
        as input. The result is written in the iDestination block.

        An homography is a distortion transform that doesn't necessarilly preserve
        features such as parallel lines.

        You can use TransformPerspectiveMetrics() in order to compute the geometry of
        the transformed output before actually scheduling the operation.

        You can specify a sub-rectangular area in the source input with the
        iSourceRect input parameter.

        \warning The input iTransformMatrix should be built from the
        FMat3F::MakeHomography() method, an homography matrix differs from an
        affine transform matrix, despite both being stored in a 3x3 matrix.

        \sa TransformAffine()
        \sa TransformPerspective()
        \sa TransformBezier()
        \sa TransformAffineMetrics()
        \sa TransformPerspectiveMetrics()
        \sa TransformBezierMetrics()
    */
    ulError
    TransformPerspective(
          const FBlock& iSource
        , FBlock& iDestination
        , const FRectI& iSourceRect = FRectI::Auto
        , const FMat3F& iTransformMatrix = FMat3F()
        , eResamplingMethod iResamplingMethod = eResamplingMethod::Resampling_Bilinear
        , eBorderMode iBorderMode = eBorderMode::Border_Transparent
        , const ISample& iBorderValue = FColor::RGBA8( 0, 0, 0 )
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MultiScanlines
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Perform a bezier transform operation using iSource as input. The result
        is written in the iDestination block.

        A bezier transform is a distortion of an input source covering a 2D bezier
        surface shape. Think about a flag.

        You can use TransformBezierMetrics() in order to compute the geometry of
        the transformed output before actually scheduling the operation.

        You can specify a sub-rectangular area in the source input with the
        iSourceRect input parameter.

        \warning The input iControlPoints array should have size 4 and control
        points define the corners of a bezier path with four control points,
        each one of them using two tangents control points. Two adjacent
        control points define a cubic bezier curve.

        \sa TransformAffine()
        \sa TransformPerspective()
        \sa TransformBezier()
        \sa TransformAffineMetrics()
        \sa TransformPerspectiveMetrics()
        \sa TransformBezierMetrics()
    */
    ulError
    TransformBezier(
          const FBlock& iSource
        , FBlock& iDestination
        , const TArray< FCubicBezierControlPoint >& iControlPoints
        , float iThreshold = 1.f
        , uint32 iPlotSize = 1
        , const FRectI& iSourceRect = FRectI::Auto
        , eResamplingMethod iResamplingMethod = eResamplingMethod::Resampling_Bilinear
        , eBorderMode iBorderMode = eBorderMode::Border_Transparent
        , const ISample& iBorderValue = FColor::RGBA8( 0, 0, 0 )
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MultiScanlines
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Perform a resize operation using iSource as input. The result
        is written in the iDestination block.

        A resize is simply a scale of an input source.

        You can specify a sub-rectangular area in the source input with the
        iSourceRect input parameter.

        It is advised to use Resize() instead of TransformAffine() when you only
        need a rescale, because the implementation can be made faster by relying
        on this assumption. This is also the only method that can handle the
        more advanced resampling methods, such as eResamplingMethod::Resampling_Area.

        \sa TransformAffine()
        \sa TransformPerspective()
        \sa TransformBezier()
        \sa TransformAffineMetrics()
        \sa TransformPerspectiveMetrics()
        \sa TransformBezierMetrics()
    */
    ulError
    Resize(
          const FBlock& iSource
        , FBlock& iDestination
        , const FRectI& iSourceRect = FRectI::Auto
        , const FRectF& iDestinationRect = FRectF::Auto
        , eResamplingMethod iResamplingMethod = eResamplingMethod::Resampling_Bilinear
        , eBorderMode iBorderMode = eBorderMode::Border_Transparent
        , const ISample& iBorderValue = FColor::RGBA8( 0, 0, 0 )
        , const FBlock* iOptionalSummedAreaTable = nullptr
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MultiScanlines
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Collect metrics before a TransformAffine() operation.

        \sa TransformAffine()
        \sa TransformPerspective()
        \sa TransformBezier()
        \sa TransformAffineMetrics()
        \sa TransformPerspectiveMetrics()
        \sa TransformBezierMetrics()
    */
    static
    FRectI
    TransformAffineMetrics(
          const FRectI& iSourceRect
        , const FMat3F& iTransform
    );

    /*!
        Collect metrics before a TransformPerspective() operation.

        \sa TransformAffine()
        \sa TransformPerspective()
        \sa TransformBezier()
        \sa TransformAffineMetrics()
        \sa TransformPerspectiveMetrics()
        \sa TransformBezierMetrics()
    */
    static
    FRectI
    TransformPerspectiveMetrics(
          const FRectI& iSourceRect
        , const FMat3F& iTransform
    );

    /*!
        Collect metrics before a TransformBezier() operation.

        \sa TransformAffine()
        \sa TransformPerspective()
        \sa TransformBezier()
        \sa TransformAffineMetrics()
        \sa TransformPerspectiveMetrics()
        \sa TransformBezierMetrics()
    */
    static
    FRectI
    TransformBezierMetrics(
          const FRectI& iSourceRect
        , const TArray< FCubicBezierControlPoint >& iControlPoints
    );

    /*!
        Get metrics before building a bezier displacement field, this will give
        you the needed format before building.
        same size.
    */
    static
    eFormat BezierDisplacementFieldMetrics( const FBlock& iSource );

    /*!
        Get metrics before building a bezier displacement mask, this will give
        you the needed format before building.
        same size.
    */
    static
    eFormat BezierDisplacementMaskMetrics( const FBlock& iSource );

    /*!
        Perform a bezier transform operation using iSource as input. The result
        is written in the iDestination block.

        A bezier transform is a distortion of an input source covering a 2D bezier
        surface shape. Think about a flag.

        You can use TransformBezierMetrics() in order to compute the geometry of
        the transformed output before actually scheduling the operation.

        You can specify a sub-rectangular area in the source input with the
        iSourceRect input parameter.

        \warning The input iControlPoints array should have size 4 and control
        points define the corners of a bezier path with four control points,
        each one of them using two tangents control points. Two adjacent
        control points define a cubic bezier curve.

        \sa TransformBezier()
        \sa TransformBezierMetrics()
        \sa BezierDisplacmentFieldMetrics()
        \sa BezierDisplacmentMaskMetrics()
    */
    ulError
    XProcessBezierDisplacementField(
          const FBlock& iSource
        , const FBlock& iDestination
        , FBlock& iField
        , FBlock& iMask
        , const TArray< FCubicBezierControlPoint >& iControlPoints
        , float iThreshold = 1.f
        , uint32 iPlotSize = 1
        , const FRectI& iSourceRect = FRectI::Auto
        , eResamplingMethod iResamplingMethod = eResamplingMethod::Resampling_Bilinear
        , eBorderMode iBorderMode = eBorderMode::Border_Transparent
        , const ISample& iBorderValue = FColor::RGBA8( 0, 0, 0 )
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MonoChunk
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Get metrics before building a mip map.
    */
    static
    FRectI MipMapMetrics( const FRectI& iSource );

    /*!
        Get metrics for a mip level.
    */
    static
    FRectI MipLevelMetrics( const FRectI& iSource, uint8 iLevel );

    /*!
        Get metrics for a mip level.
    */
    static
    void MipRectsMetrics( const FRectI& iSource, uint8 iMaxLevel, TArray< FRectI >* oArray );

    /*!
        Get metrics for a mip level.
    */
    static
    uint8 MaxMipLevelMetrics( const FRectI& iSource );

    /*!
        Perform a build mip map by chaining halving bilinear resizes.

        \sa MipMapMetrics()
        \sa MipLevelMetrics()
    */
    ulError
    XBuildMipMap(
          const FBlock& iSource
        , FBlock& iDestination
        , int iMaxMipLevel = -1
        , const FRectI& iSourceRect = FRectI::Auto
        , eResamplingMethod iResamplingMethod = eResamplingMethod::Resampling_Bilinear
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MultiScanlines
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

/////////////////////////////////////////////////////
// IO
    /*!
        Perform a load operation into the input ioBlock. The result of the load
        is written in the ioBlock.

        The internal size and formats of ioBlock will be changed to match that
        of the file. The method might reallocate the internal data of the
        ioBlock asynchronously, but the ioBlock reference itself is not
        invalidated. It is only safe to access the ioBlock fields after the
        command has actually completed, you can ensure that with a Fence, a
        Finish, a Wait on the associated FEvent, or with an FEvent callback.

        If the internals are invalidated, the cleanup function of the internal
        data will be called and could destroy the already present memory so make
        sure it is not referenced elsewhere.
        \warning Functions with a prefix X means they have side effects in terms
        of the actual storage of the input block.
    */
    ulError
    XLoadBlockFromDisk(
          FBlock& ioBlock
        , const std::string& iPath
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MonoChunk
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Perform a save operation of the input iBlock at the specified path.
        iQuality is only used for jpeg files and is beetween 0 and 100

        \warning Some blocks cannot be saved, because some formats will not
        always fit a given input eFileFormat. For example, you cannot save 8bits
        HDR, or Lab pngs right out of the box, or 32bits jpegs. If such a situation
        occurs, the function will No-OP and return directly an ulError code,
        ULIS_WARNING_NO_OP_BAD_FILE_FORMAT.

        You can either perform a conversion in an appropriate format beforehand,
        or use the SaveProxyToDisk() to make sure it is always saved, at the cost
        of a potential extra conversion call. See the docs for SaveProxyToDisk()
        as it has drawbacks too.

        \sa LoadBlockFromDisk()
        \sa SaveProxyToDisk()
    */
    ulError
    SaveBlockToDisk(
          const FBlock& iBlock
        , const std::string& iPath
        , eFileFormat iFileFormat = eFileFormat::FileFormat_png
        , int iQuality = 100
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MonoChunk
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    ///*!
    //    Perform a load operation into the input ioBlock. The result of the load
    //    is written in the ioBlock.
    //
    //    The internal size and data of ioBlock will be changed to match that
    //    of the file. The format will remain the same as the block.
    //    The method might reallocate the internal data of the ioBlock
    //    asynchronously, but the ioBlock reference itself is not
    //    invalidated. It is only safe to access the ioBlock fields after the
    //    command has actually completed, you can ensure that with a Fence, a
    //    Finish, a Wait on the associated FEvent, or with an FEvent callback.
    //
    //    If the internals are invalidated, the cleanup function of the internal
    //    data will be called and could destroy the already present memory so make
    //    sure it is not referenced elsewhere.
    //*/
    //ulError
    //LoadProxyFromDisk(
    //      FBlock& ioBlock
    //    , eFormat iFormat
    //    , const std::string& iPath
    //    , const FSchedulePolicy& iPolicy = FSchedulePolicy()
    //    , uint32 iNumWait = 0
    //    , const FEvent* iWaitList = nullptr
    //    , FEvent* iEvent = nullptr
    //);

    ///*!
    //    Perform a save operation of the input iBlock at the specified path.
    //    iQuality is only used for jpeg files and is beetween 0 and 100
    //
    //    \warning When using this method, the implementation can possibly
    //    perform a conversion the input block in order to make sure it is in the
    //    appropriate memory format to match the specs of the input file format.
    //    This converted version won't be made available to you, it will remain
    //    internal so you don't have to worry about it much, but comes with an
    //    extra overhead in performances.
    //
    //    The conversion that is performed will use the equivalent of a call to
    //    ConvertFormat() beforehand, so the saved file will be the converted
    //    version and upon reloading the file, it may not match the initial
    //    format. For instance, saving a block with eFormat::Format_LabAF with an
    //    input eFileFormat::FileFormat_png will trigger a conversion to
    //    eFormat::Format_RGBA8 beforehand, so when loading the same file it will
    //    have format eFormat::Format_RGBA8, the Lab information was lost in the
    //    process.
    //
    //    \sa LoadBlockFromDisk()
    //    \sa SaveBlockToDisk()
    //*/
    //ulError
    //SaveProxyToDisk(
    //      const FBlock& iBlock
    //    , const std::string& iPath
    //    , eFileFormat iFileFormat = eFileFormat::FileFormat_png
    //    , int iQuality = 100
    //    , const FSchedulePolicy& iPolicy = FSchedulePolicy()
    //    , uint32 iNumWait = 0
    //    , const FEvent* iWaitList = nullptr
    //    , FEvent* iEvent = nullptr
    //);

    /*!
        Collect metrics before a LoadBlockFromDisk call
    */
    static
    void
    LoadBlockFromDiskMetrics(
          const std::string& iPath
        , bool *oFileExists
        , FVec2I *oSize
        , eFormat *oFormat
    );

    /*!
        Collect metrics before a SaveBlockToDisk call
    */
    static
    void
    SaveBlockToDiskMetrics(
          const FBlock& iBlock
        , eFileFormat iFileFormat
        , bool *oCanSaveWithoutProxy
    );

/////////////////////////////////////////////////////
// Misc
    /*!
        Create a simple small 64x64 block for testing purposes, in the context format.
    */
    ulError
    XCreateTestBlock(
          FBlock& iDestination
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MultiScanlines
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Perform an extract operation from the input source into the destination.
        The result of the extract is that channels specified by the input masks
        are extracted from the source and placed in the destination, with the
        appropriate type transformation if necessary.

        Masks are used as bitfields, if a bit is on that means the channel is up
        for extraction.
    */
    ulError
    Extract(
          const FBlock& iSource
        , FBlock& iDestination
        , uint8 iSourceExtractMask
        , uint8 iDestinationExtractMask
        , bool iUseRawMask = false
        , const FRectI& iSourceRect = FRectI::Auto
        , const FVec2I& iPosition = FVec2I( 0 )
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MultiScanlines
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Perform a filter operation on the block. The block will not be modified
        but the process will iterate over it and invoke the input function for
        every pixel. You can read and compute things while it does so within the
        function you provide and collect results with an event callback or a
        lambda capture scope.
    */
    ulError
    Filter(
          std::function< void( const FBlock&, const uint8* ) > iInvocation
        , const FBlock& iSource
        , const FRectI& iSourceRect = FRectI::Auto
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MultiScanlines
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Perform a filter in place operation on the block. The block can be
        modified, and the process will iterate over it and invoke the input
        function for every pixel. You can read and write pixels and compute
        anything while it does so within the function you provide.
    */
    ulError
    FilterInPlace(
          std::function< void( FBlock&, uint8* ) > iInvocation
        , FBlock& iDestination
        , const FRectI& iDestinationRect = FRectI::Auto
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MultiScanlines
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Perform a filter into operation on the blocks. The source block can't be
        modified, but the process will iterate over it and invoke the input
        function for every pixel. The destination block can be modified.
        You can read and write pixels and compute anything while it does so
        within the function you provide.
    */
    ulError
    FilterInto(
          std::function< void( const FBlock&, const uint8*, FBlock&, uint8* ) >
        , const FBlock& iSource
        , FBlock& iDestination
        , const FRectI& iSourceRect = FRectI::Auto
        , const FVec2I& iPosition = FVec2I( 0 )
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MultiScanlines
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Perform a gamma uncompress from sRGB to linear RGB on the input block.
        The input block will be modified in place. This will work with any formats,
        but makes sense only for formats with color models GREY or RGB, with or
        without alpha. In case of grey, it will work like sGrey to linear grey.
    */
    ulError
    sRGBToLinear(
          FBlock& iBlock
        , const FRectI& iRect = FRectI::Auto
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::CacheEfficient
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Perform a gamma compress from linear RGB to sRGB on the input block.
        The input block will be modified in place. This will work with any formats,
        but makes sense only for formats with color models GREY or RGB, with or
        without alpha. In case of grey, it will work like linear grey to sGrey.
    */
    ulError
    LinearTosRGB(
          FBlock& iBlock
        , const FRectI& iRect = FRectI::Auto
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::CacheEfficient
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Perform a premultiplication operation in iBlock.
        The input block will be modified in place. This will work with any formats,
        but formats without an alpha channel will actually return immediately in
        a no-op.
    */
    ulError
    Premultiply(
          FBlock& iBlock
        , const FRectI& iRect = FRectI::Auto
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::CacheEfficient
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Perform an unpremultiplication operation in iBlock.
        The input block will be modified in place. This will work with any formats,
        but formats without an alpha channel will actually return immediately in
        a no-op.
    */
    ulError
    Unpremultiply(
          FBlock& iBlock
        , const FRectI& iRect = FRectI::Auto
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::CacheEfficient
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Perform a sanitize operation in iBlock.
        The input block will be modified in place. This will work with any formats,
        but formats without an alpha channel will actually return immediately in
        a no-op.
        What SanitizeZeroAlpha is set the color components to zero if the alpha
        is already zero for a given pixel.
    */
    ulError
    SanitizeZeroAlpha(
          FBlock& iBlock
        , const FRectI& iRect = FRectI::Auto
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::CacheEfficient
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Perform a swap operation in iBlock.
        The input block will be modified in place. This will work with any formats.
        You specify input channels and they are swapped accross the entire block.
    */
    ulError
    Swap(
          FBlock& iBlock
        , uint8 iChannel1
        , uint8 iChannel2
        , const FRectI& iRect = FRectI::Auto
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::CacheEfficient
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Perform a block allocation or reallocation on the input iBlock.
        Any data that was there will be deleted and replaced according to the
        input parameters. This is mostly useful for internal subcommands but it
        might be usefull for delegating allocation of many blocks on a separate
        thread for a non-blocking flow.

        It is recommended to use it on a block created from FBlock::MakeHollow()
        so that no resources are wasted.

        \warning This will always run on detached monothread.
        \warning Functions with a prefix X means they have side effects in terms
        of the actual storage of the input block.
    */
    ulError
    XAllocateBlockData(
          FBlock& iBlock
        , uint16 iWidth
        , uint16 iHeight
        , eFormat iFormat = eFormat::Format_RGBA8
        , const FColorSpace* iColorSpace = nullptr
        , const FOnInvalidBlock& iOnInvalid = FOnInvalidBlock()
        , const FOnCleanupData& iOnCleanup = FOnCleanupData( &OnCleanup_FreeMemory )
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MonoChunk
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Perform a block deallocation on the input iBlock.
        Any data that was there will be deleted and replaced by nullptr.
        The block will have no size or data once the command is finished. It
        might be usefull for delegating deallocation of many blocks on a separate
        thread for a non-blocking flow.

        \warning This will always run on detached monothread.
        \warning Functions with a prefix X means they have side effects in terms
        of the actual storage of the input block.
    */
    ulError
    XDeallocateBlockData(
          FBlock& iBlock
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MonoChunk
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

/////////////////////////////////////////////////////
// SAT
    /*!
        Build a Summed Area Table ( used for Area resampling ) into iDestination.

        \warning This will trigger a block allocation within the iDestination.
        \warning Functions with a prefix X means they have side effects in terms
        of the actual storage of the input block.
    */
    ulError
    BuildSummedAreaTable(
          const FBlock& iSource
        , FBlock& iDestination
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MultiScanlines
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Build a Summed Area Table ( used for Area resampling ) into iDestination,
        with premultiplication.

        \warning This will trigger a block allocation within the iDestination.
        \warning Functions with a prefix X means they have side effects in terms
        of the actual storage of the input block.
    */
    ulError
    BuildPremultipliedSummedAreaTable(
          const FBlock& iSource
        , FBlock& iDestination
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MultiScanlines
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Get metrics before building a SAT, this will give you the needed format
        before building the sat.
        A block used for SAT based in input iSource needs to have the same format
        as the source, but with its type changed to float, and it must have the
        same size.
    */
    static
    eFormat SummedAreaTableMetrics( const FBlock& iSource );

/////////////////////////////////////////////////////
// Analysis
    /*!
        Perform an analysis operation on iBlock to detect the smallest possible
        rect that has visual information with alpha > 0.
        The input block will not be modified. This will work with any formats,
        but formats without an alpha channel will actually return immediately
        with a rectangle the same size as the block.
        The result of the analysis operation will be stored in the oRect parameter,
        it is unsafe to read or modify this oRect until the command is finished,
        you can ensure that with a Fence on the event or a Finish on the context.
    */
    ulError
    AnalyzeSmallestVisibleRect(
          const FBlock& iBlock
        , FRectI* oRect
        , const FRectI& iRect = FRectI::Auto
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MultiScanlines
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Perform an accumulative sampling operation on iBlock to get an averaged
        area sample of a given input rect.
        The input block will not be modified. This will work with any formats,
        The result of the analysis operation will be stored in the oColor parameter,
        it is unsafe to read or modify this oColor until the command is finished,
        you can ensure that with a Fence on the event or a Finish on the context.
    */
    ulError
    AccumulateSample(
          const FBlock& iBlock
        , FColor* oColor
        , const FRectI& iRect = FRectI::Auto
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MultiScanlines
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

/////////////////////////////////////////////////////
// Convolution
    /*!
        Perform a convolution operation with iSource as input. The result is
        written in iDestination. Convolve operator is average by default here.
    */
    ulError
    Convolve(
          const FBlock& iSource
        , FBlock& iDestination
        , const FKernel& iKernel
        , const FRectI& iSourceRect = FRectI::Auto
        , const FVec2I& iPosition = FVec2I( 0 )
        , eResamplingMethod iResamplingMethod = eResamplingMethod::Resampling_Bilinear
        , eBorderMode iBorderMode = eBorderMode::Border_Transparent
        , const ISample& iBorderValue = FColor::RGBA8( 0, 0, 0 )
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MultiScanlines
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Perform a convolution operation with iSource as input. The result is
        written in iDestination. Convolve operator is max by default here.
    */
    ulError
    ConvolveMax(
          const FBlock& iSource
        , FBlock& iDestination
        , const FKernel& iKernel
        , const FRectI& iSourceRect = FRectI::Auto
        , const FVec2I& iPosition = FVec2I( 0 )
        , eResamplingMethod iResamplingMethod = eResamplingMethod::Resampling_Bilinear
        , eBorderMode iBorderMode = eBorderMode::Border_Transparent
        , const ISample& iBorderValue = FColor::RGBA8( 0, 0, 0 )
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MultiScanlines
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Perform a convolution operation with iSource as input. The result is
        written in iDestination. Convolve operator is max by default here.
    */
    ulError
    ConvolveMin(
          const FBlock& iSource
        , FBlock& iDestination
        , const FKernel& iKernel
        , const FRectI& iSourceRect = FRectI::Auto
        , const FVec2I& iPosition = FVec2I( 0 )
        , eResamplingMethod iResamplingMethod = eResamplingMethod::Resampling_Bilinear
        , eBorderMode iBorderMode = eBorderMode::Border_Transparent
        , const ISample& iBorderValue = FColor::RGBA8( 0, 0, 0 )
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MultiScanlines
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Perform a convolution operation with iSource as input. The result is
        written in iDestination. This version of Convolve performs an on-the fly
        premultiplication, suitable for unpremultiplied formats without having
        to make a conversion first. Use when you have only on convolution to do.
    */
    ulError
    ConvolvePremult(
          const FBlock& iSource
        , FBlock& iDestination
        , const FKernel& iKernel
        , const FRectI& iSourceRect = FRectI::Auto
        , const FVec2I& iPosition = FVec2I( 0 )
        , eResamplingMethod iResamplingMethod = eResamplingMethod::Resampling_Bilinear
        , eBorderMode iBorderMode = eBorderMode::Border_Transparent
        , const ISample& iBorderValue = FColor::RGBA8( 0, 0, 0 )
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MultiScanlines
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Perform a convolution operation with iSource as input. The result is
        written in iDestination. This version of Convolve performs an on-the fly
        premultiplication, suitable for unpremultiplied formats without having
        to make a conversion first. Use when you have only on convolution to do.
    */
    ulError
    ConvolvePremultMax(
          const FBlock& iSource
        , FBlock& iDestination
        , const FKernel& iKernel
        , const FRectI& iSourceRect = FRectI::Auto
        , const FVec2I& iPosition = FVec2I( 0 )
        , eResamplingMethod iResamplingMethod = eResamplingMethod::Resampling_Bilinear
        , eBorderMode iBorderMode = eBorderMode::Border_Transparent
        , const ISample& iBorderValue = FColor::RGBA8( 0, 0, 0 )
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MultiScanlines
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Perform a convolution operation with iSource as input. The result is
        written in iDestination. This version of Convolve performs an on-the fly
        premultiplication, suitable for unpremultiplied formats without having
        to make a conversion first. Use when you have only on convolution to do.
    */
    ulError
    ConvolvePremultMin(
          const FBlock& iSource
        , FBlock& iDestination
        , const FKernel& iKernel
        , const FRectI& iSourceRect = FRectI::Auto
        , const FVec2I& iPosition = FVec2I( 0 )
        , eResamplingMethod iResamplingMethod = eResamplingMethod::Resampling_Bilinear
        , eBorderMode iBorderMode = eBorderMode::Border_Transparent
        , const ISample& iBorderValue = FColor::RGBA8( 0, 0, 0 )
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MultiScanlines
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Perform a convolution operation with iSource as input. The result is
        written in iDestination.
    */
    ulError
    MorphologicalProcess(
          const FBlock& iSource
        , FBlock& iDestination
        , const FStructuringElement& iKernel
        , const FRectI& iSourceRect = FRectI::Auto
        , const FVec2I& iPosition = FVec2I( 0 )
        , eResamplingMethod iResamplingMethod = eResamplingMethod::Resampling_Bilinear
        , eBorderMode iBorderMode = eBorderMode::Border_Transparent
        , const ISample& iBorderValue = FColor::RGBA8( 0, 0, 0 )
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MultiScanlines
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /////////////////////////////////////////////////////
// Raster
    /*!
        Draws a line between iP0 and iP1
    */
    ulError
    DrawLine(
          FBlock& iBlock
        , const FVec2I& iP0
        , const FVec2I& iP1
        , const FColor& iColor = FColor::RGBA8( 0, 0, 0 )
        , const FRectI& iClippingRect = FRectI::Auto
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MonoChunk
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Draws a line between iP0 and iP1, AA version
    */
    ulError
    DrawLineAA(
          FBlock& iBlock
        , const FVec2I& iP0
        , const FVec2I& iP1
        , const FColor& iColor = FColor::RGBA8( 0, 0, 0 )
        , const FRectI& iClippingRect = FRectI::Auto
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MonoChunk
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Draws a line between iP0 and iP1, SP version
    */
    ulError
    DrawLineSP(
          FBlock& iBlock
        , const FVec2F& iP0
        , const FVec2F& iP1
        , const FColor& iColor = FColor::RGBA8( 0, 0, 0 )
        , const FRectI& iClippingRect = FRectI::Auto
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MonoChunk
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Draws a circle. Using Andres algorithm.
    */
    ulError
    DrawCircle(
          FBlock& iBlock
        , const FVec2I& iCenter
        , const int iRadius
        , const FColor& iColor = FColor::RGBA8( 0, 0, 0 )
        , const bool iFilled = false
        , const FRectI& iClippingRect = FRectI::Auto
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MonoChunk
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Draws a circle. AA version. Using Andres algorithm.
    */
    ulError
    DrawCircleAA(
          FBlock& iBlock
        , const FVec2I& iCenter
        , const int iRadius
        , const FColor& iColor = FColor::RGBA8( 0, 0, 0 )
        , const bool iFilled = false
        , const FRectI& iClippingRect = FRectI::Auto
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MonoChunk
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Draws a circle. SP version. Using Andres algorithm.
    */
    ulError
    DrawCircleSP(
          FBlock& iBlock
        , const FVec2F& iCenter
        , const float iRadius
        , const FColor& iColor = FColor::RGBA8( 0, 0, 0 )
        , const bool iFilled = false
        , const FRectI& iClippingRect = FRectI::Auto
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MonoChunk
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Draws a circle arc. Using Andres algorithm.
    */
    ulError
    DrawArc(
          FBlock& iBlock
        , const FVec2I& iCenter
        , const int iRadius
        , const int iStartDegree
        , const int iEndDegree
        , const FColor& iColor = FColor::RGBA8( 0, 0, 0 )
        , const FRectI& iClippingRect = FRectI::Auto
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MonoChunk
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Draws a circle arc. AA version. Using Andres algorithm.
    */
    ulError
    DrawArcAA(
          FBlock& iBlock
        , const FVec2I& iCenter
        , const int iRadius
        , const int iStartDegree
        , const int iEndDegree
        , const FColor& iColor = FColor::RGBA8( 0, 0, 0 )
        , const FRectI& iClippingRect = FRectI::Auto
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MonoChunk
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Draws a circle arc. SP version. Using Andres algorithm.
    */
    ulError
    DrawArcSP(
          FBlock& iBlock
        , const FVec2F& iCenter
        , const float iRadius
        , const int iStartDegree
        , const int iEndDegree
        , const FColor& iColor = FColor::RGBA8( 0, 0, 0 )
        , const FRectI& iClippingRect = FRectI::Auto
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MonoChunk
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Draws an ellipse
    */
    ulError
    DrawEllipse(
          FBlock& iBlock
        , const FVec2I& iCenter
        , const int iA
        , const int iB
        , const FColor& iColor = FColor::RGBA8( 0, 0, 0 )
        , const bool iFilled = false
        , const FRectI& iClippingRect = FRectI::Auto
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MonoChunk
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Draws an ellipse, AA version
    */
    ulError
    DrawEllipseAA(
          FBlock& iBlock
        , const FVec2I& iCenter
        , const int iA
        , const int iB
        , const FColor& iColor = FColor::RGBA8( 0, 0, 0 )
        , const bool iFilled = false
        , const FRectI& iClippingRect = FRectI::Auto
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MonoChunk
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Draws an ellipse, AA version
    */
    ulError
    DrawEllipseSP(
          FBlock& iBlock
        , const FVec2F& iCenter
        , const float iA
        , const float iB
        , const FColor& iColor = FColor::RGBA8( 0, 0, 0 )
        , const bool iFilled = false
        , const FRectI& iClippingRect = FRectI::Auto
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MonoChunk
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Draws a rotated ellipse
    */
    ulError
    DrawRotatedEllipse(
          FBlock& iBlock
        , const FVec2I& iCenter
        , const int iA
        , const int iB
        , const int iRotationDegrees
        , const FColor& iColor = FColor::RGBA8( 0, 0, 0 )
        , const bool iFilled = false
        , const FRectI& iClippingRect = FRectI::Auto
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MonoChunk
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Draws a rotated ellipse
    */
    ulError
    DrawRotatedEllipseAA(
          FBlock& iBlock
        , const FVec2I& iCenter
        , const int iA
        , const int iB
        , const int iRotationDegrees
        , const FColor& iColor = FColor::RGBA8( 0, 0, 0 )
        , const bool iFilled = false
        , const FRectI& iClippingRect = FRectI::Auto
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MonoChunk
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Draws a rotated ellipse
    */
    ulError
    DrawRotatedEllipseSP(
          FBlock& iBlock
        , const FVec2F& iCenter
        , const float iA
        , const float iB
        , const int iRotationDegrees
        , const FColor& iColor = FColor::RGBA8( 0, 0, 0 )
        , const bool iFilled = false
        , const FRectI& iClippingRect = FRectI::Auto
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MonoChunk
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Draws a rectangle
    */
    ulError
    DrawRectangle(
          FBlock& iBlock
        , const FVec2I& iTopLeft
        , const FVec2I& iBottomRight
        , const FColor& iColor = FColor::RGBA8( 0, 0, 0 )
        , const bool iFilled = false
        , const FRectI& iClippingRect = FRectI::Auto
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MonoChunk
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Draws a polygon
    */
    ulError
    DrawPolygon(
          FBlock& iBlock
        , const std::vector< FVec2I >& iPoints
        , const FColor& iColor = FColor::RGBA8( 0, 0, 0 )
        , const bool iFilled = false
        , const FRectI& iClippingRect = FRectI::Auto
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MonoChunk
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Draws a polygon, AA version
    */
    ulError
    DrawPolygonAA(
          FBlock& iBlock
        , const std::vector< FVec2I >& iPoints
        , const FColor& iColor = FColor::RGBA8( 0, 0, 0 )
        , const bool iFilled = false
        , const FRectI& iClippingRect = FRectI::Auto
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MonoChunk
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );


    /*!
        Draws a polygon, SP version
    */
    ulError
    DrawPolygonSP(
          FBlock& iBlock
        , const std::vector< FVec2F >& iPoints
        , const FColor& iColor = FColor::RGBA8( 0, 0, 0 )
        , const bool iFilled = false
        , const FRectI& iClippingRect = FRectI::Auto
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MonoChunk
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Draws a quadratic bezier
    */
    ulError
    DrawQuadraticBezier(
          FBlock& iBlock
        , const FVec2I& iCtrlPt0
        , const FVec2I& iCtrlPt1
        , const FVec2I& iCtrlPt2
        , const float iWeight
        , const FColor& iColor = FColor::RGBA8( 0, 0, 0 )
        , const FRectI& iClippingRect = FRectI::Auto
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MonoChunk
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );


    /*!
        Draws a quadratic bezier, AA version
    */
    ulError
    DrawQuadraticBezierAA(
          FBlock& iBlock
        , const FVec2I& iCtrlPt0
        , const FVec2I& iCtrlPt1
        , const FVec2I& iCtrlPt2
        , const float iWeight
        , const FColor& iColor = FColor::RGBA8( 0, 0, 0 )
        , const FRectI& iClippingRect = FRectI::Auto
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MonoChunk
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );


    /*!
        Draws a quadratic bezier, AA version
    */
    ulError
    DrawQuadraticBezierSP(
          FBlock& iBlock
        , const FVec2F& iCtrlPt0
        , const FVec2F& iCtrlPt1
        , const FVec2F& iCtrlPt2
        , const float iWeight
        , const FColor& iColor = FColor::RGBA8( 0, 0, 0 )
        , const FRectI& iClippingRect = FRectI::Auto
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MonoChunk
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

/////////////////////////////////////////////////////
// FX Noise
    /*!
        Render white noise
    */
    ulError
    WhiteNoise(
          FBlock& iBlock
        , int iSeed = -1
        , const FRectI& iRect = FRectI::Auto
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MultiScanlines
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Render value noise
    */
    ulError
    ValueNoise(
          FBlock& iBlock
        , float iFreq = 1.f
        , int iSeed = -1
        , const FRectI& iRect = FRectI::Auto
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MultiScanlines
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Render brownian noise
    */
    ulError
    BrownianNoise(
          FBlock& iBlock
        , float iFrequency = 0.22f
        , float iFrequencyMult = 1.8f
        , float iAmplitudeMult = 0.35f
        , uint8 iNumLayers = 5
        , int iSeed = -1
        , const FRectI& iRect = FRectI::Auto
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MultiScanlines
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Render voronoi noise
    */
    ulError
    VoronoiNoise(
          FBlock& iBlock
        , uint32 iCount
        , int iSeed = -1
        , const FRectI& iRect = FRectI::Auto
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MultiScanlines
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

    /*!
        Render cloud noise, brownian with specific params.
    */
    ulError
    Clouds(
          FBlock& iBlock
        , int iSeed = -1
        , const FRectI& iRect = FRectI::Auto
        , const FSchedulePolicy& iPolicy = FSchedulePolicy::MultiScanlines
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
        , FEvent* iEvent = nullptr
    );

private:
    FCommandQueue& mCommandQueue;
    const FContextualDispatchTable* mContextualDispatchTable;
};

ULIS_NAMESPACE_END

