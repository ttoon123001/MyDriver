#include <ntddk.h>   

#define DEVICE_NAME L"\\device\\ntmodeldrv"
#define LINK_NAME L"\\dosdevices\\ntmodeldrv" //等于\\??\\ntmodeldrv



VOID Unload(IN PDRIVER_OBJECT pDriverObject)
{
	DbgPrint("Unload/n");
}


 NTSTATUS DispatchCommon(PDEVICE_OBJECT pObject, PIRP pIrp)  //共用分发函数
{
	pIrp->IoStatus.Status = STATUS_SUCCESS;	//Io状态置为成功(用于应用层接收)
	pIrp->IoStatus.Information = 0;

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);  //结束这个IRP请求

	return STATUS_SUCCESS;  //IO管理器接收完成状态
}

NTSTATUS DriverEntry(
	IN PDRIVER_OBJECT pDriverObject,
	IN PUNICODE_STRING pRegistryPath
)
{	
	int i;
	UNICODE_STRING uDeviceName;
	UNICODE_STRING uLinkName;
	RtlInitUnicodeString(&uDeviceName, DEVICE_NAME);
	RtlInitUnicodeString(&uLinkName, LINK_NAME);

	IoCreateSymbolicLink(&uLinkName, &uDeviceName);  //为设备名创建一个符号链接

	DbgPrint("Hello/n");

	for (i = 0; i < IRP_MJ_MAXIMUM_FUNCTION + 1; i++)
	{
		pDriverObject->MajorFunction[i] = DispatchCommon;
	}


	pDriverObject->DriverUnload = Unload;
	return STATUS_SUCCESS;
}
