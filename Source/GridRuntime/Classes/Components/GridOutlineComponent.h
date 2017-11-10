#pragma once

#include "CoreMinimal.h"
#include "PrimitiveSceneProxy.h"
#include "Components/PrimitiveComponent.h"
#include "GridOutlineComponent.generated.h"

/**
 * 
 */
UCLASS()
class GRIDRUNTIME_API UGridOutlineComponent : public UPrimitiveComponent
{
	GENERATED_BODY()
	
public:
	UGridOutlineComponent();
	virtual ~UGridOutlineComponent();

	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;

	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;

	virtual void UpdateGridInfo();
};
