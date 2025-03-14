// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022


#pragma once

#include "CoreMinimal.h"

/*----------------------------------------------------------------------------
    FOdysseyDuplicateDataReader.
----------------------------------------------------------------------------*/

class FOdysseyDuplicateDataReader : public FArchive
{
public:
    FOdysseyDuplicateDataReader( const FLargeMemoryData& InObjectData )
        : ObjectData(InObjectData)
        , Offset(0)
    {
        this->SetIsLoading(true);
        this->SetIsPersistent(true);
        this->ArNoIntraPropertyDelta = true;
        ArPortFlags |= PPF_Duplicate;
    }

public:
    /**
     * Returns the name of the Archive.  Useful for getting the name of the package a struct or object
     * is in when a loading error occurs.
     *
     * This is overridden for the specific Archive Types
     **/
    virtual FString GetArchiveName() const { return TEXT("FOdysseyDuplicateDataReader"); }

    virtual int64 Tell()
    {
        return Offset;
    }
    virtual int64 TotalSize()
    {
        return ObjectData.GetSize();
    }

private:
    //~ Begin FArchive Interface.
    virtual FArchive& operator<<(FName& N) override
    {
        FNameEntryId ComparisonIndex;
        FNameEntryId DisplayIndex;
        int32 Number;
        ByteOrderSerialize(&ComparisonIndex, sizeof(ComparisonIndex));
        ByteOrderSerialize(&DisplayIndex, sizeof(DisplayIndex));
        ByteOrderSerialize(&Number, sizeof(Number));
        // copy over the name with a name made from the name index and number
        N = FName(ComparisonIndex, DisplayIndex, Number);
        return *this;
    }

    virtual void Serialize(void* Data,int64 Num)
    {
        if (ObjectData.Read(Data, Offset, Num))
        {
            Offset += Num;
        }
    }

    virtual void Seek(int64 InPos)
    {
        Offset = InPos;
    }

private:
    const FLargeMemoryData&                    ObjectData;
    int64                                    Offset;
};
