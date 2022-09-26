// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"
#include "Engine.h"
#include "Engine/World.h"
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

//在世界场景中的static mesh的顶点、索引信息
//以及该静态网格体在世界场景对应的model矩阵
USTRUCT(BlueprintType)
struct FStaticMeshData
{
	GENERATED_USTRUCT_BODY()

	////顶点数量
	//UPROPERTY(BlueprintReadOnly, Category = "StaticMesh")
	//	int32 VerticesNum;

	//索引数组
	UPROPERTY(BlueprintReadOnly, Category = "StaticMesh")
		TArray<int32> Indices;

	////文本对应
	//UPROPERTY(BlueprintReadOnly, Category = "StaticMesh")
	//	FString VsFormat;

	UPROPERTY(BlueprintReadOnly, Category = "StaticMesh")
		TArray<float> Vertices;

	UPROPERTY(BlueprintReadOnly, Category = "StaticMesh")
		FString StaticMeshName;

	UPROPERTY(BlueprintReadOnly, Category = "StaticMesh")
		TArray<float> ModelMatrix;

public:
	FStaticMeshData() = default;
	FStaticMeshData(FString nameParameter, TArray<float> &Vertices, TArray<int32> &Indices, TArray<float> &modelMatrix);
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

	UPROPERTY(BlueprintReadOnly, Category = "Camera")
		FString CameraName;

public:
	FCameraInfo() = default;
};

USTRUCT(BlueprintType)
struct FLightInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Light")
		FVector Location;
	UPROPERTY(BlueprintReadOnly, Category = "Light")
		FVector Direction;
	UPROPERTY(BlueprintReadOnly, Category = "Light")
		float Intensity;

public:
	FLightInfo() = default;
};

USTRUCT(BlueprintType)
struct FSceneData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = "SceneData")
		TArray<FStaticMeshData> StaticMeshData;

	UPROPERTY(BlueprintReadOnly, Category = "Scenedata")
		TArray<FCameraInfo> cameraInfo;

	UPROPERTY(BlueprintReadOnly, Category = "Scenedata")
		FLightInfo lightInfo;
public:
	FSceneData() = default;
};

UCLASS()
class UExportResDatasBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

public:
	//将网格体数据以json的形式写到文件中
	UFUNCTION(BlueprintCallable, Category = "ResExport")
		static void WriteFile(const FString& FileString, FString OutputPath, const FString& Filename);

	//获得静态网格体的顶点数据
	UFUNCTION(BlueprintCallable, Category = "ResExport")
		static void GetStaticMeshVerticesData(const UStaticMesh* StaticMesh, TArray<float>& Output);

	//获得静态网格体的索引数据
	UFUNCTION(BlueprintCallable, Category = "ResExport")
		static void GetStaticMeshIndicesData(const UStaticMesh* StaticMesh, TArray<int32>& Output);

	UFUNCTION(BlueprintCallable, Category = "ResExport")
		static void ExportScene(const UObject* WorldContextObject);

	//在蓝图中不能声明static变量
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
};
