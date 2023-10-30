// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "OdysseyVector.h"

#include "OdysseyPainterEditorVectorBaseTool.generated.h"

UCLASS(Abstract)
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorVectorBaseTool : public UOdysseyPainterEditorTool
{
    public:
        GENERATED_BODY()

    protected:
        // reserved flags returned by callbaks, the eight higher bits
        //static const uint64 NOMENU = ( FOdysseyVectorEngine::SIGNAL_USER0_RESERVED );

    public:
        // Destructor
        virtual ~UOdysseyPainterEditorVectorBaseTool();

        //Constructor
        UOdysseyPainterEditorVectorBaseTool();

        static bool DoubleClicked();

        virtual TSharedRef<SWidget> CreateTopTabWidget() override;

        virtual void Load();
        virtual void Unload();
        virtual bool OnKeyDown( const FKey& iKey ) override;
        virtual bool OnKeyUp( const FKey& iKey ) override;
        virtual bool OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey );
        virtual void OnMouseHover( const FOdysseyPoint& iPointInTexture );
        virtual void OnMouseDrag( const FOdysseyPoint& iPointInTexture );
        virtual bool OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey );
        virtual void Commit();
        virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;
        virtual void ExtendContextMenu( FMenuBuilder& menu );

    protected:
        virtual uint64 LoadVector( FOdysseyVectorScene* iScene ){ return 0; };
        virtual uint64 UnloadVector( FOdysseyVectorScene* iScene ){ return 0; };
        virtual uint64 OnKeyDownVector( FOdysseyVectorScene* iScene
                                        , const FKey& iKey );
        virtual uint64 OnKeyUpVector( FOdysseyVectorScene* iScene
                                      , const FKey& iKey );
        virtual uint64 OnMouseDownVector( FOdysseyVectorScene* iScene
                                        , const FOdysseyPoint& iPointInTexture
                                        , const FKey& iKey ){ return false; };
        virtual uint64 OnMouseDragVector( FOdysseyVectorScene* iScene
                                        , const FOdysseyPoint& iPointInTexture ){ return 0; };
        virtual uint64 OnMouseHoverVector( FOdysseyVectorScene* iScene
                                        , const FOdysseyPoint& iPointInTexture ){ return 0; };
        virtual uint64 OnMouseUpVector( FOdysseyVectorScene* iScene
                                      , const FOdysseyPoint& iPointInTexture
                                      , const FKey& iKey ){ return false; };
        virtual uint64 PropertyChangedVector( FOdysseyVectorScene* iScene
                                            , const FName& iPropertyName );

        void PopupContextMenu();
        TSharedPtr<SWidget> CreateContextMenu();

    private:
        void OnKeyDownCommon( FOdysseyVectorScene* iScene, const FKey& iKey );
        void OnKeyUpCommon( FOdysseyVectorScene* iScene, const FKey& iKey );
        void PropertyChangedCommon(  FOdysseyVectorScene* iScene, const FName& iPropertyName );

    protected:
        void Copy( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );
        void Paste( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene );

    private:
        void ExtendContextMenuObject( FMenuBuilder& menu );
        void ExtendContextMenuVertex( FMenuBuilder& menu );

    protected:
        bool mHasContextMenu;

    public:
        //UPROPERTY( EditAnywhere, Category = Behavior )
        //bool RestrictToSelectedObjects;
};
