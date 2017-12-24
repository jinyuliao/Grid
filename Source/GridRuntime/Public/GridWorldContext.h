#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GridManager.h"
#include "GridWorldContext.generated.h"

UCLASS(Blueprintable)
class GRIDRUNTIME_API UGridWorldContext : public UObject
{
	GENERATED_BODY()

public:
	virtual ~UGridWorldContext();

	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GridWorldContext")
	AGridManager* GetGridManager() const;
	virtual AGridManager* GetGridManager_Implementation() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GridWorldContext")
	APawn* GetPawnOnGrid(const UGrid* Grid) const;
	virtual APawn* GetPawnOnGrid_Implementation(const UGrid* Grid) const;

	UFUNCTION(BlueprintCallable, Category = "GridWorldContext")
	static void RegisterGridWorldContext(UGridWorldContext* Context);

	UFUNCTION(BlueprintCallable, Category = "GridWorldContext")
	static UGridWorldContext* GetGridWorldContext();

	static AGridManager* GetGlobalGridManager();
	static void SetGlobalGridManager(AGridManager* GridManager);

protected:
	UGridWorldContext();

protected:
	static UGridWorldContext* Instance;
	static AGridManager* GlobalGridManager;
};
