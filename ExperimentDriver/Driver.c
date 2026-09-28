/*++

Module Name:

    Driver.c

Abstract:

    A minimal KMDF (Kernel-Mode Driver Framework) driver.
    It loads and unloads cleanly and writes trace messages that you can
    watch in DebugView (or WinDbg) while testing inside a VM.

    This is a "framework-only" driver: it registers with WDF, creates a
    device object on EvtDriverDeviceAdd, and does nothing else. It is the
    correct, crash-free starting point to grow a real driver from.

Environment:

    Kernel mode only.

--*/

#include <ntddk.h>
#include <wdf.h>

// Forward declarations
DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD ExperimentEvtDeviceAdd;
EVT_WDF_OBJECT_CONTEXT_CLEANUP ExperimentEvtDriverContextCleanup;

//
// Tell the linker this routine can be discarded after init (saves memory).
//
#ifdef ALLOC_PRAGMA
#pragma alloc_text (INIT, DriverEntry)
#pragma alloc_text (PAGE, ExperimentEvtDeviceAdd)
#pragma alloc_text (PAGE, ExperimentEvtDriverContextCleanup)
#endif

NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT  DriverObject,
    _In_ PUNICODE_STRING RegistryPath
    )
/*++

Routine Description:

    DriverEntry is the first routine the system calls after loading the
    driver. It initializes the WDF framework and registers the callbacks
    the framework will invoke over the driver's lifetime.

Return Value:

    STATUS_SUCCESS if the driver initialized correctly; otherwise an
    NTSTATUS error code (the driver will fail to load).

--*/
{
    WDF_DRIVER_CONFIG      config;
    WDF_OBJECT_ATTRIBUTES  attributes;
    NTSTATUS               status;

    KdPrint(("ExperimentDriver: DriverEntry - loading\n"));

    //
    // Register a cleanup callback so we get a trace message on unload,
    // even for the WDFDRIVER object itself.
    //
    WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
    attributes.EvtCleanupCallback = ExperimentEvtDriverContextCleanup;

    //
    // Point the framework at our EvtDeviceAdd callback. WDF calls it once
    // per device the driver is asked to manage.
    //
    WDF_DRIVER_CONFIG_INIT(&config, ExperimentEvtDeviceAdd);

    status = WdfDriverCreate(DriverObject,
                             RegistryPath,
                             &attributes,
                             &config,
                             WDF_NO_HANDLE);

    if (!NT_SUCCESS(status)) {
        KdPrint(("ExperimentDriver: WdfDriverCreate failed 0x%x\n", status));
        return status;
    }

    KdPrint(("ExperimentDriver: DriverEntry - loaded OK\n"));
    return status;
}

NTSTATUS
ExperimentEvtDeviceAdd(
    _In_    WDFDRIVER       Driver,
    _Inout_ PWDFDEVICE_INIT DeviceInit
    )
/*++

Routine Description:

    Called by the framework when a device is added. For this skeleton we
    simply create a framework device object with default settings.

--*/
{
    NTSTATUS   status;
    WDFDEVICE  device;

    UNREFERENCED_PARAMETER(Driver);

    PAGED_CODE();

    KdPrint(("ExperimentDriver: EvtDeviceAdd\n"));

    status = WdfDeviceCreate(&DeviceInit,
                             WDF_NO_OBJECT_ATTRIBUTES,
                             &device);

    if (!NT_SUCCESS(status)) {
        KdPrint(("ExperimentDriver: WdfDeviceCreate failed 0x%x\n", status));
    }

    return status;
}

VOID
ExperimentEvtDriverContextCleanup(
    _In_ WDFOBJECT DriverObject
    )
/*++

Routine Description:

    Called when the framework driver object is being deleted, i.e. as the
    driver unloads. Good place for one last trace message.

--*/
{
    UNREFERENCED_PARAMETER(DriverObject);

    PAGED_CODE();

    KdPrint(("ExperimentDriver: DriverContextCleanup - unloading\n"));
}
