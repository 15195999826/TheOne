// Fill out your copyright notice in the Description page of Project Settings.


#include "HexGrid/HexGrid.h"

#include "EngineUtils.h"
#include "FastNoiseWrapper.h"
#include "LandDynamicMeshActor.h"
#include "AI/TheOneAIController.h"
#include "Components/InstancedStaticMeshComponent.h"

DEFINE_LOG_CATEGORY(LogHexGrid);

// Sets default values
AHexGrid::AHexGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	HexGridWireframe = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("HexGridWireframe"));
	HexGridWireframe->SetupAttachment(SceneRoot);
	
	HexGridFaceIndicator = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("HexGridFaceIndicator"));
	HexGridFaceIndicator->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HexGridFaceIndicator->SetupAttachment(SceneRoot);
	
	HexGridLand = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("RegularHexTile"));
	HexGridLand->SetupAttachment(SceneRoot);
}

int AHexGrid::GetDistance(const FHCubeCoord& A, const FHCubeCoord& B)
{
	return (FMath::Abs(A.QRS.X - B.QRS.X) + FMath::Abs(A.QRS.Y - B.QRS.Y) + FMath::Abs(A.QRS.Z - B.QRS.Z)) / 2;
}

// Called when the game starts or when spawned
void AHexGrid::BeginPlay()
{
	Super::BeginPlay();
	EmptyHexTile.CubeCoord.QRS = FIntVector(-1);
}

// Called every frame
void AHexGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHexGrid::Empty()
{
	GridTiles.Empty();
	auto MeshComs = SceneRoot->GetAttachChildren();
	TArray<UInstancedStaticMeshComponent*> MeshComsCache;
	for (auto MeshCom : MeshComs)
	{
		if (auto Mesh = Cast<UInstancedStaticMeshComponent>(MeshCom))
		{
			Mesh->ClearInstances();
		}
	}
}

void AHexGrid::CreateGrid(int RDSeed)
{
	RandomSeed = RDSeed;
	CreateGrid();
}

