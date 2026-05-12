# Virtual Security System (VSS)

A professional-grade, distributed security system targeting an **AArch64 Embedded Linux** environment virtualized via **QEMU**. 

> [!IMPORTANT]  
> **Purpose & Scope:** This is an **educational practice project** designed to master low-level Linux systems programming. It is **NOT** a production system. The primary goal is to gain hands-on experience with kernel-space/user-space boundaries and high-performance IPC.

---

## 🎯 Project Purpose

This project serves as a comprehensive laboratory for practicing:
- **System Calls:** Direct usage of `open()`, `read()`, `write()`, and `ioctl()` for hardware interaction.
- **Character Device Operations:** Interfacing with Linux **Sysfs** entries for GPIO and I2C peripherals.
- **IPC (Inter-Process Communication):** Implementing high-throughput **POSIX Shared Memory** (`shm_open`, `mmap`) for real-time telemetry exchange.
- **Process Synchronization:** Mastering **Process-Shared Mutexes** and **Condition Variables** to coordinate independent processes.
- **Embedded Workflows:** Practical experience with **Cross-Compilation** (Buildroot/ARM64) and **Target Virtualization** (QEMU).
- **Concurrency:** Multi-threaded application design using `std::thread` and POSIX primitives.

---

## 🏗 System Architecture

The system follows a strict **3-Layer Architecture** distributed across two independent processes to ensure separation of concerns.

### 1. Process Model (Distributed IPC)
- **`proximity_collector` (The Producer):**
    - **HAL Layer:** Interfaces with the I2C bus via Linux Sysfs.
    - **Driver Layer:** Handles the VL53L0X Proximity Sensor logic.
    - **Logic:** Polls hardware at 50Hz and publishes a 5-sample telemetry batch to Shared Memory.
- **`security_controller` (The Consumer):**
    - **Middleware Layer:** Manages the POSIX Shared Memory consumer-side logic.
    - **App Layer:** Houses the `SecurityEngine` for noise filtering and state management.
    - **Actuator Drivers:** Controls the LED and Buzzer via GPIO Sysfs.

### 2. The 3-Layer Split
| Layer | Responsibility | Components |
| :--- | :--- | :--- |
| **HAL** | Hardware-agnostic Sysfs interfacing. | `I2CBus`, `GpioPin` |
| **Drivers** | Device-specific protocol implementation. | `ProximitySensor`, `Led`, `Buzzer` |
| **App/Engine** | Business logic, filtering, and IPC. | `SecurityEngine`, `SharedMemoryManager` |

---

## ⚡ Key Technical Features

- **POSIX Shared Memory (SHM):** High-throughput telemetry exchange between processes.
- **Process-Shared Synchronization:** Uses `pthread_mutex_t` and `pthread_cond_t` (with `PTHREAD_PROCESS_SHARED`) for zero-latency event signaling.
- **Noise Mitigation:** `SecurityEngine` utilizes a **Median Filter** on 5-sample batches to eliminate sensor spikes.
- **Hysteresis Logic:** Prevents "alarm flickering" by using distinct ON/OFF distance thresholds.
- **Fail-Safe Mechanism:** The controller monitors a "heartbeat" from the collector. If the producer crashes, the system enters a permanent alarm state.

---

## 🚀 Execution Workflow

The system provides high-level automation scripts for rapid testing.

### Phase 1: Ubuntu Host (Development)
Build for the host architecture (x86_64) and initialize mock Sysfs files:
```bash
./scripts/deploy_x64.sh
```

### Phase 2: QEMU Target (Integration)
Cross-compile for ARM64 and transfer binaries to the QEMU instance:
```bash
./scripts/deploy_arm64.sh
```

### Phase 3: Execution (Inside QEMU)
Perform these steps inside the virtualized ARM64 environment.

```bash
# 1. Initialize Target Environment
cd /root/vss
sudo ./virt_env/env_init_x64.sh

# 2. Start Hardware Emulation (Uses i2c-stub kernel module)
sudo ./virt_env/i2c_data_creator_arm64.sh &

# 3. Launch the System (Two Terminals)
# Terminal A: The Producer
./bin/proximity_collector

# Terminal B: The Consumer
./bin/security_controller
```

---

## 🧪 Verification Matrix
| Scenario | Distance | Expected Outcome |
| :--- | :--- | :--- |
| **Normal** | > 150mm | Status: "Monitoring". Actuators OFF. |
| **Intrusion** | < 100mm | Status: "ALARM". LED/Buzzer pulse. |
| **Hysteresis**| 100 -> 130mm | Alarm remains ACTIVE. |
| **Recovery** | > 150mm | Alarm stops. Resumes monitoring. |
| **Fail-Safe** | Process Kill| Heartbeat loss detected -> Permanent Alarm. |

---

## 📂 Directory Structure
```text
/
├── app/            # Application entry points (Collector/Controller)
├── drivers/        # Layer 2: Device drivers (Sensors/Actuators)
├── hal/            # Layer 1: Hardware Abstraction (I2C/GPIO)
├── middlewares/    # IPC, Shared Memory, and System Services
└── scripts/        # Build and Virtualization tools
```
