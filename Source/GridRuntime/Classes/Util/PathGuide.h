#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/LineComponent.h"
#include "Grid.h"
#include "PathGuide.generated.h"

/**
	Used for rendering path
*/
UCLASS()
class GRIDRUNTIME_API APathGuide : public AActor
{
	GENERATED_BODY()
	
public:	
	APathGuide();
	virtual ~APathGuide();

	UFUNCTION(BlueprintCallable, Category = "PathGuide")
	virtual void SetPath(const TArray<UGrid*>& Path);

	UFUNCTION(BlueprintCallable, Category = "PathGuide")
	virtual void SetCustomPoints(const TArray<FVector>& Points);

	/** set materical to null will disable start decal */
	UFUNCTION(BlueprintCallable, Category = "PathGuide")
	virtual void SetStartDecalMaterial(UMaterialInterface* NewMaterial);

	/** set materical to null will disable desitination decal */
	UFUNCTION(BlueprintCallable, Category = "PathGuide")
	virtual void SetDestinationDecalMaterial(UMaterialInterface* NewMaterial);

	UFUNCTION(BlueprintCallable, Category = "PathGuide")
	virtual void Clear();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PathGuide")
	FLinearColor PathColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PathGuide")
	float PathThickness;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PathGuide")
	UMaterialInterface* StartDecalMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PathGuide")
	UMaterialInterface* DestinationDecalMaterial;

	/** using ZOffset to make sure the path is higher than terrain */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PathGuide")
	float ZOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PathGuide")
	float LineBreakThreshold;

	/** set GridType manually if you use SetCustomPoints, otherwise the decal will rendering incorrect */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PathGuide")
	EGridType GridType;

	/** set GridSize manually if you use SetCustomPoints, otherwise the decal will rendering incorrect */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PathGuide")
	float GridSize;

	/** using DecalSizeScale can make the decal smaller(or larger) than grid size */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PathGuide")
	float DecalSizeScale;

protected:
	void UpdateDecal(const TArray<FVector>& Points);

protected:
	UPROPERTY()
	UDecalComponent* StartDecalComp;

	UPROPERTY()
	UDecalComponent* DestDecalComp;

	UPROPERTY()
	ULineComponent* PathLineComponent;
};
