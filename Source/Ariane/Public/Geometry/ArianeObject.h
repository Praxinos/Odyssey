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
class UArianeLayerDrawing;

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
        FArianeObjectInvalidationFlags& SetAltered()  { Altered   = 1; return *this; };
        FArianeObjectInvalidationFlags& SetSelected() { Selected  = 1; return *this; };
        FArianeObjectInvalidationFlags& SetHierarchy(){ Hierarchy = 1; return *this; };
        FArianeObjectInvalidationFlags& SetColor()    { Color     = 1; return *this; };
        FArianeObjectInvalidationFlags& SetChildren() { Children  = 1; return *this; };

    public:
        bool Selected  : 1 = 0;
        bool Altered   : 1 = 0;
        bool Hierarchy : 1 = 0;
        bool Color     : 1 = 0;
        bool Children  : 1 = 0;
};

USTRUCT(BlueprintType)
struct ARIANE_API FArianeObject
{
    GENERATED_BODY()

    public:
        enum class TraversalReturnValue{ Continue, IgnoreChildren, Stop };

    public:
        /**
         * @brief Get the class type
         * @return the class type
         */
        static uint32 StaticClass() { return 0x7b527cb6; }; // value is crc32 FArianeObject

        /**
         * @brief Get the object type
         * @return the object type
         */
        virtual uint32 GetClass() { return StaticClass(); };
        virtual bool HasBaseClass( uint32 iBaseClassID );

    public:
        virtual ~FArianeObject();
        FArianeObject();

        FArianeObject( UArianeLayerDrawing* InDrawingLayer );

        /**
         * @brief Add a child to this object at the end of the list of children.
         * @param Child the child to add
         */
        void AppendChild( FArianeObject* Child );

        /**
         * @brief Add a child to this object at the beginning of the list of children.
         * @param Child the child to add
         */
        void PrependChild( FArianeObject* Child );

        /**
         * @brief Insert a child to this object's list of children.
         * @param Child the child to add
         * @param InsertAfter insert the added child after this one.
         */
        void InsertChild( FArianeObject* Child, FArianeObject* InsertAfter );

        /**
         * @brief Mark the object as invalidated. It will also invalidate the whole chain of parents.
         * @param InInvalidationFlags the setted flags. They will be combined with the existing ones (via the OR operand).
         */
        void Invalidate( const FArianeObjectInvalidationFlags& InInvalidationFlags );

        /**
         * @brief Get the list of invalidated children, recursively if desired.
         * @param OutInvalidatedObjects the array to fill with invalidated children.
         * @param bRecurse recurse inside sub-objects.
         */
        void GetInvalidatedChildren( TArray<FArianeObject*> OutInvalidatedObjects, bool bRecurse );

        /**
         * @brief Update the object
         * @param bRecurse Update recursively
         */
        virtual bool Update( bool bRecurse, bool bClearFlags = true );

        /** Get the invalidation flags */
        FArianeObjectInvalidationFlags& GetInvalidationFlags();

        /** Get object's bounds */
        const FBoxSphereBounds& GetBounds();

        /** Update object's bounds */
        virtual void UpdateBounds();

        /** Run any object-specific task required immediately after loading an object */
        virtual void PostLoad();

        /** Run any object-specific task required immediately after undoing / redoing */
        virtual void PostEditUndo();

        /**
         * @brief Export object's properties to another object
         * @param DestObject the object that will receive the properties
         */
        virtual void ExportProperties( FArianeObject* DestObject );

        /** Get object's translation */
        FVector GetTranslation();

        /** Get object's rotation */
        FVector GetRotationInDegrees();

        /** Get object's scaling */
        FVector GetScaling();

        /** Get object's Guid */
        const FGuid& GetGuid();

        /**
         * @brief Run a function to each object of the object tree
         * @param Callback the function to run
         */
        void Traverse( TFunction<TraversalReturnValue(FArianeObject*)> Callback );

        /** Get object's parent object */
        FArianeObject* GetParent();

        /**
         * @brief Set this objects' parent object
         * @param Parent the parent
         */
        void SetParent( FArianeObject* Parent );

        /**
         * @brief Remove a child object from this object
         * @param bRemoveFromInstancedObjects true to also unallocate the child.
         */
        void RemoveChild( FArianeObject* ChildToRemove, bool bRemoveFromInstancedObjects );

        /**
         * @brief Get the object's visibility
         * @param bInHierarchical true if it should consider the whole chain of parent objects as well, false otherwise
         * @return true or false
         */
        virtual bool IsVisible( bool bInHierarchical );

        /** Get the drawing layer this object belongs to */
        UArianeLayerDrawing* GetDrawingLayer();

        /** Set the drawing layer this object belongs to */
        void SetDrawingLayer( UArianeLayerDrawing* InLayer );

        FSimpleMulticastDelegate & GetOnPostInvalidatedDelegate();

        /** Called when the object is added to a parent object */
        virtual void Added(){};

        /** Called when the object is removed from a parent object */
        virtual void Removed(){};

        const FTransform& GetTransform();

    protected:
        /**
         * @brief Invalidate a child. It will also invalidate the whole chain of parents.
         * @param Child the child to invalidate.
         */
        void InvalidateChild( FArianeObject* Child );


        TraversalReturnValue Traverse_Private( TFunction<TraversalReturnValue(FArianeObject*)> Callback );

    protected:
        UPROPERTY( EditAnywhere )
        FName Name;

        UPROPERTY( EditAnywhere, meta = (IgnoreForMemberInitializationTest) )
        FGuid Guid;

        UPROPERTY( EditAnywhere )
        FArianeObjectID ParentID;

        UPROPERTY( EditAnywhere )
        FVector Translation;

        UPROPERTY( EditAnywhere )
        FVector RotationInDegrees;

        UPROPERTY( EditAnywhere )
        FVector Scaling;

        UPROPERTY( EditAnywhere )
        UArianeLayerDrawing* DrawingLayer;

    protected:
        FSimpleMulticastDelegate  OnPostInvalidated;
        TArray<FArianeObjectID> InvalidatedChildrenID;
        TArray<FArianeObjectID> ChildrenID;
        FBoxSphereBounds Bounds;
        FArianeObjectInvalidationFlags* InvalidationFlags;
};
