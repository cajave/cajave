#include <ntddk.h>

#define UNUSED(x) (x, 0);

void myUnload(IN PDRIVER_OBJECT DriverObject);
NTSTATUS myCreateClose(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS myHandler(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);


// ������ں���
// extern "C" �����ã��޷��������ⲿ���� DriverEntry
extern "C" NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath) {
	
	UNUSED(RegistryPath);
	UNICODE_STRING DeviceName, Win32Device;
	PDEVICE_OBJECT DeviceObject = NULL;
	NTSTATUS status;
	unsigned i;

	KdPrint(("[DriverEntry]\n"));

	// �豸��
	RtlInitUnicodeString(&DeviceName, L"\\Device\\myDevice");
	// ������
	RtlInitUnicodeString(&Win32Device, L"\\VirDevices\\myDevice");

	// ����Ĭ�ϵ�IRP������
	for (i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++) {
		DriverObject->MajorFunction[i] = myHandler;
	}
	// IRP�򿪴�����
	DriverObject->MajorFunction[IRP_MJ_CREATE] = myCreateClose;
	// IRP�رմ�����
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = myCreateClose;

	// ����ж�غ���
	DriverObject->DriverUnload = myUnload;
	// �����豸
	status = IoCreateDevice(DriverObject, 0, &DeviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &DeviceObject);

	if (!NT_SUCCESS(status)) return status;
	if (!DeviceObject) return STATUS_UNEXPECTED_IO_ERROR;

	// �����豸�Ķ�д��ʽ
	DeviceObject->Flags |= DO_DIRECT_IO;

	// ������������
	status = IoCreateSymbolicLink(&Win32Device, &DeviceName);

	// �豸��ʼ����ϣ����Թ�����
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