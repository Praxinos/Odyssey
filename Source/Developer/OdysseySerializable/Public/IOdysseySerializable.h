// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
/**
 * Odyssey Serializable
 * An abstract class for a serializable, which can be saved and loaded from a binary file
 */
class ODYSSEYSERIALIZABLE_API IOdysseySerializable
{
public:
    // Construction / Destruction
    virtual ~IOdysseySerializable() = 0;
    IOdysseySerializable( const uint8 iVersion );

public:
    virtual uint8 GetVersion() const;
    
    // Overloads for save in archive
    friend FArchive& operator<<(FArchive &Ar, IOdysseySerializable* ioSaveSerializable );

private:
    uint8         mVersion;
};


//Serialization of item
#include "IOdysseySerializable.inl"
