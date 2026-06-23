// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal headers
#include "CoreMinimal.h"
// Ariane headers
#include "ArianeCoreEnums.h"

#include "ArianeTag.generated.h"

struct FArianeObject;
struct FArianeGroup;
class UArianeLayer;
struct FArianeObjectInvalidationFlags;

USTRUCT(BlueprintType)
struct ARIANE_API FArianeTag
{
    GENERATED_BODY()

public:
    static uint32 StaticClass() { return 0x644bad9d ; }; // value is crc32 FArianeTag
    virtual uint32 GetClass() { return StaticClass(); };

    virtual ~FArianeTag();
    FArianeTag();
    FArianeTag( FArianeObject* InOwnerObject, EArianeAllocationModel InAllocationModel );
    virtual void Draw( double iAncestorsOpacity
                        , uint64 iDrawingFlags );
    virtual void Update( uint32 UpdateFlags
                        , FArianeObjectInvalidationFlags& OwnerInvalidationFlags );
    //virtual void UpdateMatrix();
    virtual void ObjectAdded();
    virtual void ObjectRemoved();
    virtual void Added();
    virtual void Removed();
    virtual FArianeTag* Copy( FArianeObject* InOwnerObject );

    FArianeObject* GetOwner( );

    /** Get tag's Guid */
    const FGuid& GetGuid();
    EArianeAllocationModel GetAllocationModel();

protected:
    UPROPERTY( EditAnywhere, meta = (IgnoreForMemberInitializationTest) )
    FGuid Guid;

    UPROPERTY()
    EArianeAllocationModel AllocationModel;

protected:
    FArianeObject* Owner;
    uint32 Flags;
    bool bShared;
};
