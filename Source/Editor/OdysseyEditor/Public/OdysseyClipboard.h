// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

class ODYSSEYEDITOR_API IOdysseyClipboardData
{
public:
    IOdysseyClipboardData(const FGuid& iId)
        : mId(iId)
    {}

public:
    const FGuid& GetId() const { return mId; }

private:
    FGuid mId;
};

class ODYSSEYEDITOR_API FOdysseyClipboard
{
public:
    FOdysseyClipboard()
        : mData(nullptr)
    {
    }

public:
    bool IsEmpty() const { return !mData; }
    const FGuid& GetId() const
    {
        static FGuid emptyId;
        return mData ? mData->GetId() : emptyId;
    }

    template <class T>
    TSharedPtr<T> GetData()
    {
        if (IsEmpty())
            return nullptr;

        if ( mData->GetId() != T().GetId() )
            return nullptr;

        return StaticCastSharedPtr<T>(mData);
    }

    template <class T>
    void SetData(TSharedPtr<T> iData)
    {
        mData = iData;
    }

private:
    TSharedPtr<IOdysseyClipboardData> mData;
};