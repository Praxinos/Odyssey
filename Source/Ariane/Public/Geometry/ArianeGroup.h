// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal headers
#include "CoreMinimal.h"
// Ariane Headers
#include "ArianeID.h"
#include "ArianeObject.h"
#include "ArianeCoreEnums.h"

#include "ArianeGroup.generated.h"

class UArianeImage;

struct ARIANE_API FArianeGroupInvalidationFlags : FArianeObjectInvalidationFlags
{
    private:
        typedef FArianeObjectInvalidationFlags Super;

    public:
        static const uint32 StaticClass() { return  0x1d1825c6; }; // value is crc32 FArianeGroupInvalidationFlags
        virtual uint32 GetClass() { return StaticClass(); };
        virtual bool HasBaseClass( uint32 BaseClass ) const override;

    public:
        virtual FArianeGroupInvalidationFlags& AND( const FArianeObjectInvalidationFlags& RHS ) override;
        virtual FArianeGroupInvalidationFlags& OR( const FArianeObjectInvalidationFlags& RHS ) override;
        virtual FArianeGroupInvalidationFlags& SetAll() override;
        virtual FArianeGroupInvalidationFlags& Clear() override;
        virtual bool HasAny() override;

    public:
        FArianeGroupInvalidationFlags& SetHUDForegroundColor()  { HUDForegroundColor  = 1; return *this; };

    public:
        uint32 HUDForegroundColor  : 1  = 0;
};

USTRUCT(BlueprintType)
struct ARIANE_API FArianeGroup : public FArianeObject
{
GENERATED_BODY()

public:
    static uint32 StaticClass() { return 0x44e3e5a2; }; // value is crc32 FArianeGroup
    virtual uint32 GetClass() override { return StaticClass(); };
    virtual bool HasBaseClass( uint32 iBaseClassID );

public:
    virtual ~FArianeGroup();
    FArianeGroup();
    FArianeGroup( UArianeImage* InImage
                , const FName& InName
                , EArianeAllocationModel InAllocationModel );
    virtual FArianeGroup* CopyShape( const FCopyArgs& CopyArgs ) override;
    virtual void CopySettings( FArianeObject* DestinationObject, const FCopyArgs& CopyArgs, bool bInvalidate ) override;
    virtual void UpdateShape( EUpdateFlags UpdateFlags ) override;
    virtual void UpdateBoundingBox( EUpdateFlags UpdateFlags ) override;

public:
    /** overriden from ArianeObject */
    //virtual bool Update( bool Recurse, bool bClearFlags = true ) override;
    /** overriden from ArianeObject */
    //virtual void UpdateBounds() override;
    /** overriden from ArianeObject */
    //virtual void PostLoad() override;
    /** overriden from ArianeObject */
    //virtual void PostEditUndo() override;
    //virtual void ExportProperties( FArianeObject* DestObject ) override;
    //virtual void Added() override;
    //virtual void Removed() override;

#if WITH_EDITOR
   void SetHUDForegroundColor( const FColor& InHUDForegroundColor );
   void UseEditorHUDForegroundColor( bool bInUseEditorHUDForegroundColor );
   virtual FColor GetHUDForegroundColor() override;
#endif

protected:
#if WITH_EDITORONLY_DATA
    UPROPERTY( EditAnywhere )
    FColor HUDForegroundColor;

    UPROPERTY( EditAnywhere )
    bool bUseEditorHUDForegroundColor;
#endif
};
