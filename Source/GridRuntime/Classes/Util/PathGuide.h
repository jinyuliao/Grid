#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/LineComponent.h"
#include "Grid.h"
#include "PathGuide.generated.h"

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

	UFUNCTION(BlueprintCallable, Category = "PathGuide")
	virtual void SetStartDecalMaterial(UMaterialInterface* NewMaterial);

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PathGuide")
	float ZOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PathGuide")
	float LineBreakThreshold;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PathGuide")
	EGridType GridType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PathGuide")
	float GridSize;

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
