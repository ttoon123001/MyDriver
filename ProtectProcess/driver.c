#include <ntddk.h>   


#define DEVICE_NAME L"\\device\\ProtectProcess"
#define LINK_NAME L"\\dosdevices\\ProtectProcess_001" //等于\\??\\ntmodeldrv


#define IOCTRL_BASE 0x800

#define MYIOCTRL_CODE(i) \
	CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTRL_BASE+i, METHOD_BUFFERED,FILE_ANY_ACCESS)

#define CTL_HELLO MYIOCTRL_CODE(0)
#define CTL_PRINT MYIOCTRL_CODE(1)
#define CTL_BYE MYIOCTRL_CODE(2)

#define PROCESS_VM_READ                    (0x0010)  
#define PROCESS_VM_WRITE                   (0x0020) 


PVOID pRegistrationHandle;



VOID Unload(IN PDRIVER_OBJECT pDriverObject)
{
	DbgPrint("Unload/n");
	// 删除符号链接
	UNICODE_STRING DosSymName;
	RtlInitUnicodeString(&DosSymName, LINK_NAME);
	IoDeleteSymbolicLink(&DosSymName);
	// 卸载设备对象
	IoDeleteDevice(pDriverObject->DeviceObject);
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
	ULONG uHelloStr = wcslen(L"hello world") * sizeof(WCHAR);
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
	 KdBreakPoint();

	 ULONG uIoctrlCode = 0;
	 PIO_STACK_LOCATION pStack = NULL;
	 PVOID InputBuff = NULL, pOutputBuff = NULL;
	 ULONG InputtLength = 0, OutputLength = 0;
	 InputBuff = pOutputBuff = pIrp->AssociatedIrp.SystemBuffer; //读和写都是systemBuffer
	 pStack = IoGetCurrentIrpStackLocation(pIrp);	//Irp栈
	 InputtLength = pStack->Parameters.DeviceIoControl.InputBufferLength; //内核读
	 OutputLength = pStack->Parameters.DeviceIoControl.OutputBufferLength;  //内核写

	 uIoctrlCode = pStack->Parameters.DeviceIoControl.IoControlCode; //控制码

	 switch (uIoctrlCode)
	 {
	 case CTL_HELLO:
		 //....
		 break;	 
	 case CTL_PRINT:
		 //....
		 break;	 
	 case CTL_BYE:
		 //....
		 break;
	 default:
		 break;
	 }
	 pIrp->IoStatus.Status = STATUS_SUCCESS;
	 pIrp->IoStatus.Information = 0;
	 IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	 return STATUS_SUCCESS;

}




 OB_PREOP_CALLBACK_STATUS
	 my_pre_callback(
		 PVOID RegistrationContext,
		 POB_PRE_OPERATION_INFORMATION OperationInformation
	 )
 {
	 DbgPrint("yjx:sys pEPROCESS=%p ", OperationInformation->Object);
	 if (OperationInformation->KernelHandle)
	 {
		 //内核创建
	 }
	 else
	 {
		 //用户层
		   //用户层下所有OpenProcess,NtOpenProcess调用者的进程名获取
		 //const char*进程名 = PsGetProcessImageFileName(PsGetCurrentProcess());//16个有效字符串
		 //获得目标进程Pid，然后获得进程名
		 //HANDLE pid = PsGetProcessId((PEPROCESS)OperationInformation->Object);
		 //const char*目标进程名 = GetProcessName(pid);
		 //KdPrint(("yjx:SYS 进程名=%s\n", 进程名));

		 //保存与修改权限
		 ACCESS_MASK old权限 = OperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess;
		 ACCESS_MASK old1 = old权限;
		 ACCESS_MASK new1 = OperationInformation->Parameters->CreateHandleInformation.DesiredAccess;
		 //排除 PROCESS_VM_READ 权限
		 old权限 &= ~PROCESS_VM_READ;
		 //排除掉 PROCESS_VM_WRITE
		 old权限 &= ~PROCESS_VM_WRITE;
		 //返回我们修改过的权限 OpenProcess
		 OperationInformation->Parameters->CreateHandleInformation.DesiredAccess = old权限;
		 DbgPrint("yjx:old权限=%x 新权限=%X", old权限, new1);

	 }

	 return OB_PREOP_SUCCESS;
 };


NTSTATUS DriverEntry(
	IN PDRIVER_OBJECT pDriverObject,
	IN PUNICODE_STRING pRegistryPath
)
{	
	int i;
	PDEVICE_OBJECT pDeviceObject;
	UNICODE_STRING uDeviceName;
	UNICODE_STRING uLinkName;



	OB_OPERATION_REGISTRATION oor;
	OB_CALLBACK_REGISTRATION ocr;



	pRegistrationHandle = 0;


	RtlZeroMemory(&oor, sizeof(OB_OPERATION_REGISTRATION));
	RtlZeroMemory(&ocr, sizeof(OB_CALLBACK_REGISTRATION));

	RtlInitUnicodeString(&uDeviceName, DEVICE_NAME);
	RtlInitUnicodeString(&uLinkName, LINK_NAME);


	IoCreateDevice(pDriverObject,
		0,
		&uDeviceName,
		FILE_DEVICE_UNKNOWN,
		0,
		FALSE,
		&pDeviceObject);

	pDeviceObject->Flags |= DO_BUFFERED_IO;
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


	//设置监听的对象类型
	oor.ObjectType = PsProcessType;
	//设置监听的操作类型
	oor.Operations = OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE;
	//设置操作发生前执行的回调
	oor.PreOperation = my_pre_callback;
	ocr.Version = OB_FLT_REGISTRATION_VERSION;
	ocr.RegistrationContext = NULL;
	ocr.OperationRegistrationCount = 1;
	ocr.OperationRegistration = &oor;
	RtlInitUnicodeString(&ocr.Altitude, L"321000"); // 设置加载顺序
	ObRegisterCallbacks(&ocr, &pRegistrationHandle);




	return STATUS_SUCCESS;
}
	