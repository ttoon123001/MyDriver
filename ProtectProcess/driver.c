#include <ntddk.h>   

#define DEVICE_NAME L"\\device\\ProctectProcess"
#define LINK_NAME L"\\dosdevices\\ProctectProcess" //等于\\??\\ntmodeldrv



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

 //用户要读到数据(理解为回调)
 NTSTATUS DispatchRead(PDEVICE_OBJECT pObject, PIRP pIrp)  //共用分发函数
{
	KdBreakPoint();
	PVOID pReadBuffer = NULL;
	ULONG uReadLength = 0;
	ULONG uHelloStr = wcslen(L"hello world");
	ULONG uMin = 0;
	PIO_STACK_LOCATION pIrpStack = NULL;
	//从头部拿到缓存地址(DO_BUFFERED_IO)
	pReadBuffer = pIrp->AssociatedIrp.SystemBuffer;
	//(DO_DIRECT_IO) pIrp->AssociatedIrp.MdlAddress; 需要通过系统函数先转为内核虚拟地址
	//(DO_NEITHER_IO) pIrp->AssociatedIrp.UserBuffer; 直接使用前通过ProbeforRead\ProbeforWrite校验
	//从IRP栈获取缓存长度
	pIrpStack = IoGetCurrentIrpStackLocation(pIrp); //指向当前IRP栈的Parameters联合体
	uReadLength = pIrpStack->Parameters.Read.Length; //Parameters的Read结构体
	//读、写操作
	uMin = uReadLength > uHelloStr ? uHelloStr : uReadLength;	//取最小值
	RtlCopyMemory(pReadBuffer, L"hello world", uMin);
	//完成IRP
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = uMin;  //实际传输字节(对应应用层ReadFile(lpNumberOfBytesRead)参数)
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;  //IO管理器接收完成状态
}

NTSTATUS DispatchWrite(PDEVICE_OBJECT pObject, PIRP pIrp)  //共用分发函数
{
	KdBreakPoint();
	ULONG uWriteLength = 0;
	PVOID pWriteBuff = NULL;
	PVOID pBuffer = NULL;
	PIO_STACK_LOCATION pStack = NULL;
	pWriteBuff = pIrp->AssociatedIrp.SystemBuffer;
	pStack = IoGetCurrentIrpStackLocation(pIrp);
	uWriteLength = pStack->Parameters.Write.Length;

	//分页(换页)内存只在IRQL:PASSIVE下使用
	//分配内存
	pBuffer = ExAllocatePoolWithTag(PagedPool, uWriteLength, 'TSET');  //Tag
	if(pBuffer == NULL)
	{
		pIrp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
		pIrp->IoStatus.Information = 0;
		IoCompleteRequest(pIrp,IO_NO_INCREMENT);
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	memset(pBuffer, 0, uWriteLength);
	RtlCopyMemory(pBuffer, pWriteBuff, uWriteLength); //写内存
	ExFreePool(pBuffer); //释放内存
	pBuffer = NULL;

	pIrp->IoStatus.Status = STATUS_SUCCESS; //状态设置成功
	pIrp->IoStatus.Information = uWriteLength; //实际要写入的数值
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

return STATUS_SUCCESS;  //IO管理器接收完成状态
} 
 
 NTSTATUS DispatchContorl(PDEVICE_OBJECT pObject, PIRP pIrp)  //共用分发函数
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

	IoCreateSymbolicLink(&uLinkName, &uDeviceName);  //为设备名创建一个符号链接

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
