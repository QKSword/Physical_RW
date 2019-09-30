#pragma once
#include<ntifs.h>
#include"RW.h"

//IOCTL控制码
#define IOCTL_ReadPhysicalMemory (DWORD32)CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)		//读
#define IOCTL_WritePhysicalMemory (DWORD32)CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)		//写

//符号链接等初始化
UNICODE_STRING DeviceName;
UNICODE_STRING SymbolLink;
PDEVICE_OBJECT Device;

//函数定义
VOID DriverUnload(PDRIVER_OBJECT pDriverObject);
NTSTATUS MajorDispatch(PDEVICE_OBJECT device, PIRP Irp);
NTSTATUS DeviceControlDispatch(PDEVICE_OBJECT deviceObject, PIRP Irp);