void AHexGrid::CreateGrid()
{
	SCOPE_CYCLE_COUNTER(STAT_CreateGrid);
	WireframeDefaultCustomData = {WireframeDefaultColor.R, WireframeDefaultColor.G, WireframeDefaultColor.B, WireframeDefaultColor.A};
	FaceIndicatorDefaultCustomData = {FaceIndicatorDefaultColor.R, FaceIndicatorDefaultColor.G, FaceIndicatorDefaultColor.B, FaceIndicatorDefaultColor.A};

	auto MeshComs = SceneRoot->GetAttachChildren();
	TArray<UInstancedStaticMeshComponent*> MeshComsCache;
	for (auto MeshCom : MeshComs)
	{
		if (auto Mesh = Cast<UInstancedStaticMeshComponent>(MeshCom))
		{
			Mesh->ClearInstances();
			if (Mesh == HexGridLand || Mesh == HexGridWireframe || Mesh == HexGridFaceIndicator)
			{
				continue;
			}
			MeshComsCache.Add(Mesh);
		}
	}
	
	EnvironmentMeshes.Empty();

	int CacheCursor = 0;
	// 初始化EnvironmentMeshes, 在蓝图中手动创建备用组件
	for (const auto& Pair:Environments)
	{
		if (!Pair.Value.DecorationMesh.IsValid())
		{
			continue;
		}

		if (!MeshComsCache.IsValidIndex(CacheCursor))
		{
			UE_LOG(LogHexGrid, Error, TEXT("MeshComsCache is not enough"));
			continue;
		}

		auto GetCom =  MeshComsCache[CacheCursor];
		CacheCursor++;
		GetCom->SetStaticMesh(Pair.Value.DecorationMesh.LoadSynchronous());
		EnvironmentMeshes.Add(Pair.Key, GetCom);
	}

	FRandomStream RandomStream{ RandomSeed };

	// Todo: 先跑通网格-》地图-》移动的流程，再思考地图随机规则， 先用柏林噪声生成地图
	FastNoiseWrapper = NewObject<UFastNoiseWrapper>();
	FastNoiseWrapper->SetupFastNoise(
		FastNoiseSetting.NoiseType,
		FastNoiseSetting.TheOne,
		FastNoiseSetting.Frequency,
		FastNoiseSetting.Interp,
		FastNoiseSetting.FractalType,
		FastNoiseSetting.Octaves,
		FastNoiseSetting.Lacunarity,
		FastNoiseSetting.Gain,
		FastNoiseSetting.CellularJitter,
		FastNoiseSetting.CellularDistanceFunction,
		FastNoiseSetting.CellularReturnType
		);
	
	RdHeightAreaCoords.Empty();
	if (TileConfig.HeightRandomType == EHTileHeightRandomType::RDHeightArea)
	{
		const TArray<FHCubeCoord>& AreaCorePossibleIndexes = GetRangeCoords(FHCubeCoord{FIntVector::ZeroValue}, RDHeightAreaConfig.AreaRadius);
	
		TArray<FHCubeCoord> AreaCorePositions;
		for (int i = 0; i < RDHeightAreaConfig.CoreCount; ++i)
		{
			// 保证随机结果与AreaCorePositions中的点不重复，且距离大于等于CoreMinDistance
			bool bIsValid = false;
			int MaxTry = 100;
			FHCubeCoord NewCore;

			while (!bIsValid && MaxTry-- > 0)
			{
				// 随机选择一个可能的索引
				int RandomIndex = RandomStream.RandRange(0, AreaCorePossibleIndexes.Num() - 1);
				NewCore = AreaCorePossibleIndexes[RandomIndex];

				bIsValid = true;
				for (const auto& ExistingCore : AreaCorePositions)
				{
					if (GetDistance(NewCore, ExistingCore) < RDHeightAreaConfig.CoreMinDistance)
					{
						bIsValid = false;
						break;
					}
				}
			}

			if (bIsValid)
			{
				AreaCorePositions.Add(NewCore);
			}
		}
		
		for (const auto& Core : AreaCorePositions)
		{
			// 随机半径
			int Radius = RandomStream.RandRange(RDHeightAreaConfig.MinRadius, RDHeightAreaConfig.MaxRadius);
			auto AreaCoords = GetRangeCoords(Core, Radius);
			for (const auto& Coord : AreaCoords)
			{
				// 随机是否空地
				if (RandomStream.FRand() < RDHeightAreaConfig.EmptyWeight)
				{
					continue;
				}

				RdHeightAreaCoords.AddUnique(Coord);
			}
		}
	}
	
	// https://www.unrealengine.com/en-US/blog/optimizing-tarray-usage-for-performance
	// preallocate array memory
	// R1 = 1 + 6*1
	// R2 = 1 + 6*1 + 6*2
	// R3 = 1 + 6*1 + 6*2 + 6*3
	// R4 = 1 + 6*1 + 6*2 + 6*3 + 6*4
	// R5 = .......
	GridTiles.Empty();
	
	// int32 T = 1.1f; 不会报错 int32 T{1.1f} Ide就会报错
	int32 Size{ 1 };
	auto Radius = TileConfig.Radius;
	for (int32 i{ 1 }; i <= Radius; ++i)
	{
		Size += 6 * i;
	}
	// Reverse 预分配指定数量的元素内存空间
	// GridCoordinates.Reserve(Size);
	GridTiles.Reserve(Size);

	auto Offset = GetGridRotator();

	if (TileConfig.BaseOnRadius)
	{
		for (int32 Q{-Radius}; Q <= Radius; ++Q)
		{
			// Calculate R1
			int32 R1{FMath::Max(-Radius, -Q - Radius)};

			// Calculate R2
			int32 R2{FMath::Min(Radius, -Q + Radius)};

			for (int32 R{R1}; R <= R2; ++R)
			{
				AddTile(Offset, RandomStream, Q, R);
			}
		}
	} else
	{
		const auto RowStart = -FMath::FloorToInt(TileConfig.Row / 2.f);
		const auto RowEnd = FMath::CeilToInt(TileConfig.Row / 2.f);
		const auto ColumnStart = -FMath::FloorToInt(TileConfig.Column / 2.f);
		const auto ColumnEnd = FMath::CeilToInt(TileConfig.Column / 2.f);
		if (TileConfig.TileOrientation == EHTileOrientationFlag::FLAT)
		{
			for (int32 Column{ColumnStart}; Column < ColumnEnd; ++Column)
			{
				for (int32 Row{RowStart}; Row < RowEnd; ++Row)
				{
					auto Q = Column;
					auto R = Row - (Column - (Column & 1)) / 2;
					AddTile(Offset, RandomStream, Q, R);
				}
			}
		}
		else if (TileConfig.TileOrientation == EHTileOrientationFlag::POINTY)
		{
			for (int32 Row{ RowStart }; Row < RowEnd; ++Row)
			{
				for (int32 Column{ ColumnStart }; Column < ColumnEnd; ++Column)
				{
					auto Q = Column - (Row - (Row & 1)) / 2;
					auto R = Row;
					AddTile(Offset, RandomStream, Q, R);
				}
			}
		}
	}

	if (CreateMesh)
	{
		ALandDynamicMeshActor* LandDynamicMeshActor = nullptr;
		for (TActorIterator<ALandDynamicMeshActor> It(GetWorld()); It; ++It)
		{
			It->GenerateLand(GridTiles, GetGridRotator(), TileConfig.TileSize);
			LandDynamicMeshActor = *It;
			break;
		}

		if (LandDynamicMeshActor)
		{
			auto Location = LandDynamicMeshActor->GetActorLocation();
			LandDynamicMeshActor->SetActorLocation(FVector(Location.X, Location.Y, 5.f));
	
			// 需要改变一次位置， 才能使得NavMesh被更新， 官方项目也是这么做的
			LandDynamicMeshActor->SetActorLocation(FVector(Location.X, Location.Y, 1.f));
		}else
		{
			UE_LOG(LogHexGrid, Error, TEXT("LandDynamicMeshActor is nullptr, 在场景中放入LandDynamicMeshActor"));
		}
	}
}

