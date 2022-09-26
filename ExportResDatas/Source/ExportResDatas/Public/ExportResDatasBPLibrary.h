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
//	//���������
//	int32 Index = 0;
//
//	//�����λ�ã���ʼ��Ϊ(0,0,0)
//	FVector Position = FVector::ZeroVector;
//
//	//����ķ���,��ʼ��Ϊ(0,0,0)
//	FVector Normal = FVector::ZeroVector;
//
//	//������������꣬Ϊʲô������?
//	FVector2D TexCoord0 = FVector2D::ZeroVector;
//
//	FVector2D TexCoord1 = FVector2D::ZeroVector;
//
//	FVector2D TexCoord2 = FVector2D::ZeroVector;
//};

//�����糡���е�static mesh�Ķ��㡢������Ϣ
//�Լ��þ�̬�����������糡����Ӧ��model����
USTRUCT(BlueprintType)
struct FStaticMeshData
{
	GENERATED_USTRUCT_BODY()

	////��������
	//UPROPERTY(BlueprintReadOnly, Category = "StaticMesh")
	//	int32 VerticesNum;

	//��������
	UPROPERTY(BlueprintReadOnly, Category = "StaticMesh")
		TArray<int32> Indices;

	////�ı���Ӧ
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
	//��������������json����ʽд���ļ���
	UFUNCTION(BlueprintCallable, Category = "ResExport")
		static void WriteFile(const FString& FileString, FString OutputPath, const FString& Filename);

	//��þ�̬������Ķ�������
	UFUNCTION(BlueprintCallable, Category = "ResExport")
		static void GetStaticMeshVerticesData(const UStaticMesh* StaticMesh, TArray<float>& Output);

	//��þ�̬���������������
	UFUNCTION(BlueprintCallable, Category = "ResExport")
		static void GetStaticMeshIndicesData(const UStaticMesh* StaticMesh, TArray<int32>& Output);

	UFUNCTION(BlueprintCallable, Category = "ResExport")
		static void ExportScene(const UObject* WorldContextObject);

	//����ͼ�в�������static����
	//����ģ��
	template <typename TargetStruct>
	static void ExportStructByJsonConverter(const TargetStruct& Target, FString OutputPath, const FString& Filename )
	{
		FString OutputString;
		//���Զ���ṹ��ת��ΪJson�ַ���
		//����struct�Ľṹд��
		FJsonObjectConverter::UStructToJsonObjectString(Target, OutputString);
		//�����ָ���ļ���
		WriteFile(OutputString, OutputPath, Filename);
	}
};
