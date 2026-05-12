# Virtual Security System - Execution & Testing Flow

This document outlines the two-tier testing strategy for the Virtual Security System (VSS). It provides step-by-step instructions for validating the system on both the Ubuntu Host (Development) and the QEMU Target (Integration).

---

## 1. Ubuntu Host (Fast Iteration & Logic Testing)
**Purpose:** Rapid validation of business logic, filtering algorithms, and IPC synchronization without the overhead of the QEMU environment.

### Automated Workflow (Recommended)
Build and initialize the mock environment in one command:
```bash
./scripts/deploy_x64.sh
```

### Manual Execution Steps
1.  **Terminal A (Collector):** `./bin/proximity_collector_host`
2.  **Terminal B (Controller):** `./bin/security_controller_host`
3.  **Terminal C (Mock Input):** Manually simulate sensor data:
    ```bash
    # Example: Set distance to 50mm (Should trigger alarm)
    echo "0x00" > /tmp/mock_sysfs/bus/i2c/devices/0-0029/1e
    echo "0x32" > /tmp/mock_sysfs/bus/i2c/devices/0-0029/1f
    ```

---

## 2. QEMU Target (Integration & Platform Verification)
**Purpose:** End-to-end verification of the ARM64 binaries and kernel-level hardware interaction.

### Automated Workflow (Recommended)
Cross-compile and transfer to QEMU in one command:
```bash
./scripts/deploy_arm64.sh
```

### Execution Steps (Inside QEMU)
1.  **Initialize Target Environment:**
    ```bash
    ssh -p 2222 root@localhost
    cd /root/vss/
    sudo ./virt_env/env_init_x64.sh
    ```
2.  **Start Hardware Emulation:**
    ```bash
    sudo ./virt_env/i2c_data_creator_arm64.sh &
    ```
3.  **Launch the System:**
    - **Terminal A:** `./bin/proximity_collector`
    - **Terminal B:** `./bin/security_controller`

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

## 🛠 Available Scripts
- **`scripts/deploy_x64.sh`**: One-step command to initialize the mock environment and build host binaries.
- **`scripts/deploy_arm64.sh`**: One-step command to cross-compile for ARM64 and deploy to QEMU.
- **`scripts/clean.sh`**: Removes all compiled binaries.
