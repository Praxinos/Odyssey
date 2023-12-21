// IDDN.FR.001.220036.002.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"

//---

class FDetailWidgetRow;
class IDetailChildrenBuilder;
class IPropertyHandle;
class IPropertyTypeCustomizationUtils;
struct FNamingConventionPlane;
struct FNamingConventionCamera;
struct FNamingConventionShot;
struct FNamingConventionBoard;

//---

struct FNamingConventionPlaneCustomization
    : public IPropertyTypeCustomization
{
public:
    static TSharedRef<IPropertyTypeCustomization> MakeInstance();

    /** IPropertyTypeCustomization interface */
    virtual void CustomizeHeader( TSharedRef<IPropertyHandle> iStructPropertyHandle, FDetailWidgetRow& ioHeaderRow, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) override;
    virtual void CustomizeChildren( TSharedRef<IPropertyHandle> iStructPropertyHandle, IDetailChildrenBuilder& ioChildBuilder, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) override;

private:
    FText GetTooltipText() const;

    FNamingConventionPlane* GetEditStruct( TSharedRef<IPropertyHandle> iStructPropertyHandle ) const;

private:
    FNamingConventionPlane* mSettings;

    /** Property handles of the properties we're editing */
    TSharedPtr<IPropertyHandle> mPatternHandle;
};

//---

struct FNamingConventionCameraCustomization
    : public IPropertyTypeCustomization
{
public:
    static TSharedRef<IPropertyTypeCustomization> MakeInstance();

    /** IPropertyTypeCustomization interface */
    virtual void CustomizeHeader( TSharedRef<IPropertyHandle> iStructPropertyHandle, FDetailWidgetRow& ioHeaderRow, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) override;
    virtual void CustomizeChildren( TSharedRef<IPropertyHandle> iStructPropertyHandle, IDetailChildrenBuilder& ioChildBuilder, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) override;

private:
    FText GetTooltipText() const;

    FNamingConventionCamera* GetEditStruct( TSharedRef<IPropertyHandle> iStructPropertyHandle ) const;

private:
    FNamingConventionCamera* mSettings;

    /** Property handles of the properties we're editing */
    TSharedPtr<IPropertyHandle> mPatternHandle;
};

//---

struct FNamingConventionShotCustomization
    : public IPropertyTypeCustomization
{
public:
    static TSharedRef<IPropertyTypeCustomization> MakeInstance();

    /** IPropertyTypeCustomization interface */
    virtual void CustomizeHeader( TSharedRef<IPropertyHandle> iStructPropertyHandle, FDetailWidgetRow& ioHeaderRow, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) override;
    virtual void CustomizeChildren( TSharedRef<IPropertyHandle> iStructPropertyHandle, IDetailChildrenBuilder& ioChildBuilder, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) override;

private:
    FText GetTooltipText() const;

    FNamingConventionShot* GetEditStruct( TSharedRef<IPropertyHandle> iStructPropertyHandle ) const;

private:
    FNamingConventionShot* mSettings;

    /** Property handles of the properties we're editing */
    TSharedPtr<IPropertyHandle> mPatternHandle;
};

//---

struct FNamingConventionBoardCustomization
    : public IPropertyTypeCustomization
{
public:
    static TSharedRef<IPropertyTypeCustomization> MakeInstance();

    /** IPropertyTypeCustomization interface */
    virtual void CustomizeHeader( TSharedRef<IPropertyHandle> iStructPropertyHandle, FDetailWidgetRow& ioHeaderRow, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) override;
    virtual void CustomizeChildren( TSharedRef<IPropertyHandle> iStructPropertyHandle, IDetailChildrenBuilder& ioChildBuilder, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) override;

private:
    FText GetTooltipText() const;

    FNamingConventionBoard* GetEditStruct( TSharedRef<IPropertyHandle> iStructPropertyHandle ) const;

private:
    FNamingConventionBoard* mSettings;

    /** Property handles of the properties we're editing */
    TSharedPtr<IPropertyHandle> mPatternHandle;
};
