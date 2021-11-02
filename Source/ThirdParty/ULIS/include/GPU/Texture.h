// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         Texture.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the FTexture class.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#ifdef ULIS_FEATURE_GPU_ENABLED

#pragma once
#include "Core/Core.h"
#include "Core/Callback.h"
#include "Image/ColorSpace.h"
#include "Image/Format.h"
#include "Math/Geometry/Rectangle.h"
#include "Math/Geometry/Vector.h"

ULIS_NAMESPACE_BEGIN
class FTexture;
class FTexture_Private;
/////////////////////////////////////////////////////
/// @class      FTexture
/// @brief      The FTexture class provides a mean of storing and manipulating
///             digital images on the GPU in various formats.
/// @details    FTexture is the way to manipulate images on GPU with ULIS.
///
///             The texture has a format which specifies the type, memory layout,
///             color model, and other informations about the image.
///             The underlying data is dependant on the GPU backend, but is most
///             likely stored in GPU VRAM.
///
///             The FTexture class is very lightweight, the memory load is in the
///             data it points to in the VRAM, and the class never performs any
///             kind of heavy operations. It differs from FBlock because of that.
///
///             Copy, wether deep or shallow, is explicitly forbidden, If you
///             need to copy image data, you can use the explicit Copy functions
///             that ULIS provides.
///
///             FTexture objects can be edited using an FContext instance, it
///             also supports uploading a of an FBlock into its bulk.
///             FTexture API does not allow implicit or explicit sharing of the
///             underlying data accross multiples instances.
///
///             You can also provide custom callbacks to signal when the texture
///             is dirty, for example use it to download a small rect of the
///             image to a CPU texture when triggered.
///
///             It is perfectly fine to create FTexture objects on the stack,
///             But you can also allocate textures dynamically.
class ULIS_API FTexture final
    : public IHasFormat
    , public IHasColorSpace
{
public:
    /*! Destroy the texture and invoke the cleanup callback. */
    ~FTexture();

    /*!
    Construct a texture with input size and format.

    The underlying bitmap data will be allocated with enough storage space as
    required by the size and format depth. The data is left uninitialized.

    \warning The \a iWidth and \a iHeight parameters should be greater than
    zero. A block doesn't own nor manage lifetime of its color-space.
    */
    FTexture(
          uint16 iWidth
        , uint16 iHeight
        , eFormat iFormat
        , const FColorSpace* iColorSpace = nullptr
        , const FOnInvalidTexture& iOnInvalid = FOnInvalidTexture()
        , const FOnCleanupData& iOnCleanup = FOnCleanupData()
    );

    /*!
    explicitly disabled copy constructor.

    If you need to make a shallow copy with shared image data, use the explicit
    constructor from external data.

    If you need to make a deep copy of the image data, use the explicit Copy or
    XCopy functions that ULIS provides.
    */
    FTexture( const FTexture& ) = delete;

    /*!
    explicitly disabled copy assignment operator.

    If you need to make a shallow copy with shared image data, use the explicit
    constructor from external data.

    If you need to make a deep copy of the image data, use the explicit Copy or
    XCopy functions that ULIS provides.
    */
    FTexture& operator=( const FTexture& ) = delete;

    /*!
    Return the width of the block.

    \sa Height()
    \sa Area()
    \sa Rect()
    */
    uint16 Width() const;

    /*!
    Return the height of the block.

    \sa Width()
    \sa Area()
    \sa Rect()
    */
    uint16 Height() const;

    /*!
    Return the area of the block.

    This can also be interpreted as the full length of the underlying buffer.

    \sa Width()
    \sa Height()
    \sa Rect()
    */
    uint32 Area() const;

    /*!
    Return a rect representing the bounding box of the block.

    \sa Width()
    \sa Height()
    \sa Area()
    */
    FRectI Rect() const;

    /*!
    Return the numbers of bytes per scanline

    \sa BytesTotal()
    */
    uint32 BytesPerScanLine() const;

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
    only once later with the union of all invalid rects.
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
    Set a new invalid callback that will be called on dirty.

    \sa OnCleanup()
    */
    void OnInvalid( const FOnInvalidTexture& iOnInvalid );

    /*!
    Set a new cleanup callback that will be called on destruction.

    \sa OnInvalid()
    */
    void OnCleanup( const FOnCleanupData& iOnCleanup );

private:
    FTexture_Private* d;
};

ULIS_NAMESPACE_END

#endif //ULIS_FEATURE_GPU_ENABLED