void AHexGrid::AddTile(const FRotator& Offset,const FRandomStream& RandomStream, int32 Q, int32 R)
{
	FHCubeCoord CCoord{FIntVector(Q, R, -Q - R)};
	// Todo: 线框模式， 只绘制线框
	// 世界坐标
	auto TileLocation = HexToWorld(CCoord);
	// 随机block (Weight表示成为障碍物的概率)
	bool IsBlocking = TileConfig.BlockWeight <= 0.f ? false : TileConfig.BlockWeight >= RandomStream.FRandRange(0.0f, 1.0f);

	float Height = TileConfig.MinHeight;
	switch (TileConfig.HeightRandomType) {
		case EHTileHeightRandomType::NONE:
			break;
		case EHTileHeightRandomType::RDHeightArea:
			if (RdHeightAreaCoords.Contains(CCoord))
			{
				// 这里是高地
				Height = TileConfig.MaxHeight;
			}
			break;
	}
	
	float Cost = TileConfig.MinCost;
	switch (TileConfig.CostRandomType) {
		case EHTileCostRandomType::NONE:
			break;
		case EHTileCostRandomType::NOISE:
			{
				auto Noise = FastNoiseWrapper->GetNoise2D(TileLocation.X, TileLocation.Y);

				if (Noise > 1.f || Noise < -1.f)
				{
					UE_LOG(LogHexGrid, Error, TEXT("Noise is out of range: %f"), Noise);
					Noise = 1.f;
				}
				else if (Noise < -1.f)
				{
					UE_LOG(LogHexGrid, Error, TEXT("Noise is out of range: %f"), Noise);
					Noise = -1.f;
				}

				// 将Noise值映射到TileConfig.MinCost, TileConfig.MaxCost之间
				// 将噪声值映射到0到1之间
				float normalizedNoise = (Noise + 1) / 2;
				// 使用线性插值将噪声值映射到TileConfig.MinCost和TileConfig.MaxCost之间
				if (TileConfig.bDiscreteCost)
				{
					Cost = FMath::RoundToInt(FMath::Lerp(TileConfig.MinCost, TileConfig.MaxCost, normalizedNoise));
				}
				else
				{
					Cost = FMath::Lerp(TileConfig.MinCost, TileConfig.MaxCost, normalizedNoise);
				}

				// // 目前，噪声算法中，高度和Cost使用了一个噪声值，Todo: 考虑是否应该分开
				//
				// float NoiseHeight = FMath::Lerp(TileConfig.MinHeight, TileConfig.MaxHeight, normalizedNoise);
				// Height = IsBlocking ? TileConfig.BlockHeight : NoiseHeight * NoiseScale;
			}
			break;
		case EHTileCostRandomType::SIMPLE:
			{
				if (TileConfig.bDiscreteCost)
				{
					Cost = FMath::RoundToInt(RandomStream.FRandRange(TileConfig.MinCost, TileConfig.MaxCost));
				}
				else
				{
					Cost = RandomStream.FRandRange(TileConfig.MinCost, TileConfig.MaxCost);
				}
			}
			break;
	}
	
	check(Cost >= 1.f);
	// 创建HexTile
	FHexTile Tile;
	Tile.CubeCoord = CCoord;
	Tile.WorldPosition = TileLocation;
	Tile.Cost = Cost;
	Tile.bIsBlocking = IsBlocking;
	Tile.Height = Height;
	Tile.EnvironmentType = IsBlocking?EHTileEnvironmentType::OBSTACLE : Cost2Environment[FMath::RoundToInt(Cost)];
	// UE_LOG(LogHexGrid, Log, TEXT("Cost: %f, CCoord: %s"), Cost, *CCoord.ToString());
	GridTiles.Add(Tile);
	auto ScaleXY = TileConfig.TileSize / DefaultHexMeshSize;
	auto Transform = FTransform(Offset, TileLocation, FVector(ScaleXY, ScaleXY, 1));
	float ScaleZ =  (Height - 1) * TileConfig.RenderHeightScale;
	ScaleZ = ScaleZ<=0.1f? 1.0f : ScaleZ;
	
	if (DrawWireframe)
	{
		FTransform WireFrameTransform = FTransform(Offset, TileLocation + FVector(0.f, 0.f, ScaleZ * TileConfig.BaseRenderHeight + WireframeOffsetZ), FVector(ScaleXY, ScaleXY, 1));
		auto Index = HexGridWireframe->AddInstance(WireFrameTransform, true);
		HexGridWireframe->SetCustomData(Index, WireframeDefaultCustomData);
	}

	if (DrawFaceIndicator)
	{
		FTransform FaceIndicatorTransform = FTransform(Offset, TileLocation + FVector(0.f, 0.f, ScaleZ * TileConfig.BaseRenderHeight + FaceIndicatorOffsetZ), FVector(ScaleXY, ScaleXY, 1));
		auto Index = HexGridFaceIndicator->AddInstance(FaceIndicatorTransform, true);
		HexGridFaceIndicator->SetCustomData(Index, FaceIndicatorDefaultCustomData);
	}

	if (DrawLand)
	{
		// 目前默认的Tile的Mesh是以100为标准制作的， 因此这里按照100进行缩放
		// Block的设置Z的Scale为BlockHeight， 普通区块设置为Cost * 2.f
		
		Transform.SetScale3D(FVector(ScaleXY, ScaleXY, ScaleZ));
		auto Index = HexGridLand->AddInstance(Transform, true);

		const auto& Environment = Environments[Tile.EnvironmentType];
		TArray<float> CustomData {Environment.CustomData.R, Environment.CustomData.G, Environment.CustomData.B, Environment.CustomData.A};
		HexGridLand->SetCustomData(Index, CustomData);
		if (Environment.DecorationMesh.IsValid())
		{
			auto EnvironmentMesh = EnvironmentMeshes[Tile.EnvironmentType];
			auto RDCount = RandomStream.RandRange(1, Environment.DecorationMaxCount);
			for (int32 i = 0; i < RDCount; ++i)
			{
				FTransform ItemTransform;
				if (Environment.bRandomDecorationRotation)
				{
					ItemTransform.SetRotation(FQuat(FRotator(0.f, RandomStream.FRandRange(0.f, 360.f), 0.f)));
				}

				if (Environment.bRandomDecorationLocation)
				{
					// 随机XY平面的位置， 范围是TileSize的一半
					auto RandomX = RandomStream.FRandRange(-TileConfig.TileSize / 2.f, TileConfig.TileSize / 2.f);
					auto RandomY = RandomStream.FRandRange(-TileConfig.TileSize / 2.f, TileConfig.TileSize / 2.f);
					auto RandomLocation = FVector(RandomX, RandomY, 0.f);
					ItemTransform.SetLocation(TileLocation + RandomLocation + FVector(0.f, 0.f, ScaleZ * TileConfig.BaseRenderHeight));
				}
				else
				{
					ItemTransform.SetLocation(TileLocation+ FVector(0.f, 0.f, ScaleZ * TileConfig.BaseRenderHeight));
				}

				ItemTransform.SetScale3D(FVector(Environment.DecorationScale));
				EnvironmentMesh->AddInstance(ItemTransform, true);
			}
		}
	}
}

