// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal headers
#include "CoreMinimal.h"

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

class ARIANE_API FArianeObject
{
    public:
        ~FArianeObject();
        FArianeObject();

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

    protected:
        TArray<FArianeObject*> Children;
        TArray<FArianeObject*> InvalidatedChildren;
        FArianeObject* Parent;
        FBoxSphereBounds Bounds;
        FArianeInvalidationFlags* InvalidationFlags;
};
