// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TResourceManager.generated.h"

UENUM(BlueprintType)
enum class ETResourceType: uint8
{
	NONE = 0,
	Wood,
	MAX
};

struct FResource;
USTRUCT(BlueprintType)
struct TYCOON_API FTResourceDesc
{
	GENERATED_BODY()

public:
	int32 GetCount() const;
	ETResourceType GetResourceType() const;
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Resource, meta = (AllowPrivateAccess = "true"))
	int32 Count = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Resource, meta = (AllowPrivateAccess = "true"))
	ETResourceType Type = ETResourceType::Wood;
};

USTRUCT(BlueprintType)
struct TYCOON_API FTResourceIncomeDesc : public FTResourceDesc
{
	GENERATED_BODY()

	public:
	float GetSpawnRewardTime() const;
	int32 GetMaxDeposit() const;
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Resource, meta = (AllowPrivateAccess = "true"))
	float SpawnRewardTime = 5.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Resource, meta = (AllowPrivateAccess = "true"))
	int32 MaxDeposit = 500.f;
};


USTRUCT()
struct TYCOON_API FResource
{
	GENERATED_BODY()

public:
	FResource();
	explicit FResource(int32 CountInit);
	bool Add(int32 Value);
	int32 GetCount() const;
	ETResourceType GetType() const;
private:
	int32 Count = 0;
	ETResourceType Type = ETResourceType::Wood;
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnUpdateResourceCount, ETResourceType, int32);

UCLASS()
class TYCOON_API ATResourceManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATResourceManager();
	UFUNCTION(BlueprintCallable)
	int32 GetCount(ETResourceType Type) const;
	void Add(ETResourceType Type, int32 Value);
	void Spend(const TArray<FTResourceDesc>& Price);
	void CheckResources();
	bool CanBuy(const TArray<FTResourceDesc>& Price);

	FOnUpdateResourceCount OnUpdateResourceCount;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TMap<ETResourceType, FResource> CurrentResources;
};