FRotator AHexGrid::GetGridRotator() const
{
	if (TileConfig.TileOrientation == EHTileOrientationFlag::FLAT)
	{
		return FlatRotator;
	}
	else if (TileConfig.TileOrientation == EHTileOrientationFlag::POINTY)
	{
		return  PointyRotator;
	}

	return FRotator::ZeroRotator;
}

void AHexGrid::UpdateHitLocation(const FVector& InHitLocation)
{
	MouseHitLocation = InHitLocation;

	if (TileConfig.BaseOnRadius)
	{
		MouseHitColumn = 0;
		MouseHitRow = 0;
	}
	else
	{
		const auto& HexCoord = WorldToHex(InHitLocation);
		if (TileConfig.TileOrientation == EHTileOrientationFlag::FLAT)
		{
			MouseHitColumn = HexCoord.QRS.X;
			MouseHitRow = HexCoord.QRS.Y + (HexCoord.QRS.X - (HexCoord.QRS.X & 1)) / 2;
		}
		else if (TileConfig.TileOrientation == EHTileOrientationFlag::POINTY)
		{
			MouseHitColumn = HexCoord.QRS.X + (HexCoord.QRS.Y - (HexCoord.QRS.Y & 1)) / 2;
			MouseHitRow = HexCoord.QRS.Y;
		}
	}
}

