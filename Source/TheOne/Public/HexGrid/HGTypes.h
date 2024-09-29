#pragma once

#include "CoreMinimal.h"
#include "HGTypes.generated.h"

/**
 * Orientation of the tile
 * @see https://www.redblobgames.com/grids/hexagons/#basics
 */
UENUM(BlueprintType)
enum class EHTileOrientationFlag : uint8
{
	FLAT UMETA(DisplayName = "平顶"),
	POINTY UMETA(DisplayName = "尖顶"),
	NONE
};

UENUM(BlueprintType)
enum class EHTileHeightRandomType : uint8
{
	NONE UMETA(DisplayName = "无高度差"),
	// NOISE UMETA(DisplayName = "噪声"), TODO: 噪声高度差，暂时没必要
	RDHeightArea UMETA(DisplayName = "随机高度区块")
};

UENUM(BlueprintType)
enum class EHTileCostRandomType : uint8
{
	NONE UMETA(DisplayName = "总是最小Cost"),
	SIMPLE UMETA(DisplayName = "简单随机"),
	NOISE UMETA(DisplayName = "噪声"),
	// Todo: 增加一种，Cost（地形） 与高度相关的自定义随机算法
};

UENUM(BlueprintType)
enum class EHAStarHeuristicCostType : uint8
{
	/** 最短路径， 总是返回1
	 * 如果h(n)始终小于等于节点n到终点的代价，则A*算法保证一定能够找到最短路径。但是当h(n)的值越小，算法将遍历越多的节点，也就导致算法越慢。
	 */
	Nearest UMETA(DisplayName = "最短路径"),
	// 使用六边形距离作为启发函数，可以更准确地估计从当前节点到目标节点的成本，从而更有可能找到最佳路径。
	Manhattan UMETA(DisplayName = "曼哈顿距离"),
	// 如果一定要找到最短路径，那么需要使得h(n)完全等于节点n到终点的代价，可惜的是，并非所有场景下都能做到这一点。因为在没有达到终点之前，我们很难确切算出距离终点还有多远。
	// 但是当h(n)的值越接近节点n到终点的代价，A*算法的效率就越高， 获取最佳路径的可能性也就越大。
	// 为此， 增加一个稍微优化一点方案， Manhattan + 终点格子的Cost
	ManhattanAndCost UMETA(DisplayName = "曼哈顿距离+终点Cost"),
};

/**
 * Cost type for A* traversal
 */
UENUM(BlueprintType)
enum class EHAStarTraversalCostType : uint8
{
	//仅使用Tile本身的Cost
	TileCost,
	// 使用Tile本身的Cost + Tile之间高度差带来的Cost
	TileAndHeightCost,
};

USTRUCT(BlueprintType)
struct FRDHeightAreaConfig
{
	GENERATED_BODY()

	FRDHeightAreaConfig(): AreaRadius(0), CoreCount(0), CoreMinDistance(0), MinRadius(0), MaxRadius(0), EmptyWeight(0)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="区块生成范围半径"))
	int AreaRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="区块数量"))
	int CoreCount;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="区块核心间最小距离"))
	int CoreMinDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="区块范围最小半径"))
	int MinRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="区块范围最大半径"))
	int MaxRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="区块内空地概率"))
	float EmptyWeight;
};


/**
 * Information about the tile orientation, size and origin.
 * @see https://www.redblobgames.com/grids/hexagons/implementation.html#layout
 */
USTRUCT(BlueprintType)
struct FHTileConfig
{
	GENERATED_USTRUCT_BODY()

	FHTileConfig() {}

	FHTileConfig(EHTileOrientationFlag orientation, float size, FVector origin)
		: TileOrientation(orientation), TileSize(size), Origin(origin)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EHTileOrientationFlag TileOrientation {
		EHTileOrientationFlag::NONE
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TileSize{ 0 };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Origin {
		FVector::ZeroVector
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool BaseOnRadius{ false };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EHAStarHeuristicCostType HeuristicCostType {EHAStarHeuristicCostType::Nearest};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EHAStarTraversalCostType TraversalCostType {EHAStarTraversalCostType::TileCost};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="地形高度随机算法"))
	EHTileHeightRandomType HeightRandomType {EHTileHeightRandomType::NONE};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="Cost随机算法"))
	EHTileCostRandomType CostRandomType {EHTileCostRandomType::NONE};
	
	/**
	 * Radius of the grid in "tiles", clamped [1, 25]
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Radius {1};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Row{1};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Column{1};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="障碍物概率"))
	float BlockWeight {0.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BlockHeight {50.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="是否离散分布Cost"))
	bool bDiscreteCost {false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite,meta =(ClampMin = 1))
	float MinCost {1.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite,meta =(ClampMin = 1))
	float MaxCost {1.f};

	// 地块的高度
	UPROPERTY(EditAnywhere, BlueprintReadWrite,meta =(ClampMin = 1))
	float MinHeight {1.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite,meta =(ClampMin = 1))
	float MaxHeight {1.f};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName="高度缩放比例"))
	float RenderHeightScale {1.f};
};

/**
 * @see https://www.redblobgames.com/grids/hexagons/#coordinates
 * @see https://www.redblobgames.com/grids/hexagons/implementation.html#hex
 */
