#include <ntddk.h>   


#define LINK_NAME L"\\dosdevices\\ntmodeldrv" //����\\??\\ntmodeldrv



VOID Unload(IN PDRIVER_OBJECT pDriverObject)
{
	DbgPrint("Unload/n");
}


 NTSTATUS DispatchCommon(PDEVICE_OBJECT pObject, PIRP pIrp)  //���÷ַ�����
{
	pIrp->IoStatus.Status = STATUS_SUCCESS;	//Io״̬��Ϊ�ɹ�(����Ӧ�ò����)
	pIrp->IoStatus.Information = 0;

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);  //�������IRP����

	return STATUS_SUCCESS;  //IO�������������״̬
}

NTSTATUS DriverEntry(
	IN PDRIVER_OBJECT pDriverObject,
	IN PUNICODE_STRING pRegistryPath
)
{	
	int i;
	UNICODE_STRING uLinkName;

	RtlInitUnicodeString(&uLinkName, LINK_NAME);

	DbgPrint("Hello/n");

	for (i = 0; i < IRP_MJ_MAXIMUM_FUNCTION + 1; i++)
	{
		pDriverObject->MajorFunction[i] = DispatchCommon;
	}


	pDriverObject->DriverUnload = Unload;
	return STATUS_SUCCESS;
}
