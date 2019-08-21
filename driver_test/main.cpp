#include <ntddk.h>

#define UNUSED(x) (x, 0);

void myUnload(IN PDRIVER_OBJECT DriverObject);
NTSTATUS myCreateClose(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS myHandler(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);


// 驱动入口函数
// extern "C" 的作用：无法解析的外部符号 DriverEntry
extern "C" NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath) {
	
	UNUSED(RegistryPath);
	UNICODE_STRING DeviceName, Win32Device;
	PDEVICE_OBJECT DeviceObject = NULL;
	NTSTATUS status;
	unsigned i;

	KdPrint(("[DriverEntry]\n"));

	// 设备名
	RtlInitUnicodeString(&DeviceName, L"\\Device\\myDevice");
	// 符号名
	RtlInitUnicodeString(&Win32Device, L"\\VirDevices\\myDevice");

	// 设置默认的IRP处理函数
	for (i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++) {
		DriverObject->MajorFunction[i] = myHandler;
	}
	// IRP打开处理函数
	DriverObject->MajorFunction[IRP_MJ_CREATE] = myCreateClose;
	// IRP关闭处理函数
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = myCreateClose;

	// 驱动卸载函数
	DriverObject->DriverUnload = myUnload;
	// 创建设备
	status = IoCreateDevice(DriverObject, 0, &DeviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &DeviceObject);

	if (!NT_SUCCESS(status)) return status;
	if (!DeviceObject) return STATUS_UNEXPECTED_IO_ERROR;

	// 设置设备的读写方式
	DeviceObject->Flags |= DO_DIRECT_IO;

	// 创建符号链接
	status = IoCreateSymbolicLink(&Win32Device, &DeviceName);

	// 设备初始化完毕，可以工作了
	DeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;

	return STATUS_SUCCESS;
}

void myUnload(IN PDRIVER_OBJECT DriverObject) {

	UNICODE_STRING Win32Device;
	KdPrint(("[myUnload]\n"));

	RtlInitUnicodeString(&Win32Device, L"\\VirDevices\\myDevice");

	IoDeleteSymbolicLink(&Win32Device);
	IoDeleteDevice(DriverObject->DeviceObject);
}

NTSTATUS myCreateClose(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp) {
	UNUSED(DeviceObject);
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;

}

NTSTATUS myHandler(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp) {
	UNUSED(DeviceObject);
	Irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
	Irp->IoStatus.Information = 0;
	
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return Irp->IoStatus.Status;

}