USTRUCT(BlueprintType)
struct FHAxialCoord
{
	GENERATED_USTRUCT_BODY()

	FHAxialCoord() {}

	FHAxialCoord(int32 q, int32 r)
	{
		QR.X = q;
		QR.Y = r;
	}

	FHAxialCoord(FIntPoint _qr) : QR(_qr)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GraphAStarExample|HGTypes|Axial Coord")
	FIntPoint QR {FIntPoint::ZeroValue};

};

/**
 * @see https://www.redblobgames.com/grids/hexagons/#coordinates
 * @see https://www.redblobgames.com/grids/hexagons/implementation.html#hex
 */
USTRUCT(BlueprintType)
struct FHCubeCoord
{
	GENERATED_USTRUCT_BODY()
	
	static const FHCubeCoord Invalid;

	FHCubeCoord() {}

	FHCubeCoord(int32 q, int32 r, int32 s)
	{
		check(q + r + s == 0);
		QRS.X = q;
		QRS.Y = r;
		QRS.Z = s;
	}

	FHCubeCoord(FIntVector _v) : QRS(_v) {}


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GraphAStarExample|HGTypes|Cube Coord")
	FIntVector QRS {
		FIntVector::ZeroValue
	};


	friend FHCubeCoord operator+(const FHCubeCoord &lhs, const FHCubeCoord &rhs)
	{
		return FHCubeCoord{ lhs.QRS + rhs.QRS };
	}

	friend FHCubeCoord operator-(const FHCubeCoord &lhs, const FHCubeCoord &rhs)
	{
		return FHCubeCoord{ lhs.QRS - rhs.QRS };
	}

	friend FHCubeCoord operator*(const FHCubeCoord &lhs, int32 k)
	{
		return FHCubeCoord{ lhs.QRS * k };
	}

	friend bool operator==(const FHCubeCoord &lhs, const FHCubeCoord &rhs)
	{
		return lhs.QRS == rhs.QRS;
	}

	friend bool operator!=(const FHCubeCoord &lhs, const FHCubeCoord &rhs)
	{
		return lhs.QRS != rhs.QRS;
	}

	friend uint32 GetTypeHash(const FHCubeCoord &Other)
	{
		FString TypeHash{ Other.QRS.ToString() };
		return GetTypeHash(TypeHash);
	}

	FString ToString() const
	{
		FString Ret;
		Ret.Append("{");
		Ret.Append(FString::FromInt(QRS.X));
		Ret.Append(", ");
		Ret.Append(FString::FromInt(QRS.Y));
		Ret.Append(", ");
		Ret.Append(FString::FromInt(QRS.Z));
		Ret.Append("}");
		return Ret;
	}
};


/**
 * @see https://www.redblobgames.com/grids/hexagons/implementation.html#fractionalhex
 */
USTRUCT(BlueprintType)
struct FHFractional
{
	GENERATED_USTRUCT_BODY()

	FHFractional() {}

	FHFractional(float q, float r, float s)
	{
		QRS.X = q;
		QRS.Y = r;
		QRS.Z = s;
	}

	FHFractional(FVector _v) : QRS(_v)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GraphAStarExample|HGTypes|Fractional Coord")
	FVector QRS {FVector::ZeroVector};
};

/**
 * @see  https://www.redblobgames.com/grids/hexagons/#neighbors
 */
USTRUCT(BlueprintType)
struct FHDirections
{
	GENERATED_USTRUCT_BODY()

