#include"RW.h"

/*
function
	读取物理地址，大小为FF
argv
	MapAddress：物理地址映射出来的地址
	Physicaladdress：指定读取的物理地址
return
	返回状态
*/
NTSTATUS ReadPhysicalAddress(PVOID MapAddress, DWORD64 Physicaladdress)
{
	NTSTATUS status;
	PVOID BaseAddress = NULL; // 映射的虚地址
	DWORD32 offset;
	LARGE_INTEGER SectionOffset; //存放物理地址
	SIZE_T size = 0x2000; //映射大小

	//打开内核对象
	status = GetPhysicalHandle();
	if (status < 0)
	{
		status = FALSE;
		goto Leave;
	}

	//读取出来的地址是4K对齐，所以偏移自己处理一下即可
	offset = Physicaladdress & 0xFFF;	//取低12位作为偏移使用
	SectionOffset.QuadPart = (ULONGLONG)(Physicaladdress);

	// 映射物理内存地址到当前进程的虚地址空间
	status = ZwMapViewOfSection(
		hPhysicalhandle,
		NtCurrentProcess(),
		(PVOID *)&BaseAddress,
		0,
		size,
		&SectionOffset,
		&size,
		ViewShare,
		MEM_TOP_DOWN,
		PAGE_READWRITE);

	if (status < 0)
	{
		status = FALSE;
		goto Leave;
	}

	//获取映射出来的数据
	memmove_s(MapAddress, 0x100, (PVOID)((DWORD64)BaseAddress + offset), 0x100);

	// 完成操作，取消地址映射
	status = ZwUnmapViewOfSection(NtCurrentProcess(), BaseAddress);

	if (status < 0)
	{
		status = FALSE;
		goto Leave;
	}

Leave:
	if (hPhysicalhandle != NULL)
	{
		ZwClose(hPhysicalhandle);
	}

	return status;
}

/*
function
	写物理地址，大小为DWORD
argv
	PhysicalAddress：物理地址
	WriteData：写入数据
return
	返回状态
*/
NTSTATUS WritePhysicalMemory(DWORD64 PhysicalAddress, DWORD32 WriteData)
{
	NTSTATUS status;
	PVOID BaseAddress = NULL; // 映射的虚地址
	DWORD32 offset;
	LARGE_INTEGER SectionOffset; //存放物理地址
	SIZE_T size = 0x2000; //映射大小

	//打开内核对象
	status = GetPhysicalHandle();
	if (status < 0)
	{
		status = FALSE;
		goto Leave;
	}

	offset = PhysicalAddress & 0xFFF;		//取低12位作为偏移使用

	SectionOffset.QuadPart = (ULONGLONG)(PhysicalAddress);

	// 映射物理内存地址到当前进程的虚地址空间
	status = ZwMapViewOfSection(
		hPhysicalhandle,
		NtCurrentProcess(),
		(PVOID *)&BaseAddress,
		0,
		size,
		&SectionOffset,
		&size,
		ViewShare,
		MEM_TOP_DOWN,
		PAGE_READWRITE);

	if (status < 0)
	{
		status = FALSE;
		goto Leave;
	}

	memmove_s((PVOID)((DWORD64)BaseAddress + offset), sizeof(DWORD32), &WriteData, sizeof(DWORD32));

	status = ZwUnmapViewOfSection(NtCurrentProcess(), BaseAddress);

	if (status < 0)
	{
		status = FALSE;
	}

Leave:
	if (hPhysicalhandle != NULL)
	{
		ZwClose(hPhysicalhandle);
	}

	return status;
}

/*
function
	打开内核对象
argv
	NULL
return
	返回调用状态
*/
NTSTATUS GetPhysicalHandle()
{
	NTSTATUS status;
	UNICODE_STRING PhysicalMemoryString;
	OBJECT_ATTRIBUTES attributes;

	WCHAR PhysicalMemoryName[] = L"\\Device\\PhysicalMemory";
	RtlInitUnicodeString(&PhysicalMemoryString, PhysicalMemoryName);
	InitializeObjectAttributes(&attributes, &PhysicalMemoryString, 0, NULL, NULL);
	status = ZwOpenSection(&hPhysicalhandle, SECTION_MAP_READ | SECTION_MAP_WRITE, &attributes);

	return status;
}