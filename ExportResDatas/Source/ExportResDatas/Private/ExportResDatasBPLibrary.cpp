// Copyright Epic Games, Inc. All Rights Reserved.

#include "ExportResDatasBPLibrary.h"
#include "ExportResDatas.h"

DEFINE_LOG_CATEGORY_STATIC(LogResExporter, Log, All);
//namespace RE
//{
//	//<<运算符重载，能对FStaticVertex_RE结构体成员变量进行操作
//	FArchive& operator<<(FArchive& Ar, FStaticVertex_RE& Value)
//	{
//		Ar << Value.Position;
//		Ar << Value.TangentZ;
//		Ar << Value.UV0;
//		Ar << Value.Color;
//		return Ar;
//	}
//
//	FArchive& operator<<(FArchive& Ar, FStaticMesh_Lod_RE& Value)
//	{
//		Ar << Value.VertStride;
//		Ar << Value.Vertice;
//		Ar << Value.Indices;
//		return Ar;
//	}
//}

UExportResDatasBPLibrary::UExportResDatasBPLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

FStaticMeshData::FStaticMeshData(TArray<float> Vertices, TArray<int32> Indices) : Indices(Indices), Vertices(Vertices)
{
	VsFormat =
		TEXT("POSITION,")
		TEXT("NORMAL,")
		TEXT("TEXCOORD0,")
		TEXT("TEXCOORD1");

	VerticesNum = Vertices.Num();
}

void UExportResDatasBPLibrary::WriteFile(const FString& FileString, FString OutputPath, const FString& Filename)
{
	if (OutputPath.IsEmpty())
	{
		OutputPath = FPaths::ProfilingDir();
	}

	FString Fullname = OutputPath + Filename + TEXT(".json");

	if (!FFileHelper::SaveStringToFile(FileString, *Fullname, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
	{
		UE_LOG(LogResExporter, Display, TEXT("Failed to write to temp file: %s"), *Fullname);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Failed to write to temp file: %s"), *Fullname));
	}
	else
	{
		UE_LOG(LogResExporter, Error, TEXT("Succeed to save file: %s"), *Fullname);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Succeed to save file: %s"), *Fullname));
	}
}

void UExportResDatasBPLibrary::GetStaticMeshVerticesData(const UStaticMesh* StaticMesh, TArray<float>& Output)
{
	TArray<float> Vertices;

	if (!StaticMesh || !StaticMesh->GetRenderData())
	{
		return;
	}
	if (StaticMesh->GetRenderData()->LODResources.Num() > 0)
	{
		const FStaticMeshVertexBuffers* VertexBuffers = &StaticMesh->GetRenderData()->LODResources[0].VertexBuffers;

		if (VertexBuffers)
		{
			//将顶点的位置信息，法线，uv坐标存放到Vertices中
			for (uint32 i = 0; i < VertexBuffers->PositionVertexBuffer.GetNumVertices(); i++)
			{
				Vertices.Add(VertexBuffers->PositionVertexBuffer.VertexPosition(i).X);
				Vertices.Add(VertexBuffers->PositionVertexBuffer.VertexPosition(i).Y);
				Vertices.Add(VertexBuffers->PositionVertexBuffer.VertexPosition(i).Z);

				Vertices.Add(VertexBuffers->StaticMeshVertexBuffer.VertexTangentZ(i).X);
				Vertices.Add(VertexBuffers->StaticMeshVertexBuffer.VertexTangentZ(i).Y);
				Vertices.Add(VertexBuffers->StaticMeshVertexBuffer.VertexTangentZ(i).Z);

				Vertices.Add(VertexBuffers->StaticMeshVertexBuffer.GetVertexUV(i, 0).X);
				Vertices.Add(VertexBuffers->StaticMeshVertexBuffer.GetVertexUV(i, 0).Y);
			}
		}
		else
		{
			return;
		}
	}

	Output = Vertices;
}

void UExportResDatasBPLibrary::GetStaticMeshIndicesData(const UStaticMesh* StaticMesh, TArray<int32>& Output)
{
	TArray<int32> Indices = TArray<int32>();

	if (!StaticMesh || !StaticMesh->GetRenderData())
	{
		return;
	}
	if (StaticMesh->GetRenderData()->LODResources.Num() > 0)
	{
		const FRawStaticIndexBuffer* IndexBuffer = &StaticMesh->GetRenderData()->LODResources[0].IndexBuffer;
		if (IndexBuffer)
		{
			const int32 IndexCount = IndexBuffer->GetNumIndices();
			for (int32 i = 0; i < IndexCount; i++)
			{
				const int32 Index = (int32)IndexBuffer->GetIndex(i);
				Indices.Add(Index);
			}
		}
	}

	Output = Indices;
}

//指向静态网格体对象的指针由蓝图给出
void UExportResDatasBPLibrary::ExportStaticMesh(const UStaticMesh* StaticMesh, FString Path, const FString& Filename)
{
	//获得顶点数据（包含顶点坐标、顶点法线坐标以及顶点的纹理坐标）
	TArray<float> Vertices;
	GetStaticMeshVerticesData(StaticMesh, Vertices);

	//获得静态网格体的索引数据
	TArray<int32> Indices;
	GetStaticMeshIndicesData(StaticMesh, Indices);

	FStaticMeshData Data(Vertices, Indices);

	ExportStructByJsonConverter(Data, Path, Filename);
}


float UExportResDatasBPLibrary::ExportResDatasSampleFunction(float Param)
{
	return -1;
}