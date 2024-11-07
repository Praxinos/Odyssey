// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

class FOdysseyDuplicateDataWriter : public FArchive
{
public:
    FOdysseyDuplicateDataWriter(
        FLargeMemoryData& InObjectData
    )
    : ObjectData(InObjectData)
    , Offset(0)
    {
        this->SetIsSaving(true);
        this->SetIsPersistent(true);
        this->ArNoIntraPropertyDelta = true;
        ArAllowLazyLoading    = false;
        ArPortFlags |= PPF_Duplicate;
    }

public:
    virtual FString GetArchiveName() const { return TEXT("FOdysseyDuplicateDataWriter"); }

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
        FNameEntryId ComparisonIndex = N.GetComparisonIndex();
        FNameEntryId DisplayIndex = N.GetDisplayIndex();
        int32 Number = N.GetNumber();
        ByteOrderSerialize(&ComparisonIndex, sizeof(ComparisonIndex));
        ByteOrderSerialize(&DisplayIndex, sizeof(DisplayIndex));
        ByteOrderSerialize(&Number, sizeof(Number));
        return *this;
    }

    virtual void Serialize(void* Data,int64 Num)
    {
        if (ObjectData.Write(Data, Offset, Num))
        {
            Offset += Num;
        }
    }

    virtual void Seek(int64 InPos)
    {
        Offset = InPos;
    }

private:
    FLargeMemoryData&                        ObjectData;
    int64                                    Offset;
};
