﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "EGIK_GetPublicIp.h"

UEGIK_GetPublicIp* UEGIK_GetPublicIp::GetIpInformation()
{
	UEGIK_GetPublicIp* Node = NewObject<UEGIK_GetPublicIp>();
	return Node;
}

void UEGIK_GetPublicIp::OnResponseReceived(TSharedPtr<IHttpRequest> HttpRequest, TSharedPtr<IHttpResponse> HttpResponse,
	bool bArg)
{
	if (HttpResponse.IsValid())
	{
		if (EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode()))
		{
			FString IpAddress;
			TSharedPtr<FJsonObject> JsonObject;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(HttpResponse->GetContentAsString());
			if (FJsonSerializer::Deserialize(Reader, JsonObject))
			{
				IpAddress = JsonObject->GetStringField(TEXT("public_ip"));
				OnSuccess.Broadcast(IpAddress, FEGIK_ErrorStruct());
			}
			else
			{
				OnFailure.Broadcast("", FEGIK_ErrorStruct(0, "Failed to deserialize response"));
			}
		}
		else
		{
			OnFailure.Broadcast("", FEGIK_ErrorStruct(HttpResponse->GetResponseCode(), "Failed to get ip information"));
		}
	}
	else
	{
		OnFailure.Broadcast("", FEGIK_ErrorStruct(0, "Failed to deserialize response"));
	}
	SetReadyToDestroy();
	MarkAsGarbage();
}

void UEGIK_GetPublicIp::Activate()
{
	Super::Activate();
	FHttpModule* Http = &FHttpModule::Get();
	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->SetVerb("GET");
	Request->SetURL("https://api.edgegap.com/v1/ip");
	Request->SetHeader("Content-Type", "application/json");
	Request->SetHeader("Authorization", "token 3d6e71e0-5717-4e8b-959d-b374d004be73");
	Request->OnProcessRequestComplete().BindUObject(this, &UEGIK_GetPublicIp::OnResponseReceived);
	if (!Request->ProcessRequest())
	{
		OnFailure.Broadcast("", FEGIK_ErrorStruct(0, "Failed to process request"));
		SetReadyToDestroy();
		MarkAsGarbage();
	}
}