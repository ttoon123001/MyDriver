#include <ntddk.h>   


#define DEVICE_NAME L"\\device\\ProtectProcess"
#define LINK_NAME L"\\dosdevices\\ProtectProcess_001" //����\\??\\ntmodeldrv


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
	// ɾ����������
	UNICODE_STRING DosSymName;
	RtlInitUnicodeString(&DosSymName, LINK_NAME);
	IoDeleteSymbolicLink(&DosSymName);
	// ж���豸����
	IoDeleteDevice(pDriverObject->DeviceObject);
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
	ULONG uHelloStr = wcslen(L"hello world") * sizeof(WCHAR);
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
	 KdBreakPoint();

	 ULONG uIoctrlCode = 0;
	 PIO_STACK_LOCATION pStack = NULL;
	 PVOID InputBuff = NULL, pOutputBuff = NULL;
	 ULONG InputtLength = 0, OutputLength = 0;
	 InputBuff = pOutputBuff = pIrp->AssociatedIrp.SystemBuffer; //����д����systemBuffer
	 pStack = IoGetCurrentIrpStackLocation(pIrp);	//Irpջ
	 InputtLength = pStack->Parameters.DeviceIoControl.InputBufferLength; //�ں˶�
	 OutputLength = pStack->Parameters.DeviceIoControl.OutputBufferLength;  //�ں�д

	 uIoctrlCode = pStack->Parameters.DeviceIoControl.IoControlCode; //������

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
		 //�ں˴���
	 }
	 else
	 {
		 //�û���
		   //�û���������OpenProcess,NtOpenProcess�����ߵĽ�������ȡ
		 //const char*������ = PsGetProcessImageFileName(PsGetCurrentProcess());//16����Ч�ַ���
		 //���Ŀ�����Pid��Ȼ���ý�����
		 //HANDLE pid = PsGetProcessId((PEPROCESS)OperationInformation->Object);
		 //const char*Ŀ������� = GetProcessName(pid);
		 //KdPrint(("yjx:SYS ������=%s\n", ������));

		 //�������޸�Ȩ��
		 ACCESS_MASK oldȨ�� = OperationInformation->Parameters->CreateHandleInformation.OriginalDesiredAccess;
		 ACCESS_MASK old1 = oldȨ��;
		 ACCESS_MASK new1 = OperationInformation->Parameters->CreateHandleInformation.DesiredAccess;
		 //�ų� PROCESS_VM_READ Ȩ��
		 oldȨ�� &= ~PROCESS_VM_READ;
		 //�ų��� PROCESS_VM_WRITE
		 oldȨ�� &= ~PROCESS_VM_WRITE;
		 //���������޸Ĺ���Ȩ�� OpenProcess
		 OperationInformation->Parameters->CreateHandleInformation.DesiredAccess = oldȨ��;
		 DbgPrint("yjx:oldȨ��=%x ��Ȩ��=%X", oldȨ��, new1);

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


	//���ü����Ķ�������
	oor.ObjectType = PsProcessType;
	//���ü����Ĳ�������
	oor.Operations = OB_OPERATION_HANDLE_CREATE | OB_OPERATION_HANDLE_DUPLICATE;
	//���ò�������ǰִ�еĻص�
	oor.PreOperation = my_pre_callback;
	ocr.Version = OB_FLT_REGISTRATION_VERSION;
	ocr.RegistrationContext = NULL;
	ocr.OperationRegistrationCount = 1;
	ocr.OperationRegistration = &oor;
	RtlInitUnicodeString(&ocr.Altitude, L"321000"); // ���ü���˳��
	ObRegisterCallbacks(&ocr, &pRegistrationHandle);




	return STATUS_SUCCESS;
}
	