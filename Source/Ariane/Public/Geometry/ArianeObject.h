// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal headers
#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
#include "DynamicMeshBuilder.h"
// Ariane Headers
#include "ArianeID.h"
#include "ArianeCoreEnums.h"

#include "ArianeObject.generated.h"

class UArianePainting3DComponent;
struct FArianeObject;
struct FArianeGroup;
struct FArianeTag;
class UArianeLayerDrawing;
class UArianeImage;

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
    FArianeObjectInvalidationFlags& SetAltered()   { Altered   = 1; return *this; };
    FArianeObjectInvalidationFlags& SetSelected()  { Selected  = 1; return *this; };
    FArianeObjectInvalidationFlags& SetHierarchy() { Hierarchy = 1; return *this; };
    FArianeObjectInvalidationFlags& SetColor()     { Color     = 1; return *this; };
    FArianeObjectInvalidationFlags& SetChildren()  { Children  = 1; return *this; };
    FArianeObjectInvalidationFlags& SetTags()      { Tags      = 1; return *this; };
    FArianeObjectInvalidationFlags& SetName()      { Name      = 1; return *this; };
    FArianeObjectInvalidationFlags& SetTransform() { Transform = 1; return *this; };

public:
    uint32 Selected  : 1 = 0;
    uint32 Altered   : 1 = 0;
    uint32 Hierarchy : 1 = 0;
    uint32 Color     : 1 = 0;
    uint32 Children  : 1 = 0;
    uint32 Tags      : 1 = 0;
    uint32 Name      : 1 = 0;
    uint32 Transform : 1 = 0;
};

class ARIANE_API FArianeObjectGeometry3D
{
    public:
        virtual ~FArianeObjectGeometry3D();
        FArianeObjectGeometry3D( FArianeObject* InObject );

        virtual void Build() = 0;

        const uint32 GetVertexCount() const;
        const FRawStaticIndexBuffer& GetIndexBuffer() const;
        FArianeObject* GetObject();
        FLocalVertexFactory* GetVertexFactory();
        void InitVertexFactory();

    protected:
        FArianeObject* Object;

        TArray<FDynamicMeshVertex> MeshVertices;
        TArray<uint32> MeshIndices;

        uint32 VertexCount;
        FPositionVertexBuffer PositionBuffer;
        FStaticMeshVertexBuffer StaticMeshVB;
        FColorVertexBuffer ColorBuffer;
        FRawStaticIndexBuffer IndexBuffer;
        FLocalVertexFactory* VertexFactory;
};

USTRUCT(BlueprintType)
struct ARIANE_API FArianeObject
{
GENERATED_BODY()

public:
    enum class ECopyFlags : uint8
    {
        IgnoreTags             = ( 1 << 0 ),
        Rename                 = ( 1 << 1 ),
        AsBezier               = ( 1 << 2 ),
        AsPolyline             = ( 1 << 3 ),
        PrimitiveAsPath        = ( 1 << 4 ),
    };

    struct FCopyArgs
    {
        ECopyFlags Flags = {}; // init as zero
        EArianeAllocationModel AllocationModel;
        UArianeImage* Image = nullptr;
    };

    enum class ETraversalReturnValue{ Continue, IgnoreChildren, Stop };

    enum class EUpdateFlags : uint8
    {
        None            =          0,
        Interactive     = ( 1 << 0 ),
        KeepInvalidated = ( 1 << 1 ),
    };

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
    FArianeObject( UArianeImage* InImage
                 , const FName& InName
                 , EArianeAllocationModel InAllocationModel
                 , FArianeObjectInvalidationFlags* InInvalidationFlags = nullptr );

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
    virtual bool Update( EUpdateFlags UpdateFlags, bool bRecurse );

    /** Get the invalidation flags */
    virtual FArianeObjectInvalidationFlags& GetInvalidationFlags();

    /** Get object's bounds */
    const FBox& GetBoundingBox() const;

    /** Update object's bounds */
    virtual void UpdateBoundingBox( EUpdateFlags UpdateFlags );

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
    FVector GetRotation();

    /** Get object's scaling */
    FVector GetScaling();

    /** Get object's Guid */
    const FGuid& GetGuid();

    /**
        * @brief Run a function to each object of the object tree
        * @param Callback the function to run
        */
    static void Traverse( FArianeObject* Object, TFunction<ETraversalReturnValue(FArianeObject*)> Callback );

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
    UArianeImage* GetImage();

    /** Set the drawing layer this object belongs to */
    void SetImage( UArianeImage* InImage );

    FSimpleMulticastDelegate & GetOnPostInvalidatedDelegate();

    /** Called when the object is added to a parent object */
    virtual void Added(){};

    /** Called when the object is removed from a parent object */
    virtual void Removed(){};

