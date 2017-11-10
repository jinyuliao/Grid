// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "Components/DecalComponent.h"
#include "LineComponent.generated.h"

/**
 * 
 */
UCLASS()
class GRIDRUNTIME_API ULineComponent : public UPrimitiveComponent
{
	GENERATED_BODY()
	
public:
	ULineComponent();
	virtual ~ULineComponent();

	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;

	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "LineComponent")
	FLinearColor LineColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "LineComponent")
	float LineThickness;

	UFUNCTION(BlueprintCallable, Category = "LineComponent")
	virtual void SetPoints(const TArray<FVector>& Points);

protected:
	FBoxSphereBounds Bounds;
};
