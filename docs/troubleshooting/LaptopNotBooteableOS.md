# Debugging Guide: Data Recovery with Hiren's Boot CD PE & Linux

## 📌 Problem Context
- **Initial symptom**: Disk does not appear as bootable in BIOS, but is detected in Ubuntu.
- **Possible causes**:
    - Damaged partition table (GPT/MBR).
    - Corrupted file system (NTFS/FAT32).
    - Windows hibernation (`fast startup` enabled).
    - Hardware failure (bad sectors). (Not in this case)

---

## Hardware & Software Used
<ul>
    <li><strong>Hiren's PE:</strong> A lightweight Windows environment. Download it <a href="https://www.hirensbootcd.org/download/">here</a>. It runs like Windows, allowing you to <strong>retrieve your data, run admin commands, and perform diagnostics</strong>.</li>
    <li><strong>Ubuntu Live USB:</strong> "Live" means running Ubuntu without installing it. This is very useful, especially when the system has no bootable OS.</li>
</ul>

## 🔍 Debugging Steps

### 1. Want to Retrieve Your Data?
First, download and install Hiren's PE using the link above. Boot from it and wait for the minimal Windows environment to load. Open File Explorer to check your storage devices. If nothing appears, proceed to step 2.

### 2. Check Disk Status
In Ubuntu or Ubuntu Live:
```bash
# From Ubuntu/Live USB:
sudo fdisk -l                         # List disks/partitions
sudo gdisk -l /dev/sdX                # Show GPT details
sudo ntfsfix -d /dev/sdXN             # Repair NTFS (read-only)
```
If this doesn't work or doesn't show the partitions, move to the next step.

### 3. Restore Partition Table
First, in Linux install gdisk:
```bash
sudo apt update # Ensure you are connected to the internet
sudo apt install gdisk
```

Then run:
```bash
sudo gdisk /dev/sdX # Replace sdX with your disk identifier
```
If prompted to select between options (e.g., corrupt or new table), you can ignore and use the following commands:

<ul>
    <li> r &rarr; Recovery mode </li>
    <li> v &rarr; Check integrity </li>
    <li> p &rarr; Show partitions </li>
    <li> w &rarr; Write changes <strong>(be careful if you are unsure about the changes)</strong> </li>
</ul>

### 4. Solve Windows Hibernation
In Hiren's PE (CMD as admin):

These commands may help, but in this case, they did not resolve the issue.
```bash
powercfg /h off                       # Disable hibernation
chkdsk X: /r                          # Repair errors (X: = drive letter)
```

### 5. Repair BCD in UEFI Systems (Hiren’s Boot PE)
If the BIOS does not detect the SSD as bootable and tools like EasyBCD show "Would you like to manually load a BCD?", you must manually rebuild the EFI bootloader.

#### Steps to Fix BCD (UEFI + GPT Disks)
1. Open CMD as Administrator in Hiren's Boot PE.
2. Launch `diskpart` and list volumes:
    ```cmd
    diskpart
    list vol
    ```
3. Identify:
    - The Windows partition (e.g., C: – contains C:\Windows, Users, etc.)
    - The EFI partition (usually 100–500 MB, FAT32, may not have a drive letter)
4. If the EFI partition lacks a drive letter, assign one (e.g., S:):
    ```cmd
    select volume X        # Replace X with EFI volume number
    assign letter=S
    exit
    ```
    If the EFI partition does not exist, open Disk Management and check if any partition matches the previous requirements (100–500 MB, FAT32).

5. Use `bcdboot` to rebuild the boot files:
    ```cmd
    bcdboot C:\Windows /s S: /f UEFI # Where S: its the partition of 500 MB
    ```
6. Verify that `S:\EFI\Microsoft\Boot\BCD` now exists.
7. Reboot the system. Enter BIOS/UEFI and you can ensure:
    - UEFI mode is enabled
    - Secure Boot is disabled
    - The SSD now appears in the boot order

This last step doesnt need to be realize it should work after rebooting 

> ⚠️ **DO NOT** use `bootrec /fixmbr` or `/fixboot` on GPT systems!  
> These commands are for Legacy BIOS + MBR setups and may damage your GPT-based bootloader.

If you have further inquiries, please refer to Hector.