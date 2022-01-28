// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         Block.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the FBlock class.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Core/Callback.h"
#include "Image/Color.h"
#include "Image/ColorSpace.h"
#include "Image/Format.h"
#include "Image/Size2D.h"
#include "Image/Pixel.h"
#include "Math/Geometry/Rectangle.h"
#include "Math/Geometry/Vector.h"

ULIS_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @class      FBlock
/// @brief      The FBlock class provides a mean of storing and manipulating
///             digital images in various formats, with direct access to pixel
///             data.
/// @details    FBlock is the primary class to use when manipulating images
///             with ULIS.
///
///             The block has a format which specifies the type, memory layout,
///             color model, and other informations about the image.
///             The underlying data is always a regular contiguous buffer
///             storage that lives on RAM with no alignment requirement.
///
///             The FBlock class is very lightweight, the memory load is in the
///             data it points to, and the class never performs any kind of
///             heavy operations. It does not communicate with the GPU or use
///             any kind of hardware accelerated feature just by itself.
///
///             Copy, wether deep or shallow, is explicitly forbidden, If you
///             need to copy image data, you can use the explicit Copy or XCopy
///             functions that ULIS provides.
///
///             FBlock supports loading an external data pointer and
///             treat it as its own storage: this is useful for sharing memory
///             without having to copy it and you can grab or release ownership
///             at any time with the help of the cleanup callbacks, just make
///             sure to use the proper deallocator when providing external data
///             which was allocated with a custom allocator. However the data
///             is not reference counted.
///
///             You can also provide custom callbacks to signal when the block
///             is dirty, for example use it to upload a small rect of the
///             image to a GPU texture when triggered.
///
///             It is perfectly fine to create FBlock objects on the stack:
///             \snippet data/block_snippet.h FBlock on stack
///             But you can also allocate blocks dynamically:
///             \snippet data/block_snippet.h FBlock on heap
class ULIS_API FBlock
    : public IHasFormat
    , public IHasColorSpace
    , public IHasSize2D
{
public:
    /*! Destroy the block and invoke the cleanup callback. */
    ~FBlock();

    /*!
    Construct a block with input size and format.

    The underlying bitmap data will be allocated with enough storage space as
    required by the size and format depth. The data is left uninitialized.

    \warning The \a iWidth and \a iHeight parameters should be greater than
    zero. A block doesn't own nor manage lifetime of its color-space.
    */
    FBlock(
          uint16 iWidth
        , uint16 iHeight
        , eFormat iFormat = eFormat::Format_RGBA8
        , const FColorSpace* iColorSpace = nullptr
        , const FOnInvalidBlock& iOnInvalid = FOnInvalidBlock()
        , const FOnCleanupData& iOnCleanup = FOnCleanupData( &OnCleanup_FreeMemory )
    );

    /*!
    Construct a block from an existing external buffer with input size and
    format.

    The input buffer data is used and no allocation is done here, if coherent
    visual data was present in the buffer it will remain as is. Make sure to
    chose the appropriate size and format, given the buffer length. Bad values
    could result in misinterpreting the visual information or accessing the
    buffer out of bounds and crash or corrupt memory.

    \warning The block will not delete the external input data by default,
    unless you provide and appropriate cleanup callback to handle that. The
    data can be shared with other representations but it must remain valid at
    least as long as the block lifetime.

    \warning The \a iWidth and \a iHeight parameters should be greater than
    zero. A block doesn't own nor manage lifetime of its color-space.
    */
    FBlock(
          uint8* iData = nullptr
        , uint16 iWidth = 0
        , uint16 iHeight = 0
        , eFormat iFormat = eFormat::Format_RGBA8
        , const FColorSpace* iColorSpace = nullptr
        , const FOnInvalidBlock& iOnInvalid = FOnInvalidBlock()
        , const FOnCleanupData& iOnCleanup = FOnCleanupData()
    );

    /*!
    Construct a hollow block with no internal data or size.
    */
    static FBlock MakeHollow();

    /*!
    Check wether a block is hollow.
    */
    bool IsHollow() const;

    /*!
    explicitly disabled copy constructor.

    If you need to make a shallow copy with shared image data, use the explicit
    constructor from external data.

    If you need to make a deep copy of the image data, use the explicit Copy or
    XCopy functions that ULIS provides.
    */
    FBlock( const FBlock& ) = delete;

    /*!
    explicitly disabled copy assignment operator.

    If you need to make a shallow copy with shared image data, use the explicit
    constructor from external data.

    If you need to make a deep copy of the image data, use the explicit Copy or
    XCopy functions that ULIS provides.
    */
    FBlock& operator=( const FBlock& ) = delete;

    /*!
    Obtain a pointer to the raw data at the base element of the underlying
    image buffer.

    This data is the same as the one used by the block. It is non-const and can
    be modified safely, as long as you don't dereference elements outside of
    the buffer boundaries, or trigger race conditions when applying
    multithreaded operations on the same data.

    \sa ScanlineBits()
    \sa PixelBits()
    */
    uint8* Bits();

    /*!
    Obtain a pointer to the raw data at the specified scanline row element of
    the underlying image buffer.

    This data is the same as the one used by the block. It is non-const and can
    be modified safely, as long as you don't dereference elements outside of
    the buffer boundaries, or trigger race conditions when applying
    multithreaded operations on the same data.

    \warning If you specify a row that is negative or bigger than the block
    height, the function will trigger an assert and crash in debug builds, but
    it will fail silently and access the buffer out of bounds in release builds
    , leading to potential memory corruption or crashes further down the line.

    \sa Bits()
    \sa PixelBits()
    */
    uint8* ScanlineBits( uint16 iRow );

    /*!
    Obtain a pointer to the raw data at the specified plane element of the
    underlying image buffer.

    This data is the same as the one used by the block. It is non-const and can
    be modified safely, as long as you don't dereference elements outside of
    the buffer boundaries, or trigger race conditions when applying
    multithreaded operations on the same data.

    \warning If you specify a plane that is negative or bigger than the block
    sample count, the function will trigger an assert and crash in debug builds, but
    it will fail silently and access the buffer out of bounds in release builds
    , leading to potential memory corruption or crashes further down the line.

    \sa Bits()
    \sa PixelBits()
    \sa ScanlineBits()
    */
    uint8* PlaneBits( uint16 iPlane );

    /*!
    Obtain a pointer to the raw data at the specified pixel location of
    the underlying image buffer.

    This data is the same as the one used by the block. It is non-const and can
    be modified safely, as long as you don't dereference elements outside of
    the buffer boundaries, or trigger race conditions when applying
    multithreaded operations on the same data.

    \warning If you specify X and Y coordinates that are either negative or
    bigger than the block width and height respectively, the function will
    trigger an assert and crash in debug builds, but it will fail silently and
    access the buffer out of bounds in release builds, leading to potential
    memory corruption or crashes further down the line.

    \sa Bits()
    \sa ScanlineBits()
    */
    uint8* PixelBits( uint16 iX, uint16 iY );

    /*!
    Obtain a pointer to the raw data at the base element of the underlying
    image buffer.

    This data is the same as the one used by the block. It is const so it
    cannot be modified, but it can be read freely, as long as you don't
    dereference elements outside of the buffer boundaries.

    \sa ScanlineBits()
    \sa PixelBits()
    */
    const uint8* Bits() const;

    /*!
    Obtain a pointer to the raw data at the specified scanline row element of
    the underlying image buffer.

    This data is the same as the one used by the block. It is const so it
    cannot be modified, but it can be read freely, as long as you don't
    dereference elements outside of the buffer boundaries.

    \warning If you specify a row that is negative or bigger than the block
    height, the function will trigger an assert and crash in debug builds, but
    it will fail silently and access the buffer out of bounds in release
    builds, leading to potential memory corruption or crashes further down the
    line.

    \sa Bits()
    \sa PixelBits()
    */
    const uint8* ScanlineBits( uint16 iRow ) const;

    /*!
    Obtain a pointer to the raw data at the specified plane element of the
    underlying image buffer.

    This data is the same as the one used by the block. It is non-const and can
    be modified safely, as long as you don't dereference elements outside of
    the buffer boundaries, or trigger race conditions when applying
    multithreaded operations on the same data.

    \warning If you specify a plane that is negative or bigger than the block
    sample count, the function will trigger an assert and crash in debug builds, but
    it will fail silently and access the buffer out of bounds in release builds
    , leading to potential memory corruption or crashes further down the line.

    \sa Bits()
    \sa PixelBits()
    \sa ScanlineBits()
    */
    const uint8* PlaneBits( uint16 iPlane ) const;

    /*!
    Obtain a pointer to the raw data at the specified pixel location of
    the underlying image buffer.

    This data is the same as the one used by the block. It is const so it
    cannot be modified, but it can be read freely, as long as you don't
    dereference elements outside of the buffer boundaries.

    \warning If you specify X and Y coordinates that are either negative or
    bigger than the block width and height respectively, the function will
    trigger an assert and crash in debug builds, but it will fail silently and
    access the buffer out of bounds in release builds, leading to potential
    memory corruption or crashes further down the line.

    \sa Bits()
    \sa ScanlineBits()
    */
    const uint8* PixelBits( uint16 iX, uint16 iY ) const;

    /*!
    Return the numbers of bytes per scanline

    \sa BytesTotal()
    */
    uint32 BytesPerScanLine() const;

    /*!
    Return the numbers of bytes per plane

    \sa BytesTotal()
    \sa BytesPerScanLine()
    */
    uint32 BytesPerPlane() const;

    /*!
    Return the total numbers of bytes in the block.

    \sa BytesPerScanLine()
    */
    uint64 BytesTotal() const;

    /*!
    Dirty the entire block and trigger the invalid callback if set.

    The optional \a iCallOnInvalid parameter can be set to false in order to
    conditionaly disable the callback trigger depending on your needs.

    In the case of many successive calls to dirty, it can be more efficient to
    just disable them first and call it only once later.
    */
    void Dirty( bool iCallOnInvalid = true ) const;

    /*!
    Dirty a part of the block according to the input rect and trigger the
    invalid callback if set.

    The optional \a iCallOnInvalid parameter can be set to false in order to
    conditionaly disable the callback trigger depending on your needs.

    In the case of many successive calls to dirty with different small
    geometries, it can be more efficient to just disable them first and call it
    only once later with the union of all invalid rects, or use the array
    version of the Dirty function.
    */
    void Dirty( const FRectI& iRect, bool iCallOnInvalid = true ) const;

    /*!
    Dirty parts of the block from the input array of rects and trigger the
    invalid callback if set.

    The optional \a iCallOnInvalid parameter can be set to false in order to
    conditionaly disable the callback trigger depending on your needs.
    */
    void Dirty( const FRectI* iRectList, const uint32 iNumRects = 1, bool iCallOnInvalid = true ) const;

    /*!
    Return the color of the pixel at the given coordinates.

    The FColor is a copy of the pixel data, safe to read and write without
    altering the block contents.

    \warning If you specify X and Y coordinates that are either negative or
    bigger than the block width and height respectively, the function will
    trigger an assert and crash in debug builds, but it will fail silently and
    access the buffer out of bounds in release builds, leading to potential
    memory corruption or crashes further down the line.

    \sa Pixel()
    \sa PixelBits()
    \sa FColor
    */
    FColor Color( uint16 iX, uint16 iY ) const;

    /*!
    Return the color sample at the given coordinates.

    The FColor has its own storage that represents a subpixel color, safe to
    read and write without altering the block contents.

    \sa Pixel()
    \sa PixelBits()
    \sa FColor
    */
    FColor
    Sample(
          int16 iX
        , int16 iY
        , eBorderMode iBorderMode = eBorderMode::Border_Transparent
        , const FColor& iConstant = FColor::Transparent
    ) const;

    /*!
    Return the color at the given subpixel coordinates.

    The FColor has its own storage that represents a subpixel color, safe to
    read and write without altering the block contents.

    \sa Pixel()
    \sa PixelBits()
    \sa FColor
    */
    FColor
    SampleSubpixel(
          float iX
        , float iY
        , eBorderMode iBorderMode = eBorderMode::Border_Transparent
        , const FColor& iConstant = FColor::Transparent
        , bool iCompensateBlackDrifting = true
    ) const;

    /*!
    Return the pixel at the given coordinates.

    The FPixel is an implicitely shared representation of the block data at the
    input pixel location, if you modify it you will alter the contents of the
    block. It is sometimes more convenient than PixelBits() in order to read
    or modify the values, and does not perform a copy.

    \warning If you specify X and Y coordinates that are either negative or
    bigger than the block width and height respectively, the function will
    trigger an assert and crash in debug builds, but it will fail silently and
    access the buffer out of bounds in release builds, leading to potential
    memory corruption or crashes further down the line.

    \sa Color()
    \sa PixelBits()
    \sa FPixel
    */
    FPixel Pixel( uint16 iX, uint16 iY );

    /*!
    Return the const pixel at the given coordinates.

    The FPixel is an implicitely shared representation of the block data at the
    input pixel location It is sometimes more convenient than PixelBits() in
    order to read the values, and does not perform a copy.

    \warning If you specify X and Y coordinates that are either negative or
    bigger than the block width and height respectively, the function will
    trigger an assert and crash in debug builds, but it will fail silently and
    access the buffer out of bounds in release builds, leading to potential
    memory corruption or crashes further down the line.

    \sa Color()
    \sa PixelBits()
    \sa FPixel
    */
    const FPixel Pixel( uint16 iX, uint16 iY ) const;

    /*!
    Set the pixel at the given coordinates from input ISample.
    The sample will be converted to the appropriate format.
    This function is here for convenience but is not recommended to used for
    intensive raster tasks, as it does not provide any kind of optimizations.

    \warning If you specify X and Y coordinates that are either negative or
    bigger than the block width and height respectively, the function will
    trigger an assert and crash in debug builds, but it will fail silently and
    access the buffer out of bounds in release builds, leading to potential
    memory corruption or crashes further down the line.

    \sa Color()
    \sa PixelBits()
    \sa ISample
    */
    void SetPixel( uint16 iX, uint16 iY, const ISample& iSample );

    /*!
    Set the pixel at the given coordinates from input ISample.
    The sample will be converted to the appropriate format.
    This function is here for convenience but is not recommended to used for
    intensive raster tasks, as it does not provide any kind of optimizations.

    Safe version that checks if the pixel set is in the block bounds.
    Slow but useful in some cases. The usage of this function usually 
    means that optimization is possible where it is used.

    \sa Color()
    \sa PixelBits()
    \sa ISample
    */
    void SetPixelSafe(uint16 iX, uint16 iY, const ISample& iSample);


    /*!
    Set a new invalid callback that will be called on dirty.

    \sa OnCleanup()
    */
    void OnInvalid( const FOnInvalidBlock& iOnInvalid );

    /*!
    Optain the invalid callback for convenience when reloading data or propagating.

    \sa OnCleanup()
    */
    const FOnInvalidBlock& OnInvalid() const;

    /*!
    Set a new cleanup callback that will be called on destruction.

    \sa OnInvalid()
    */
    void OnCleanup( const FOnCleanupData& iOnCleanup );

    /*!
    Optain the cleanup callback for convenience when reloading data or propagating.

    \sa OnCleanup()
    */
    const FOnCleanupData& OnCleanup() const;

    /*!
    Reconstruct the internal representation from an existing external buffer
    with input size and format.

    If a cleanup callback was setup, it will be called beforehand, effectively
    cleaning the data if needed. Although the format might change, references
    to the format info object itself are not invalidated.

    \sa FBlock()
    */
    void LoadFromData(
          uint8* iData
        , uint16 iWidth
        , uint16 iHeight
        , eFormat iFormat
        , const FColorSpace* iColorSpace = nullptr
        , const FOnInvalidBlock& iOnInvalid = FOnInvalidBlock()
        , const FOnCleanupData& iOnCleanup = FOnCleanupData()
    );

    /*!
    Reconstruct the internal representation with input size and format.

    If a cleanup callback was setup, it will be called beforehand, effectively
    cleaning the data if needed. Although the format might change, references
    to the format info object itself are not invalidated.

    \sa FBlock()
    */
    void ReallocInternalData(
          uint16 iWidth
        , uint16 iHeight
        , eFormat iFormat
        , const FColorSpace* iColorSpace = nullptr
        , const FOnInvalidBlock& iOnInvalid = FOnInvalidBlock()
        , const FOnCleanupData& iOnCleanup = FOnCleanupData()
    );

    /*!
    Get CRC32 Hash for Tile
    */
    uint32 CRC32() const;

protected:
    uint8* mBitmap; ///< Contiguous memory storage buffer for the block.
    uint32 mBytesPerScanline; ///< Cached number of bytes per scanline.
    uint32 mBytesPerPlane; ///< Cached number of bytes per plane.
    uint64 mBytesTotal; ///< Cached number of bytes for the whole buffer.
    FOnInvalidBlock mOnInvalid; ///< The callback for when the block is dirty.
    FOnCleanupData mOnCleanup; ///< The callback for when the block is destroyed.
};

ULIS_NAMESPACE_END