    /**
        * @brief Get the object's name
        * @return a reference to the object's name.
        */
    const FName& GetName();

    const FTransform& GetTransform();

#ifdef WITH_EDITOR
    virtual FColor GetHUDForegroundColor();
#endif
    FArianeObject* GetAncestorByClass( uint32 iClass, bool iBaseClass, bool iSelf );
    TArray<FArianeObjectID>& GetChildren();
    const TArray<FArianeObjectID>& GetChildren() const;

    void SetExpanded( bool bInExpanded );
    bool IsExpanded();
    bool IsSelected();
    UArianePainting3DComponent* GetPainting3DComponent();
    FArianeTag* GetTagByGuid( const FGuid& InGuid );
    const TArray<FArianeTagID>& GetTags() const;
    TArray<FArianeTagID>& GetTags();
    FArianeGroup* GetRootGroup();
    void TransferChild( FArianeObject* FosterChild, FArianeObject* InsertAfter );
    FArianeObject* GetPreviousChild( FArianeObject* Child );
    void SetVisible( bool bInVisible );
    void SetName( const FName& InName );
    void SetSelected( bool bInSelected );
    static void TraverseBackwards( FArianeObject* Object, TFunction<ETraversalReturnValue(FArianeObject*)> Callback );

    FArianeObject* Copy( const FCopyArgs& CopyArgs
                       , TFunction<void( FArianeObject*, const FCopyArgs& )> PreCallback
                       , TFunction<void( FArianeObject*, FArianeObject*, const FCopyArgs& )> PostCallback );
    FArianeObject* Copy( const FCopyArgs& CopyArgs );
    void AddTag( FArianeTag* Tag );
    EArianeAllocationModel GetAllocationModel();
    static uint32 GetCommonClass( const TArray<FArianeObject*>& Objects );
    virtual void UpdateShape( EUpdateFlags UpdateFlags );
    void UpdateTransform();
    void SetTranslation( double InX, double InY, double InZ );
    void SetTranslation( const FVector& InTranslation );
    void SetRotation( double InX, double InY, double InZ );
    void SetRotation( const FVector& InRotation );
    void SetScaling( double InX, double InY, double InZ );
    void SetScaling( const FVector& InScaling );
    const FTransform& GetLocalTransform();
    void GetTransform( FVector& OutTranslation
                     , FVector& OutRotation
                     , FVector& OutScaling
                     , FVector& OutSkewing );

    void SetTransform( const FVector& InTranslation
                     , const FVector& InRotation
                     , const FVector& InScaling
                     , const FVector& InSkewing );

    void ResetTransform();
    FArianeObject* GetNextChild( FArianeObject* Child );

protected:
    /**
        * @brief Invalidate a child. It will also invalidate the whole chain of parents.
        * @param Child the child to invalidate.
        */
    void InvalidateChild( FArianeObject* Child );

    virtual FArianeObject* CopyShape( const FCopyArgs& CopyArgs );
    virtual void CopySettings( FArianeObject* DestinationObject, const FCopyArgs& CopyArgs, bool bInvalidate );

    static ETraversalReturnValue Traverse_Private( FArianeObject* Object, TFunction<ETraversalReturnValue(FArianeObject*)> Callback );
    static ETraversalReturnValue TraverseBackwards_Private( FArianeObject* Object, TFunction<ETraversalReturnValue(FArianeObject*)> Callback );

    static uint32 CheckCommonClass( const TArray<FArianeObject*>& Objects, uint32 CommonClass );


protected:
    UPROPERTY( EditAnywhere )
    FName Name;

    UPROPERTY( EditAnywhere, meta = (IgnoreForMemberInitializationTest) )
    FGuid Guid;

    UPROPERTY( EditAnywhere )
    FArianeObjectID ParentID;

    UPROPERTY( EditAnywhere )
    UArianeImage* Image;

    UPROPERTY( EditAnywhere )
    bool bVisible;

    UPROPERTY( EditAnywhere )
    bool bExpanded;

    UPROPERTY( EditAnywhere )
    TArray<FArianeTagID> Tags;

    UPROPERTY()
    TArray<FInstancedStruct> InstancedTags;

    UPROPERTY()
    EArianeAllocationModel AllocationModel;

    UPROPERTY()
    FTransform LocalTransform;


    FTransform WorldTransform;
    uint32 WorldTransformVersion;

protected:
    FSimpleMulticastDelegate  OnPostInvalidated;
    TArray<FArianeObjectID> InvalidatedChildren;
    TArray<FArianeObjectID> Children;
    FBox BoundingBox;
    FArianeObjectInvalidationFlags* InvalidationFlags;
    bool bSelected;
};

// define bitwise op
ENUM_CLASS_FLAGS(FArianeObject::ECopyFlags)
ENUM_CLASS_FLAGS(FArianeObject::EUpdateFlags)