bool AHexGrid::TryGetHexXYToWorld(int Row, int Column, FVector& OutWorldPosition)
{
	auto Key = GetHexCoordByXY(Row, Column);
	if (GetHexTileIndex(Key) != INDEX_NONE)
	{
		OutWorldPosition = HexToWorld(Key);
		return true;
	}

	return false;
}

FVector AHexGrid::HexXYToWorld(int Row, int Column)
{
	FHCubeCoord CCoord = GetHexCoordByXY(Row, Column);
	return HexToWorld(CCoord);
}

FVector AHexGrid::HexToWorld(const FHCubeCoord& H)
{
	// Set the layout orientation
	FHTileOrientation TileOrientation;
	if (TileConfig.TileOrientation == EHTileOrientationFlag::FLAT)
	{
		TileOrientation = HFlatTopLayout;
	}
	else
	{
		TileOrientation = HPointyLayout;
	}

	float x = ((TileOrientation.f0 * H.QRS.X) + (TileOrientation.f1 * H.QRS.Y)) * TileConfig.TileSize;
	float y = ((TileOrientation.f2 * H.QRS.X) + (TileOrientation.f3 * H.QRS.Y)) * TileConfig.TileSize;

	return FVector(x + TileConfig.Origin.X, y + TileConfig.Origin.Y, TileConfig.Origin.Z);
}

float AHexGrid::GetTileHeightByCoord(const FHCubeCoord& H)
{
	auto Index = GetHexTileIndex(H);
	return GetTileHeightByIndex(Index);
}

float AHexGrid::GetTileHeightByIndex(int Index)
{
	if (Index == INDEX_NONE)
	{
		return 0.f;
	}

	const auto& Tile = GridTiles[Index];
	float ScaleZ =  (Tile.Height - 1) * TileConfig.RenderHeightScale;
	ScaleZ = ScaleZ<=0.1f? 1.0f : ScaleZ;
	return ScaleZ * TileConfig.BaseRenderHeight;
}

FHCubeCoord AHexGrid::WorldToHex(const FVector &Location)
{
	// Set the layout orientation
	FHTileOrientation TileOrientation;
	if (TileConfig.TileOrientation == EHTileOrientationFlag::FLAT)
	{
		TileOrientation = HFlatTopLayout;
	}
	else
	{
		TileOrientation = HPointyLayout;
	}

	FVector InternalLocation{ FVector((Location.X - TileConfig.Origin.X) / TileConfig.TileSize,
									  (Location.Y - TileConfig.Origin.Y) / TileConfig.TileSize,
									  (Location.Z - TileConfig.Origin.Z))	// Z is useless here.
	};

	float q = ((TileOrientation.b0 * InternalLocation.X) + (TileOrientation.b1 * InternalLocation.Y));
	float r = ((TileOrientation.b2 * InternalLocation.X) + (TileOrientation.b3 * InternalLocation.Y));
	
	FVector v{ (TileConfig.TileOrientation == EHTileOrientationFlag::FLAT) ? FVector(q, -q - r, r) : FVector(q, r, -q - r) };

	return HexRound(FHFractional(v));
}

FVector AHexGrid::SnapToGrid(const FVector &Location)
{
	const double TempZ{Location.Z};
	FVector Result{ HexToWorld(WorldToHex(Location)) };
	Result.Z = TempZ;
	return Result;
}

