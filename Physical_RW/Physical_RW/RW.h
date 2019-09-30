#pragma once
#include"Physical_RW.h"

//物理地址读写的全局句柄
static HANDLE hPhysicalhandle = NULL;

//函数声明
NTSTATUS ReadPhysicalAddress(PVOID MapAddress, DWORD64 Physicaladdress);
NTSTATUS WritePhysicalMemory(DWORD64 PhysicalAddress, DWORD32 WriteData);
NTSTATUS GetPhysicalHandle();


//结构体声明
typedef struct _WritePhysicalMemoryStruct
{
	DWORD64 PhysicalAddress;	//物理地址
	DWORD32 WriteData;			//写入的数据
}WritePhysicalMemoryStruct, *PWritePhysicalMemoryStruct;
