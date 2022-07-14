#include <ntddk.h>   

#define DEVICE_NAME L"\\device\\ProctectProcess"
#define LINK_NAME L"\\dosdevices\\ProctectProcess" //����\\??\\ntmodeldrv



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

 //�û�Ҫ��������(���Ϊ�ص�)
 NTSTATUS DispatchRead(PDEVICE_OBJECT pObject, PIRP pIrp)  //���÷ַ�����
{
	KdBreakPoint();
	PVOID pReadBuffer = NULL;
	ULONG uReadLength = 0;
	ULONG uHelloStr = wcslen(L"hello world");
	ULONG uMin = 0;
	PIO_STACK_LOCATION pIrpStack = NULL;
	//��ͷ���õ������ַ(DO_BUFFERED_IO)
	pReadBuffer = pIrp->AssociatedIrp.SystemBuffer;
	//(DO_DIRECT_IO) pIrp->AssociatedIrp.MdlAddress; ��Ҫͨ��ϵͳ������תΪ�ں������ַ
	//(DO_NEITHER_IO) pIrp->AssociatedIrp.UserBuffer; ֱ��ʹ��ǰͨ��ProbeforRead\ProbeforWriteУ��
	//��IRPջ��ȡ���泤��
	pIrpStack = IoGetCurrentIrpStackLocation(pIrp); //ָ��ǰIRPջ��Parameters������
	uReadLength = pIrpStack->Parameters.Read.Length; //Parameters��Read�ṹ��
	//����д����
	uMin = uReadLength > uHelloStr ? uHelloStr : uReadLength;	//ȡ��Сֵ
	RtlCopyMemory(pReadBuffer, L"hello world", uMin);
	//���IRP
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = uMin;  //ʵ�ʴ����ֽ�(��ӦӦ�ò�ReadFile(lpNumberOfBytesRead)����)
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;  //IO�������������״̬
}

NTSTATUS DispatchWrite(PDEVICE_OBJECT pObject, PIRP pIrp)  //���÷ַ�����
{
	KdBreakPoint();
	ULONG uWriteLength = 0;
	PVOID pWriteBuff = NULL;
	PVOID pBuffer = NULL;
	PIO_STACK_LOCATION pStack = NULL;
	pWriteBuff = pIrp->AssociatedIrp.SystemBuffer;
	pStack = IoGetCurrentIrpStackLocation(pIrp);
	uWriteLength = pStack->Parameters.Write.Length;

	//��ҳ(��ҳ)�ڴ�ֻ��IRQL:PASSIVE��ʹ��
	//�����ڴ�
	pBuffer = ExAllocatePoolWithTag(PagedPool, uWriteLength, 'TSET');  //Tag
	if(pBuffer == NULL)
	{
		pIrp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
		pIrp->IoStatus.Information = 0;
		IoCompleteRequest(pIrp,IO_NO_INCREMENT);
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	memset(pBuffer, 0, uWriteLength);
	RtlCopyMemory(pBuffer, pWriteBuff, uWriteLength); //д�ڴ�
	ExFreePool(pBuffer); //�ͷ��ڴ�
	pBuffer = NULL;

	pIrp->IoStatus.Status = STATUS_SUCCESS; //״̬���óɹ�
	pIrp->IoStatus.Information = uWriteLength; //ʵ��Ҫд�����ֵ
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

return STATUS_SUCCESS;  //IO�������������״̬
} 
 
 NTSTATUS DispatchContorl(PDEVICE_OBJECT pObject, PIRP pIrp)  //���÷ַ�����
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
	PDEVICE_OBJECT pDeviceObject;
	UNICODE_STRING uDeviceName;
	UNICODE_STRING uLinkName;
	RtlInitUnicodeString(&uDeviceName, DEVICE_NAME);
	RtlInitUnicodeString(&uLinkName, LINK_NAME);


	IoCreateDevice(pDriverObject,
		0,
		&uDeviceName,
		FILE_DEVICE_UNKNOWN,
		0,
		FALSE,
		&pDeviceObject);

	//pDeviceObject->Flags |= DO_BUFFERED_IO;
	//pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;
	//pDevExt->pDevice = pDevObj;
	//pDevExt->ustrDeviceName = devName;

	IoCreateSymbolicLink(&uLinkName, &uDeviceName);  //Ϊ�豸������һ����������

	DbgPrint("Hello/n");

	for (i = 0; i < IRP_MJ_MAXIMUM_FUNCTION + 1; i++)
	{
		pDriverObject->MajorFunction[i] = DispatchCommon;
	}
	pDriverObject->MajorFunction[IRP_MJ_WRITE] = DispatchWrite;
	pDriverObject->MajorFunction[IRP_MJ_READ] = DispatchRead;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchContorl;


	pDriverObject->DriverUnload = Unload;
	return STATUS_SUCCESS;
}