FHCubeCoord AHexGrid::HexRound(const FHFractional &F)
{
	int32 q{ int32(FMath::RoundToDouble(F.QRS.X)) };
	int32 r{ int32(FMath::RoundToDouble(F.QRS.Y)) };
	int32 s{ int32(FMath::RoundToDouble(F.QRS.Z)) };

	const double q_diff{ FMath::Abs(q - F.QRS.X) };
	const double r_diff{ FMath::Abs(r - F.QRS.Y) };
	const double s_diff{ FMath::Abs(s - F.QRS.Z) };

	if ((q_diff > r_diff) && (q_diff > s_diff))
	{
		q = -r - s;
	}
	else if (r_diff > s_diff)
	{
		r = -q - s;
	}
	else
	{
		s = -q - r;
	}

	return FHCubeCoord{ FIntVector(q, r, s) };
}

bool AHexGrid::HexEqual(const FHCubeCoord &A, const FHCubeCoord &B)
{
	return A == B;
}

FHCubeCoord AHexGrid::GetDirection(int32 Dir)
{
	check(Dir < HDirections.Directions.Num());
	return HDirections.Directions[Dir];
}

FHCubeCoord AHexGrid::GetNeighbor(const FHCubeCoord &H, const FHCubeCoord &Dir)
{
	return H + Dir;
}

TArray<FHCubeCoord> AHexGrid::GetRangeCoords(const FHCubeCoord& Center, int32 Radius, bool OnlyExact) const
{
	TArray<FHCubeCoord> Result;

	for (int32 dq = -Radius; dq <= Radius; ++dq)
	{
		for (int32 dr = FMath::Max(-Radius, -dq - Radius); dr <= FMath::Min(Radius, -dq + Radius); ++dr)
		{
			int32 ds = -dq - dr;
			if (OnlyExact)
			{
				if (FMath::Max3(FMath::Abs(dq), FMath::Abs(dr), FMath::Abs(ds)) != Radius)
				{
					continue;
				}
			}

			Result.Add(FHCubeCoord{FIntVector(Center.QRS.X + dq, Center.QRS.Y + dr, Center.QRS.Z + ds)});
		}
	}
	
	return Result;
}

TArray<FHexTile> AHexGrid::GetRange(const FHCubeCoord& Center, int32 Radius)
{
	TArray<FHexTile> Result;

	const TArray<FHCubeCoord>& Coords = GetRangeCoords(Center, Radius);
	for (const auto& Coord : Coords)
	{
		auto Index = GetHexTileIndex(Coord);
		if (Index != INDEX_NONE)
		{
			Result.Add(GridTiles[Index]);
		}
	}
	
	return Result;
}


bool AHexGrid::InRange(const FHCubeCoord& Center, const FHCubeCoord& Target, int32 Radius)
{
	return FMath::Abs(Center.QRS.X - Target.QRS.X) <= Radius &&
		FMath::Abs(Center.QRS.Y - Target.QRS.Y) <= Radius &&
		FMath::Abs(Center.QRS.Z - Target.QRS.Z) <= Radius;
}

bool AHexGrid::IsTileReachable(const FVector& Location)
{
	const auto& HexCoord = WorldToHex(Location);
	auto Index = GetHexTileIndex(HexCoord);
	if (Index == INDEX_NONE)
	{
		return false;
	}
	
	const auto& Tile = GridTiles[Index];
	return !Tile.bIsBlocking;
}

