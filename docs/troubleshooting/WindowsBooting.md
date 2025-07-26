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
If this doesn't work or doesn't show the partitions, move to the next step .

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
