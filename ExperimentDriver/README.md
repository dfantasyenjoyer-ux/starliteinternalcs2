# ExperimentDriver

A minimal **KMDF** (Kernel-Mode Driver Framework) Windows driver. It loads
and unloads cleanly and writes trace messages you can watch while testing.
It does nothing else yet — this is the safe, crash-free skeleton to build on.

## Files

| File | Purpose |
|------|---------|
| `Driver.c` | Driver source: `DriverEntry`, `EvtDeviceAdd`, cleanup callback. |
| `ExperimentDriver.inf` | Install script (software-only, root-enumerated). |
| `ExperimentDriver.vcxproj` | Visual Studio / MSBuild project. |

## Build (on a machine with the tools)

You need **Visual Studio 2022** with the **C++ workload**, the **WDK**
(Windows Driver Kit) matching your SDK, and the **KMDF** components.

1. Open `ExperimentDriver.vcxproj` in Visual Studio, pick `Debug | x64`,
   and **Build**. Output: `x64\Debug\ExperimentDriver\ExperimentDriver.sys`.
2. Or from a *Developer Command Prompt*:
   ```
   msbuild ExperimentDriver.vcxproj /p:Configuration=Debug /p:Platform=x64
   ```

For loading, the build produces a **test-signed** `.sys` and a `.cat`.

## Test inside your Hyper-V VM

> Do all loading/unloading in the VM. A driver bug can bugcheck (BSOD)
> the whole machine — never the host.

1. **Enable test signing** in the VM (admin prompt), then reboot:
   ```
   bcdedit /set testsigning on
   shutdown /r /t 0
   ```
   You should see a "Test Mode" watermark on the desktop.

2. **Watch trace output**: run [DebugView](https://learn.microsoft.com/sysinternals/downloads/debugview)
   as admin, and enable *Capture > Capture Kernel*.

3. **Install and start** the driver (admin prompt, from the folder with the
   `.inf` and `.sys`):
   ```
   sc create ExperimentDriver type= kernel binPath= C:\path\to\ExperimentDriver.sys
   sc start  ExperimentDriver
   ```
   DebugView should show `DriverEntry - loading` / `... loaded OK` / `EvtDeviceAdd`.

4. **Stop and remove**:
   ```
   sc stop   ExperimentDriver
   sc delete ExperimentDriver
   ```
   DebugView should show `DriverContextCleanup - unloading`.

## What this could grow into (all standard, documented Windows APIs)

- **A control device + IOCTL interface** so a user-mode app can send
  requests and read replies (`WdfIoQueue`, `WdfDeviceCreateSymbolicLink`).
- **Process / thread / image-load notifications** via
  `PsSetCreateProcessNotifyRoutineEx` and `PsSetLoadImageNotifyRoutine` —
  the basis of monitoring and EDR-style tooling.
- **A minifilter** (file-system filter) to observe file operations, built
  with FltMgr.
- **Registry filtering** via `CmRegisterCallbackEx`.
- **A WFP callout driver** to inspect or filter network traffic.
- **Kernel event tracing (ETW)** as a proper structured logging layer.

Pick a direction and we can extend the skeleton toward it.
