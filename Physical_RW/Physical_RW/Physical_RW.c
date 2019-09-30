#include"Physical_RW.h"

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegPath)
{
	int i = 0;
	NTSTATUS Status;

	RtlInitUnicodeString(&DeviceName, L"\\Device\\Physical_RW");
	RtlInitUnicodeString(&SymbolLink, L"\\DosDevices\\Physical_RW");

	//创建设备对象
	Status = IoCreateDevice(pDriverObject, 0, &DeviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &Device);
	if (!NT_SUCCESS(Status))
	{
		KdPrint(("CreateDevice Failed!!!\n"));
		return Status;
	}

	Status = IoCreateSymbolicLink(&SymbolLink, &DeviceName);
	if (!NT_SUCCESS(Status))
	{
		IoDeleteSymbolicLink(&SymbolLink);
		KdPrint(("synbol_link failed\n"));
	}

	//设置IRP派遣函数
	for (i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
	{
		pDriverObject->MajorFunction[i] = MajorDispatch;
	}

	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DeviceControlDispatch;

	//卸载例程
	pDriverObject->DriverUnload = DriverUnload;

	return STATUS_SUCCESS;


}

//DeviceControl派遣函数
NTSTATUS DeviceControlDispatch(PDEVICE_OBJECT deviceObject, PIRP Irp)
{
	NTSTATUS Status = STATUS_SUCCESS;

	PIO_STACK_LOCATION Irpsp = IoGetCurrentIrpStackLocation(Irp);
	DWORD32 Code = Irpsp->Parameters.DeviceIoControl.IoControlCode;
	DWORD32 In_Size = Irpsp->Parameters.DeviceIoControl.InputBufferLength;
	DWORD32 Out_Size = Irpsp->Parameters.DeviceIoControl.OutputBufferLength;
	PVOID In_Buffer = Irp->AssociatedIrp.SystemBuffer;
	PVOID Out_Buffer = Irp->AssociatedIrp.SystemBuffer;

	switch (Code)
	{
	case IOCTL_ReadPhysicalMemory:
	{
		PVOID MapAddress = NULL;
		DWORD64 Physicaladdress;

		//分配空间保存读取到的物理地址数据
		MapAddress = ExAllocatePoolWithTag(NonPagedPool, 0x100, 'buf');
		RtlZeroMemory(MapAddress, 0x100);

		//获取要读取的物理地址
		Physicaladdress = *(DWORD64*)In_Buffer;
		//获取指定物理地址数据
		Status = ReadPhysicalAddress(MapAddress, Physicaladdress);

		memmove_s(Out_Buffer, 0x100 ,MapAddress, 0x100);

		ExFreePoolWithTag(MapAddress, 'buf');

		Irp->IoStatus.Status = Status;
		Irp->IoStatus.Information = 0x100;

		break;
	}
	case IOCTL_WritePhysicalMemory:
	{
		WritePhysicalMemoryStruct* Struct_PhysicalDword = In_Buffer;
		WritePhysicalMemory(Struct_PhysicalDword->PhysicalAddress, Struct_PhysicalDword->WriteData);

		Irp->IoStatus.Status = Status;
		Irp->IoStatus.Information = 0;
		break;
	}

	default:
	{
		break;
	}
	}

	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return Status;
}

//卸载函数
VOID DriverUnload(PDRIVER_OBJECT pDriverObject)
{
	if (pDriverObject->DeviceObject != NULL)
	{
		IoDeleteSymbolicLink(&SymbolLink);
		IoDeleteDevice(pDriverObject->DeviceObject);
	}
}

//默认处理其他IRP
NTSTATUS MajorDispatch(PDEVICE_OBJECT device, PIRP Irp)
{
	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}