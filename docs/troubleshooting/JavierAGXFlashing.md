# Javier AGX Flashing - Board ID not recognized
## Context

Our jetson device is a Xavier AGX system (Javier AGX DevKit 32GB) and we entered a bad state for our OS. We had some issues with the apt package manager, and had several other issues with the device related to this, like faulty drivers and other system issues. We decided to reflash the device to start from scratch and fix the issues.

We stared trying to reflash the device using the SDK Manager, but we encountered an error during the `linux flash` step. 

## Problem

When trying to flash our Xavier AGX system (Javier AGX DevKit 32GB) using the SDK Manager, the `linux flash` step fails with the following error:
```
Parsing board information failed.
```

This error doesnt specify the root cause of the problem, and since we had a bad fs state on the device, we suspected that the error was related to the previous problems. Also, other related users reported this error and relied on introspection with the EEPROM values after relying on the manual fs flashing process. First of all, in order to modify the EEPROM values, we needed a working system to access the i2c bus and modify the EEPROM values. Second, we didnt want to rely on modifying these values, since we could brick the device if we did something wrong.


> **Warning:** THIS IS NOT A TUTORIAL. This is a posmortem document of the steps we followed to fix the problem. This is not a guide to fix the problem, but a guide to understand the problem and the steps we followed to fix it.

### Step 1: Get a stable system working
If we were able to flash the device and get a stable jetpack running that was all we needed. We didnt even need to modify the EEPROM values.

