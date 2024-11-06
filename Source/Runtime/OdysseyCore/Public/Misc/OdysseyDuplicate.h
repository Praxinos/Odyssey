// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Serialization/LargeMemoryData.h"
#include "Misc/OdysseyDuplicateDataWriter.h"
#include "Misc/OdysseyDuplicateDataReader.h"

namespace Odyssey {
    /** Duplicates a iSource in iDest */
    template<class T> void Duplicate(T* iSource, T* iDest)
    {
        FPooledLargeMemoryData ObjectData;
        FOdysseyDuplicateDataWriter writer(ObjectData.Get());
        iSource->Serialize(writer);
        FOdysseyDuplicateDataReader reader(ObjectData.Get());
        iDest->Serialize(reader);
        iDest->PostDuplicate();
    }
}