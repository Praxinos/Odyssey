// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal headers
#include "CoreMinimal.h"
#include "ArianeID.h"
#include <functional>

#include "ArianeObject.generated.h"

class UArianePainting3DComponent;
struct FArianeObject;

struct ARIANE_API FArianeObjectInvalidationFlags
{
    public:
        static const uint32 StaticClass() { return 0xd8793c00; }; // value is crc32 FArianeObjectInvalidationFlags
        virtual uint32 GetClass() { return StaticClass(); };
        virtual bool HasBaseClass( uint32 BaseClass ) const;

    public:
        virtual FArianeObjectInvalidationFlags& AND( const FArianeObjectInvalidationFlags& RHS );
        virtual FArianeObjectInvalidationFlags& OR( const FArianeObjectInvalidationFlags& RHS );
        virtual FArianeObjectInvalidationFlags& SetAll();
        virtual FArianeObjectInvalidationFlags& Clear();
        virtual bool HasAny();

    public:
        static void ClearOwn( FArianeObjectInvalidationFlags& Flags );

    public:
        FArianeObjectInvalidationFlags& SetAltered()  { Selected  = 1; return *this; };
        FArianeObjectInvalidationFlags& SetSelected() { Altered   = 1; return *this; };
        FArianeObjectInvalidationFlags& SetHierarchy(){ Hierarchy = 1; return *this; };

    public:
        uint32 Selected  : 1 = 0;
        uint32 Altered   : 1 = 0;
        uint32 Hierarchy : 1 = 0;
};

USTRUCT(BlueprintType)
struct ARIANE_API FArianeObject
{
    GENERATED_BODY()

    private:
        static const uint32 mStaticClass = 0x7b527cb6; // value is crc32 FArianeObject

    public:
        static uint32 StaticClass() { return mStaticClass; };
        virtual uint32 GetClass() { return mStaticClass; };
        //virtual bool HasBaseClass( uint32 iBaseClassID );

    public:
        virtual ~FArianeObject();
        FArianeObject();

        FArianeObject( UArianePainting3DComponent* InPainting3DComponent );

        void AppendChild( FArianeObject* iChild );
        void PrependChild( FArianeObject* iChild );
        void AddChild( FArianeObject* Child, FArianeObject* InsertAfter );
        void Invalidate( const FArianeObjectInvalidationFlags& InInvalidationFlags );
        void GetInvalidatedObjects( TArray<FArianeObject*> OutInvalidatedObjects, bool Recurse );
        virtual bool Update( bool Recurse );
        FArianeObjectInvalidationFlags& GetInvalidationFlags();
        const FBoxSphereBounds& GetBounds();
        virtual void UpdateBounds();
        virtual void InvalidatePointerCache();

        virtual void PostEditUndo(){};

    protected:
        void InvalidateChild( FArianeObject* Child );
        void InvalidatePointerCache( TArray<FArianeObjectID>& ObjectIDArray );

    public:
        UPROPERTY( EditAnywhere )
        UArianePainting3DComponent* Painting3DComponent;

        UPROPERTY( EditAnywhere )
        FGuid Guid;

        UPROPERTY( EditAnywhere )
        TArray<FArianeObjectID> ChildrenID;

        UPROPERTY( EditAnywhere )
        FArianeObjectID ParentID;

    protected:
        TArray<FArianeObjectID> InvalidatedChildrenID;

        FBoxSphereBounds Bounds;
        FArianeObjectInvalidationFlags* InvalidationFlags;
};