int AHexGrid::GetHexTileIndex(const FHCubeCoord& InCoord) const
{
	auto Q = InCoord.QRS.X;
	auto R = InCoord.QRS.Y;
	int32 PassedCount;
	if (TileConfig.BaseOnRadius)
	{
		// Q,R,S中的最大值就是距离中心的距离
		if (FMath::Abs(Q) > TileConfig.Radius || FMath::Abs(R) > TileConfig.Radius || FMath::Abs(-Q - R) > TileConfig.Radius)
		{
			return INDEX_NONE;
		}
		
		// 根据AddTile的添加顺序，逆推
		auto Radius = TileConfig.Radius;
		
		int32 Start{FMath::Max(-Radius, -Q - Radius)};
		
		auto DeltaQ = Q + Radius;
		
		// 实际是等差数列，优化为下列的数学公式
		if (DeltaQ <= Radius + 1)
		{
			PassedCount = ( Radius + 1 + Radius + DeltaQ) * (DeltaQ) / 2;
		}
		else
		{
			PassedCount = (Radius + 1 + 2 * Radius + 1) * (Radius + 1) / 2;
			PassedCount += (5 * Radius - DeltaQ + 2) * (DeltaQ - Radius - 1) / 2;
		}
		
		return PassedCount + R - Start;
	}
	
	const auto RowStart = -FMath::FloorToInt(TileConfig.Row / 2.f);
	const auto RowEnd = FMath::CeilToInt(TileConfig.Row / 2.f);
	const auto ColumnStart = -FMath::FloorToInt(TileConfig.Column / 2.f);
	const auto ColumnEnd = FMath::CeilToInt(TileConfig.Column / 2.f);
	if (TileConfig.TileOrientation == EHTileOrientationFlag::FLAT)
	{
		auto Row = R + (Q - (Q & 1)) / 2;
		if (Q < ColumnStart || Q >= ColumnEnd || Row < RowStart || Row >= RowEnd)
		{
			return INDEX_NONE;
		}
		
		auto DeltaCol = Q - ColumnStart;
		PassedCount = DeltaCol * TileConfig.Row;
		
		return PassedCount + Row - RowStart;
	}
	
	if (TileConfig.TileOrientation == EHTileOrientationFlag::POINTY)
	{
		auto Col = Q + (R - (R&1)) / 2;
		if (Col < ColumnStart || Col >= ColumnEnd || R < RowStart || R >= RowEnd)
		{
			return INDEX_NONE;
		}
		
		auto DeltaRow = R - RowStart;
		PassedCount = DeltaRow * TileConfig.Column;
		
		return PassedCount + Col - ColumnStart;
	}

	UE_LOG(LogHexGrid, Error, TEXT("[AHexGrid] GetHexTileIndex Failed, Return -1"));
	return INDEX_NONE;
}

FHCubeCoord AHexGrid::GetHexCoordByIndex(int Index) const
{
	if (Index < 0 || Index >= GridTiles.Num())
	{
		UE_LOG(LogHexGrid, Error, TEXT("[AHexGrid] GetHexCoordByIndex 查询越界的Index, Index: %d"), Index);
		return FHCubeCoord{FIntVector::ZeroValue};;
	}

	return GridTiles[Index].CubeCoord;
	// if (TileConfig.BaseOnRadius)
	// {
	// 	// 从Index逆推出Q,R
	// 	auto Radius = TileConfig.Radius;
	// 	int32 DeltaQ = 0;
	// 	int32 PassedCount = 0;
	//
	// 	// Find the maximum DeltaQ that satisfies PassedCount <= Index
	// 	while (true)
	// 	{
	// 		int32 NextPassedCount;
	// 		if (DeltaQ <= Radius + 1)
	// 		{
	// 			NextPassedCount = (Radius + 1 + Radius + DeltaQ) * (DeltaQ) / 2;
	// 		}
	// 		else
	// 		{
	// 			NextPassedCount = (Radius + 1 + 2 * Radius + 1) * (Radius + 1) / 2;
	// 			NextPassedCount += (5 * Radius - DeltaQ + 2) * (DeltaQ - Radius - 1) / 2;
	// 		}
	//
	// 		if (NextPassedCount > Index)
	// 		{
	// 			break;
	// 		}
	//
	// 		PassedCount = NextPassedCount;
	// 		DeltaQ++;
	// 	}
	// 	// Calculate Q
	// 	int32 Q = DeltaQ - Radius - 1;
	// 	int32 Start{FMath::Max(-Radius, -Q - Radius)};
	//
	// 	// Calculate R
	// 	int32 R = Index - PassedCount + Start;
	// 	return FHCubeCoord{FIntVector(Q, R, -Q - R)};
	// }
	//
	// const auto RowStart = -FMath::FloorToInt(TileConfig.Row / 2.f);
	// const auto ColumnStart = -FMath::FloorToInt(TileConfig.Column / 2.f);
	// if (TileConfig.TileOrientation == EHTileOrientationFlag::FLAT)
	// {
	// 	int32 DeltaCol = Index / TileConfig.Row;
	// 	int32 Col = DeltaCol + ColumnStart;
	// 	int32 Row = Index % TileConfig.Row + RowStart;
	// 	// UE_LOG(LogHexGrid, Log, TEXT("Row: %d, Col: %d"), Row, Col);
	// 	int32 R = Row - (Col - (Col & 1)) / 2;
	// 	return FHCubeCoord{FIntVector(Col, R, -Col - R)};
	// }
	//
	// if (TileConfig.TileOrientation == EHTileOrientationFlag::POINTY)
	// {
	// 	int32 DeltaRow = Index / TileConfig.Column;
	// 	int32 Row = DeltaRow + RowStart;
	// 	int32 Col = Index % TileConfig.Column + ColumnStart;
	// 	// UE_LOG(LogHexGrid, Log, TEXT("Row: %d, Col: %d"), Row, Col);
	// 	int32 Q = Col - (Row - (Row & 1)) / 2;
	// 	return FHCubeCoord{FIntVector(Q, Row, -Q - Row)};
	// }
	//
	// UE_LOG(LogHexGrid, Error, TEXT("[AHexGrid] GetHexCoordByIndex Failed, HexGrid配置错误"));
	// return FHCubeCoord{FIntVector::ZeroValue};
}