The first conversation followed was [the next one](https://forums.developer.nvidia.com/t/installation-of-ubuntu-20-04-in-jetson-agx-xavier/290722)

This and reading further more about manually flashing the sample Tegra fs with the jetson linux in the device, we decided to try to flash the device manually.
- https://docs.nvidia.com/jetson/archives/r35.5.0/DeveloperGuide/IN/QuickStart.html

Now, the hypothesis here was that if we could flash the device with a working system in the eMMC or the nvme, we could then use the SDK Manager to flash the device with the wanted jetpack. (We later found out that this was the incorrect approach, since the SDK Manager's error was actually related to the EEPROM values not being recognized)

Following the steps in this [page](https://docs.nvidia.com/jetson/archives/r35.5.0/DeveloperGuide/IN/QuickStart.html) with the following resources:
- [Jetson linux release](https://developer.nvidia.com/downloads/embedded/l4t/r35_release_v5.0/release/jetson_linux_r35.5.0_aarch64.tbz2)
- [Tegra sample fs](https://developer.nvidia.com/downloads/embedded/l4t/r35_release_v5.0/release/tegra_linux_sample-root-filesystem_r35.5.0_aarch64.tbz2)

We were able to successfully flash the device with a working system **in the eMMC**.

#### Observations:

1. You might see how in the tutorial steps, the last step is to use the `flash.sh` script to flash the device, like so:
![flash.sh](/assets/troubleshooting/jetson-flashers/flash-sh-sample-usage.png)

    ```bash
    sudo ./flash.sh jetson-agx-xavier-devkit internal
    ```
    After the successful flashing, we thought that if there's a way to flash the "internal" storage, there **might** be a way to flash the nvme storage. After further reading, we found out that the `flash.sh` can receive also `external` and `nvme0n1p1`. Which we saw as a miracle that we could later try if the SDK Manager failed again.

2. Again follow with attention the command being used:

    ```bash
    sudo ./flash.sh jetson-agx-xavier-devkit internal
    ```
    The `jetson-agx-xavier-devkit` is the board ID that the script uses to flash the device. This means that the script doesn't rely on anything else but user input to flash the device. This might have been a nice hint for use related to the SDK Manager error. 


### Step 2: Flash the device with the SDK Manager

After successfully flashing the device with a working system in the eMMC, we tried to flash the device with the SDK Manager again. As you might have guessed, the error persisted.

We then tried to flash the device with the `flash.sh` script using the `external` and `nvme0n1p1` options. The flashing was "successful", but the SDK Manager still failed with the same error and the device was not booting. We learned for the helpfull [comment in this conversation](https://forums.developer.nvidia.com/t/parsing-board-information-failed/212526/4) that the `flash.sh` script DOESN'T flash the device with other storage than the eMMC.


### Step 3: Just write the EEPROM values

We followed these two conversations with related issues:

- [Cannot reflash AGX device - incorrect EEPROM values?](https://forums.developer.nvidia.com/t/cannot-reflash-agx-device-incorrect-eeprom-values/211428)
- [Parsing board information failed](https://forums.developer.nvidia.com/t/parsing-board-information-failed/212526)

And this EEPROM layout [page](https://docs.nvidia.com/jetson/archives/r34.1/DeveloperGuide/text/HR/JetsonEepromLayout.html)

Now, at first we have to state two clear problems:

1. There's no actual way to know where the EEPROM corruption is. We knew that this COULD be the problem, but we didn't know for sure and we didn't knew where exactly in our EEPROM values the corruption was other than go through the whole EEPROM values and check them one by one.
2. The only way to modify the EEPROM values is to use the `i2c` bus. This means that we needed a working system to access the EEPROM values and modify them.

Both of these problems meant that in order to fix the issue we needed to flash the device with a working system in the eMMC, change the EEPROM values and then try to flash the device with the SDK Manager.

Also, a nice resource given is the how to obtain the CRC Value of the EEPROM values [here](https://docs.nvidia.com/jetson/archives/r34.1/DeveloperGuide/text/HR/JetsonEepromLayout.html#value-of-the-crc-8-byte)

```python
def AddToCRC(b, crc):
    b2 = b
    if (b < 0):
        b2 = b + 256
    for i in range(8):
        odd = ((b2^crc) & 1) == 1
        crc >>= 1
        b2 >>= 1
        if (odd):
            crc ^= 0x8C # This means crc ^= 140.
    return crc
```
For each byte B of EEPROM content:
```python
crc = AddToCRC(B, crc)
```

First of all we retreived the EEPROM values using `i2cdump -y 0 0x50` and then we parsed the values and calculated the CRC value. 

```bash
nvidia@nvidia:~$ sudo i2cdump -y 0 0x50
No size specified (using byte-data access)
     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f    0123456789abcdef
00: 01 00 ff 00 48 0b 04 00 04 4d 00 00 00 00 00 00    ?...H??.?M......
10: 00 00 00 00 36 39 39 2d 38 32 38 38 38 2d 30 30    ....699-82888-00
20: 30 34 2d 34 30 30 20 4d 2e 30 00 00 00 00 00 00    04-400 M.0......
30: 00 00 ff ff ff ff ff ff ff ff ff ff ff ff ff ff    ................
40: ff ff ff ff d6 a4 3a 2d b0 48 31 35 36 31 32 32    ....??:-?H156122
50: 31 30 30 34 36 38 36 00 00 00 00 00 00 00 00 00    1004686.........
60: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00    ................
70: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00    ................
80: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00    ................
90: 00 00 00 00 00 00 4e 56 43 42 1c 00 4d 31 00 00    ......NVCB?.M1..
a0: ff ff ff ff ff ff ff ff ff ff ff ff d6 a4 3a 2d    ............??:-
b0: b0 48 00 00 00 00 00 00 00 00 00 00 00 00 00 00    ?H..............
c0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00    ................
d0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00    ................
e0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00    ................
f0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 f0    ...............?
```

> **Warning:** These are our EEPROM values. This are not be the same for your device. 

We made a simple script to parse the values from the EEPROM and calculate the CRC value. 

```python
def AddToCRC(b, crc):
    b2 = b
    if (b < 0):
        b2 = b + 256
    for i in range(8):
        odd = ((b2 ^ crc) & 1) == 1
        crc >>= 1
        b2 >>= 1
        if (odd):
            crc ^= 0x8C  # This means crc ^= 140.
    return crc


dump = """00: 01 00 ff 00 48 0b 04 00 04 4d 00 00 00 00 00 00    ?...H??.?M......
10: 00 00 00 00 36 39 39 2d 38 32 38 38 38 2d 30 30    ....699-82888-00
20: 30 34 2d 34 30 30 20 4d 2e 30 00 00 00 00 00 00    04-400 M.0......
30: 00 00 ff ff ff ff ff ff ff ff ff ff ff ff ff ff    ................
40: ff ff ff ff d6 a4 3a 2d b0 48 31 35 36 31 32 32    ....??:-?H156122
50: 31 30 30 34 36 38 36 00 00 00 00 00 00 00 00 00    1004686.........
60: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00    ................
70: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00    ................
80: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00    ................
90: 00 00 00 00 00 00 4e 56 43 42 1c 00 4d 31 00 00    ......NVCB?.M1..
a0: ff ff ff ff ff ff ff ff ff ff ff ff d6 a4 3a 2d    ............??:-
b0: b0 48 00 00 00 00 00 00 00 00 00 00 00 00 00 00    ?H..............
c0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00    ................
d0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00    ................
e0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00    ................
f0: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 f0    ...............?"""  # given by `i2cdump -y 0 0x50`

crc = 0
count = 0
for line in dump.split("\n"):
    for byte in line.split(" ")[1:17]:
        if count == 255:
            break
        print(f"{byte}", end=" ")
        crc = AddToCRC(int(byte, 16), crc)
        count += 1
    print()
print(f"{crc:x}")
```
        
Now, you might see that the CRC value is `f0`. And after running the script, we found out that the CRC value was `f0`. We dont really know the values were corrupted but the CRC value was correct. But we knew that the EEPROM values were corrupted.

After a simple lookup in the [EEPROM layout page](https://docs.nvidia.com/jetson/archives/r34.1/DeveloperGuide/text/HR/JetsonEepromLayout.html) we understood the layout and the values given. We saw that the board information was correctly stored (`699-82888-0004-400 M.0`) and the CRC value was correct. We quickly saw that the https://docs.nvidia.com/jetson/archives/r34.1/DeveloperGuide/text/HR/JetsonEepromLayout.html#value-of-the-crc-8-byte:~:text=2%E2%80%933,of%20EEPROM%20data. values for the "Lenght of module ID" (`2–3	254	Length of module ID data starting from this field to the end of EEPROM data.`) was `0xff` in our EEPROM values and should be 254 = `0xfe`. 

> **Warning:** This (`0xfe`) isnt the correct value for the "Lenght of module ID" field. This is just us following the layout table and the value given in it. THIS ISNT CORRECT.

![Eeprom layout table](/assets/troubleshooting/jetson-flashers/layout-eeprom.png)

We then modified the EEPROM values using the `i2cset` command. 

```bash
sudo i2cset -y 0 0x50 0x02 0xfe
```

We then calculated the CRC value again and it was `0xe9`. We then modified the CRC value in the EEPROM values.

```bash
sudo i2cset -y 0 0x50 0xff 0xe9
```

We then tried to flash the device with the SDK Manager again and it gave us the **same error again**. 

After all of this, we thought that this was a mayor error with our board. But kept looking and investigating. We dig deep into what the sdk manager scripts for the jetpack instalation and saw that the error was related to reading the eeprom values. We even modified the scripts in order to asume that the board that we were flashing was the `jetson-agx-xavier-devkit` board. After doing this, the SDK Manager was able to go forward with the flashing process, so we knew that the problem was related to the EEPROM values not being recognized by the SDK Manager.

As you might have seen in the [Cannot reflash AGX device - incorrect EEPROM values?](https://forums.developer.nvidia.com/t/cannot-reflash-agx-device-incorrect-eeprom-values/211428) conversation. This user was able to do similar steps to the ones we did, with a simple difference. the value for the "Lenght of module ID" field was `0xfc` and not `0xfe`. 

We repeated the steps changing the value of `0x02` to `0xfc` and the CRC value to `0xdb`. After doing this, the SDK Manager was able to flash the device successfully.

## Thanks to the internal response
- Leonardo Llanas.
- Diego Hernandez.
- Gerardo Fregoso.
- Oscar Arreola.
- Angel Cervantes.

Written by: Ivan Romero.