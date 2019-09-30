#include<Windows.h>
#include<stdio.h>

#define IOCTL_ReadPhysicalMemory (DWORD32)CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)		//读字节

int main()
{
	HANDLE device;

	DWORD BytesReturned;

	//打开句柄
	device = CreateFile(L"\\\\.\\Physical_RW", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_SYSTEM, NULL);
	if (device == INVALID_HANDLE_VALUE)
	{
		goto Leave;
	}
	while (1)
	{
		LPVOID OutputBuffer = VirtualAlloc(NULL, 0x100, MEM_COMMIT | MEM_TOP_DOWN, PAGE_READWRITE);
		DWORD64 inputaddress;
		printf_s("输入物理地址(16进制):");
		scanf_s("%llx", &inputaddress);

		if (DeviceIoControl(device, IOCTL_ReadPhysicalMemory, (LPVOID)&inputaddress, sizeof(DWORD64), OutputBuffer, 0x100, &BytesReturned, NULL))
		{
			for (int i = 0; i < 0x100; i++)
			{
				BYTE buffer = *((BYTE*)OutputBuffer + i);
				printf_s("%02x ", buffer);
				if (i != 0 && (i + 1) % 16 == 0)
				{
					printf_s("\n");
				}
			}
			printf_s("\n");
		}
		VirtualFree(OutputBuffer, 0, MEM_RELEASE);
	}

Leave:
	if (device != INVALID_HANDLE_VALUE)
	{
		CloseHandle(device);
	}

	system("pause");
	return 0;
}