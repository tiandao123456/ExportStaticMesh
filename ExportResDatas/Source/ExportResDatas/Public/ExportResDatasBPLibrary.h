// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"
#include "Engine.h"
#include "Serialization/Archive.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ExportResDatasBPLibrary.generated.h"

//struct FVertexStruct
//{
//	//顶点的索引
//	int32 Index = 0;
//
//	//顶点的位置，初始化为(0,0,0)
//	FVector Position = FVector::ZeroVector;
//
//	//顶点的法线,初始化为(0,0,0)
//	FVector Normal = FVector::ZeroVector;
//
//	//顶点的纹理坐标，为什么有三个?
//	FVector2D TexCoord0 = FVector2D::ZeroVector;
//
//	FVector2D TexCoord1 = FVector2D::ZeroVector;
//
//	FVector2D TexCoord2 = FVector2D::ZeroVector;
//};

USTRUCT(BlueprintType)
struct FStaticMeshData
{
	GENERATED_USTRUCT_BODY()

		//顶点数量
	UPROPERTY(BlueprintReadOnly, Category = "StaticMesh")
		int32 VerticesNum;

	//索引数组
	UPROPERTY(BlueprintReadOnly, Category = "StaticMesh")
		TArray<int32> Indices;

	////文本对应
	//UPROPERTY(BlueprintReadOnly, Category = "StaticMesh")
	//	FString VsFormat;

	UPROPERTY(BlueprintReadOnly, Category = "StaticMesh")
		TArray<float> Vertices;

public:
	FStaticMeshData() = default;
	FStaticMeshData(TArray<float> Vertices, TArray<int32> Indices);
};

USTRUCT(BlueprintType)
struct FCameraInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Camera")
		FVector Location;

	UPROPERTY(BlueprintReadOnly, Category = "Camera")
		FVector Target;

	UPROPERTY(BlueprintReadOnly, Category = "Camera")
		FVector Rotator;

	UPROPERTY(BlueprintReadOnly, Category = "Camera")
		float Fov;

	UPROPERTY(BlueprintReadOnly, Category = "Camera")
		float Aspect;

public:
	FCameraInfo() : Location(FVector()), Target(FVector()), Rotator(FVector()), Fov(0.f), Aspect(0.f) {}
};

USTRUCT(BlueprintType)
struct FWorldCoordinateInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "World")
		FVector Location;

	UPROPERTY(BlueprintReadOnly, Category = "World")
		FRotator Rotation;

	UPROPERTY(BlueprintReadOnly, Category = "World")
		FVector Scale;

	UPROPERTY(BlueprintReadOnly, Category = "World")
		FVector4 MatrixCol1;

	UPROPERTY(BlueprintReadOnly, Category = "World")
		FVector4 MatrixCol2;

	UPROPERTY(BlueprintReadOnly, Category = "World")
		FVector4 MatrixCol3;

	UPROPERTY(BlueprintReadOnly, Category = "World")
		FVector4 MatrixCol4;
};

//namespace RE
//{
//	//顶点
//	struct FStaticVertex_RE
//	{
//		//顶点位置、法线、纹理坐标、顶点颜色
//		FVector Position;
//		FVector TangentZ;
//		FVector2D UV0;
//		FVector4 Color;
//	};
//
//	struct FStaticMesh_Lod_RE
//	{
//		int32 VertStride;
//		TArray<FStaticVertex_RE> Vertice;
//		TArray<int32> Indices;
//	};
//}
//
//using namespace RE;
/**
 *
 */

UCLASS()
class UExportResDatasBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	//导出静态网格体数据
	UFUNCTION(BlueprintCallable, Category = "ResExport")
		static void ExportStaticMesh(const UStaticMesh* StaticMesh, FString OutputPath = TEXT(""), const FString& Filename = TEXT("StaticMeshMessage"));

	//将网格体数据以json的形式写到文件中
	UFUNCTION(BlueprintCallable, Category = "ResExport")
		static void WriteFile(const FString& FileString, FString OutputPath, const FString& Filename);

	//获得静态网格体的顶点数据
	UFUNCTION(BlueprintCallable, Category = "ResExport")
		static void GetStaticMeshVerticesData(const UStaticMesh* StaticMesh, TArray<float>& Output);

	//获得静态网格体的索引数据
	UFUNCTION(BlueprintCallable, Category = "ResExport")
		static void GetStaticMeshIndicesData(const UStaticMesh* StaticMesh, TArray<int32>& Output);

	//导出相机的数据
	UFUNCTION(BlueprintCallable, Category = "ResExport", meta = (WorldContext = WorldContextObject))
		static void ExportCamera(const UObject* WorldContextObject, const UCameraComponent* CameraComponent, FString OutputPath = TEXT(""), const FString& Filename = TEXT("CameraDataMessage"));

	//导出静态网格体的平移旋转缩放等信息
	UFUNCTION(BlueprintCallable, Category = "ResExport", meta = (WorldContext = WorldContextObject))
		static void ExportActorWorldCoordinate(const AActor* StaticMeshActor, FString OutputPath = TEXT(""), const FString& Filename = TEXT("WorldCoordinate"));


	//函数模板
	template <typename TargetStruct>
	static void ExportStructByJsonConverter(const TargetStruct& Target, FString OutputPath, const FString& Filename )
	{
		FString OutputString;
		//将自定义结构体转换为Json字符串
		//按照struct的结构写入
		FJsonObjectConverter::UStructToJsonObjectString(Target, OutputString);
		//输出到指定文件中
		WriteFile(OutputString, OutputPath, Filename);
	}

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Execute Sample function", Keywords = "ExportResDatas sample test testing"), Category = "ExportResDatasTesting")
		static float ExportResDatasSampleFunction(float Param);
};