	FHDirections()
	{
															//   Flat			| Pointy
		Directions.Add(FHCubeCoord(FIntVector(0, 1, -1)));	// 0 Top			| Top Left
		Directions.Add(FHCubeCoord(FIntVector(1, 0, -1)));	// 1 Top Right		| Top Right
		Directions.Add(FHCubeCoord(FIntVector(1, -1, 0)));	// 2 Bottom Right	| Right
		Directions.Add(FHCubeCoord(FIntVector(0, -1, 1)));	// 3 Bottom			| Bottom Right
		Directions.Add(FHCubeCoord(FIntVector(-1, 0, 1)));	// 4 Bottom Left	| Bottom Left
		Directions.Add(FHCubeCoord(FIntVector(-1, 1, 0)));	// 5 Top Left		| Left
	}

	UPROPERTY(BlueprintReadOnly, Category = "GraphAStarExample|HGTypes|Directions")
	TArray<FHCubeCoord> Directions;
};

/**
 * @see  https://www.redblobgames.com/grids/hexagons/#neighbors
 */
USTRUCT(BlueprintType)
struct FHDiagonals
{
	GENERATED_USTRUCT_BODY()

	FHDiagonals()
	{
															//   Flat			| Pointy
		Diagonals.Add(FHCubeCoord(FIntVector(1, 1, -2)));	// 0 Top Right		| Top
		Diagonals.Add(FHCubeCoord(FIntVector(2, -1, -1)));	// 1 Right			| Top Right
		Diagonals.Add(FHCubeCoord(FIntVector(1, -2, 1)));	// 2 Bottom Right	| Bottom Right
		Diagonals.Add(FHCubeCoord(FIntVector(-1, -1, 2)));	// 3 Bottom Left	| Bottom
		Diagonals.Add(FHCubeCoord(FIntVector(-2, 1, 1)));	// 4 Left			| Bottom Left
		Diagonals.Add(FHCubeCoord(FIntVector(-1, 2, -1)));	// 5 Top Left		| Top Left
	}

	UPROPERTY(BlueprintReadOnly, Category = "GraphAStarExample|HGTypes|Diagonals")
	TArray<FHCubeCoord> Diagonals;
};

/**
 * @see https://www.redblobgames.com/grids/hexagons/implementation.html#layout
 */
USTRUCT(BlueprintType)
struct FHTileOrientation
{
	GENERATED_USTRUCT_BODY()

	FHTileOrientation()
	{
	}

	friend bool operator==(const FHTileOrientation &lhs, const FHTileOrientation &rhs)
	{
		return (lhs.f0 == rhs.f0) && (lhs.f1 == rhs.f1) && (lhs.f2 == rhs.f2) && (lhs.f3 == rhs.f3);
	}

	double f0, f1, f2, f3;	// f0, f1 for X, f2, f3 for Y				- used in HexToWorld
	double b0, b1, b2, b3;	// Inverse.	Q b0*x, b1*y - R b2*x, b3*y		- used in WorldToHex
};

const struct FHFlatTopOrientation : FHTileOrientation
{
	// Flat top hexagon layout (X = y, Y = -x where uppercase is the original coordinates from Red Blob Games article, lowercase is UE4 coordinates)
	FHFlatTopOrientation()
	{
										// UE4 | Original
		f0 = -FMath::Sqrt(3.0) / 2.0;	// -f2 | f0 = 3/2
		f1 = -FMath::Sqrt(3.0);			// -f3 | f1 = 0
		f2 = 3.0 / 2.0;					//  f0 | f2 = sqrt(3)/2
		f3 = 0.0;						//  f1 | f3 = sqrt(3)

		b0 = 0.0;						// b1 | b0 = 2/3
		b1 = 2.0 / 3.0;					// b0 | b1 = 0
		b2 = FMath::Sqrt(3.0) / 3.0;	// b3 | b2 = -1/3
		b3 = -1.0 / 3.0;				// b2 | b3 = sqrt(3)/3
	}

}HFlatTopLayout;

const struct FHPointyOrientation : FHTileOrientation
{
	FHPointyOrientation()
	{
										// UE4 | Original
		f0 = 0.0;						// -f2 | f0 = sqrt(3)
		f1 = -3.0 / 2.0;				// -f3 | f1 = sqrt(3)/2
		f2 = FMath::Sqrt(3.0);			//  f0 | f2 = 0
		f3 = FMath::Sqrt(3.0) / 2.0;	//  f1 | f3 = 3/2

		b0 = 1.0 / 3.0;					// -b1 | b0 = sqrt(3)/3
		b1 = FMath::Sqrt(3.0) / 3.0;	//  b0 | b1 = -1/3
		b2 = -2.0 / 3.0;				// -b3 | b2 = 0
		b3 = 0.0;						//  b2 | b3 = 2/3
	}

}HPointyLayout;





