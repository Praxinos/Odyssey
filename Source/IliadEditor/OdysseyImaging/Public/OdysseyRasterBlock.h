// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Serialization/EditorBulkData.h"

#include <ULIS>

/**
 * The pixels of a raster layer or cell saved before the tiled image storage.
 *
 * Only loaded: the owner moves these pixels into its tiled image after loading, then drops this object.
 * Assets are no longer saved with it.
 */
class ODYSSEYIMAGING_API FOdysseyRasterBlock
{
public:
    ~FOdysseyRasterBlock();

    /** @param iOwner The object whose archive holds the pixels. */
    explicit FOdysseyRasterBlock(UObject* iOwner);

    FOdysseyRasterBlock(const FOdysseyRasterBlock&) = delete;
    FOdysseyRasterBlock& operator=(const FOdysseyRasterBlock&) = delete;

public:
    int GetWidth() const;
    int GetHeight() const;
    ::ULIS::eFormat GetFormat() const;

    /**
     * Returns the loaded pixels, read from the bulk data on first call.
     * @return Null when no pixels were loaded, or they do not match the loaded size and format.
     */
    TSharedPtr<::ULIS::FBlock> GetBlock();

    /** Loads the block from an archive; saving writes nothing. */
    void Serialize(FArchive& Ar);

private:
    friend class FOdysseyRasterBlockImport;

private:
    UObject* mOwner;
    int mWidth = -1;
    int mHeight = -1;
    ::ULIS::eFormat mFormat = ::ULIS::Format_RGBA8;

    /** The id of the block in the former disk cache, read to stay in step with the archive. */
    FGuid mId;

    /** The saved pixels. */
    UE::Serialization::FEditorBulkData mBulkData;

    TSharedPtr<::ULIS::FBlock> mBlock;
};

ODYSSEYIMAGING_API FArchive& operator<<(FArchive& Ar, FOdysseyRasterBlock& iRasterBlock);