const FHexTile& AHexGrid::GetHexTile(const FVector& InLocation)
{
	return GetHexTile(WorldToHex(InLocation));
}

const FHexTile& AHexGrid::GetHexTile(const FHCubeCoord& InCoord)
{
	auto Index = GetHexTileIndex(InCoord);
	if (Index != INDEX_NONE)
	{
		return GridTiles[Index];
	}

	UE_LOG(LogHexGrid, Error, TEXT("[AHexGrid] GetHexTile Failed, Return EmptyHexTile"));
	return EmptyHexTile;
}

FHexTile& AHexGrid::GetMutableHexTile(const FHCubeCoord& InCoord)
{
	auto Index = GetHexTileIndex(InCoord);
	if (Index != INDEX_NONE)
	{
		return GridTiles[Index];
	}

	UE_LOG(LogHexGrid, Error, TEXT("[AHexGrid] GetMutableHexTile Failed, Return EmptyHexTile"));
	return EmptyHexTile;
}

int AHexGrid::GetDistanceByIndex(int A, int B) const
{
	if (GridTiles.IsValidIndex(A) && GridTiles.IsValidIndex(B))
	{
		return GetDistance(GridTiles[A].CubeCoord, GridTiles[B].CubeCoord);
	}

	return INT_MAX;
}

void AHexGrid::SetWireFrameColor(int Index, const FLinearColor& InColor, float NewHeightOffset)
{
	TArray<float> CustomData {InColor.R, InColor.G, InColor.B, InColor.A};
	HexGridWireframe->SetCustomData(Index, CustomData);
	auto Height = GetTileHeightByIndex(Index);
	const auto& Tile = GridTiles[Index];
	FTransform Transform ;
	HexGridWireframe->GetInstanceTransform(Index, Transform, true);
	Transform.SetLocation(Tile.WorldPosition + FVector(0.f, 0.f, Height + WireframeOffsetZ + NewHeightOffset));
	HexGridWireframe->UpdateInstanceTransform(Index,Transform, true);
}

void AHexGrid::SetFaceIndicatorColor(int Index, const FLinearColor& InColor, float NewHeightOffset)
{
	TArray<float> CustomData {InColor.R, InColor.G, InColor.B, InColor.A};
	HexGridFaceIndicator->SetCustomData(Index, CustomData);
	auto Height = GetTileHeightByIndex(Index);
	const auto& Tile = GridTiles[Index];
	FTransform Transform ;
	HexGridFaceIndicator->GetInstanceTransform(Index, Transform, true);
	Transform.SetLocation(Tile.WorldPosition + FVector(0.f, 0.f, Height + FaceIndicatorOffsetZ + NewHeightOffset));
	HexGridFaceIndicator->UpdateInstanceTransform(Index,Transform, true);
}

FHCubeCoord AHexGrid::GetHexCoordByXY(int32 Row, int32 Column) const
{
	if (TileConfig.BaseOnRadius)
	{
		return FHCubeCoord{FIntVector(Column, Row, -Column - Row)};
	}

	if (TileConfig.TileOrientation == EHTileOrientationFlag::FLAT)
	{
		return FHCubeCoord{FIntVector(Column, Row - (Column - (Column & 1)) / 2, Row)};
	}
	
	if (TileConfig.TileOrientation == EHTileOrientationFlag::POINTY)
	{
		return FHCubeCoord{FIntVector(Column - (Row - (Row & 1)) / 2, Row, Column)};
	}

	UE_LOG(LogHexGrid, Error, TEXT("[AHexGrid] GetHexCoordByXY Failed, Return ZeroValue"));
	return FHCubeCoord{FIntVector::ZeroValue};
}

