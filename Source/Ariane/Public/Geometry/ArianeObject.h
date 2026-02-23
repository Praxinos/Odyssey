// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal headers
#include "CoreMinimal.h"
#include "ArianeID.h"

#include "ArianeObject.generated.h"

class UArianePainting3DComponent;
struct FArianeObject;

struct ARIANE_API FArianeInvalidationFlags
{
    public:
        static void AND( FArianeInvalidationFlags& Result
                       , const FArianeInvalidationFlags& LHS
                       , const FArianeInvalidationFlags& RHS );
        static void OR( FArianeInvalidationFlags& Result
                      , const FArianeInvalidationFlags& LHS
                      , const FArianeInvalidationFlags& RHS );
        void Clear();

    protected:
        virtual uint32 GetSize() const = 0;
};

struct ARIANE_API FArianeObjectInvalidationFlags : FArianeInvalidationFlags
{
    protected:
        virtual uint32 GetSize() const override { return sizeof( FArianeObjectInvalidationFlags ); };

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
        FArianeInvalidationFlags& GetInvalidationFlags();
        const FBoxSphereBounds& GetBounds();
        virtual void UpdateBounds();

    protected:
        void InvalidateChild( FArianeObject* Child );

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
        TArray<FArianeObject*> InvalidatedChildren;

        FBoxSphereBounds Bounds;
        FArianeInvalidationFlags* InvalidationFlags;
};
