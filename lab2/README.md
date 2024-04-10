# Лабораторная работа 2

**Название:** "Разработка драйверов блочных устройств"

**Цель работы:** Получить знания и навыки разработки драйверов блочных устройств для операционной системы Linux.

## Описание функциональности драйвера

Один первичный раздел размером 20Мбайт и один расширенный раздел, содержащий два логических раздела размером 17Мбайт и 13Мбайт.
Каждый последующий байт должен быть результатом умножения предыдущих.

## Инструкция по сборке

1. `make all`

## Инструкция пользователя

1. Загрузка модуля: `make load`
2. Выгрузка модуля: `make unload`
3. Записать информацию можно с помощью dd
4. Считать информацию можно с помощью hexdump

## Примеры использования

### Загрузка модуля

```bash
root@cv3635969:~/lab2# make load
sudo insmod lab2.ko
root@cv3635969:~/lab2# dmesg
[2546969.197222] Major Number is : 252
[2546969.352971] THIS IS DEVICE SIZE 102400
[2546969.377318] vramdiskdriver: open 
[2546969.377362] vramdisk: Start Sector: 0, Sector Offset: 0; Buffer: 0000000030d04832; Length: 8 sectors
[2546969.377399] vramdisk: Start Sector: 40960, Sector Offset: 0; Buffer: 00000000b9006435; Length: 8 sectors
[2546969.377414] vramdisk: Start Sector: 75776, Sector Offset: 0; Buffer: 000000001012615f; Length: 8 sectors
[2546969.377422]  vramdisk: vramdisk1 vramdisk2 < vramdisk5 vramdisk6 >
[2546969.386303] vramdisk: p6 size 26624 extends beyond EOD, truncated
[2546969.390621] vramdiskdriver: closed 
[2546969.390947] vramdiskdriver: open
```

### Вывод разделов созданного диска

```bash
root@cv3635969:~/lab2# fdisk -l /dev/vramdisk
Disk /dev/vramdisk: 50 MiB, 52428800 bytes, 102400 sectors
Units: sectors of 1 * 512 = 512 bytes
Sector size (logical/physical): 512 bytes / 512 bytes
I/O size (minimum/optimal): 512 bytes / 512 bytes
Disklabel type: dos
Disk identifier: 0x36e5756d

Device         Boot Start    End Sectors Size Id Type
/dev/vramdisk1          1  40960   40960  20M 83 Linux
/dev/vramdisk2      40961 102400   61440  30M  5 Extended
/dev/vramdisk5      40962  75777   34816  17M 83 Linux
/dev/vramdisk6      75778 102401   26624  13M 83 Linux
```

### Запись + Чтение на диск

Строка на запись: ```"Masha, Dora and Lara"```

| Chars | Bytes |
| ----- | ----- |
| "Masha, " | 4d 61 73 68 61 2c 20 |
| "Dora " | 44 6f 72 61 20 |
| "and " | 61 6e 64 20 |
| "Lara" | 4c 61 72 61 |

Строка в байтах: ```bytes = [4d, 61, 73, 68, 61, 2c, 20, 44, 6f, 72, 61, 20, 61, 6e, 64, 20, 4c, 61, 72, 61]```

Код для преобразования согласно варианту:
```c
#include <ctype.h>
#include <stdio.h>

int main() {
  u_int8_t bytes[] = {0x4d, 0x61, 0x73, 0x68, 0x61, 0x2c, 0x20, 0x44, 0x6f, 0x72,
                      0x61, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x4c, 0x61, 0x72, 0x61};
  u_int8_t cur = 1;
  for (size_t i = 0; i < sizeof(bytes); ++i) {
    cur *= bytes[i];
    printf("%02x ", cur);
  }
  return 0;
}
```

Output:
```bash
4d 2d 37 58 58 20 00 00 00 00 00 00 00 00 00 00 00 00 00 00
```

Запись на диск и вывод содержимого:
```bash
root@cv3635969:~/lab2# echo "Masha, Dora and Lara" | dd of=/dev/vramdisk
0+1 records in
0+1 records out
21 bytes copied, 0.00028776 s, 73.0 kB/s
root@cv3635969:~/lab2# hexdump -C /dev/vramdisk
00000000  4d 2d 37 58 58 20 00 00  00 00 00 00 00 00 00 00  |M-7XX ..........|
```


### Измерение скорости передачи данных при копировании файлов между разделами созданного виртуального диска

#### BlockSize 1M, from vramdisk1 to vramdisk6

```bash
root@cv3635969:~/lab2# dd if=/dev/vramdisk1 of=/dev/vramdisk6 bs=1M count=12
12+0 records in
12+0 records out
12582912 bytes (13 MB, 12 MiB) copied, 0.344751 s, 36.5 MB/s
```

#### BlockSize 1M, from vramdisk5 to vramdisk6

```bash
root@cv3635969:~/lab2# dd if=/dev/vramdisk5 of=/dev/vramdisk6 bs=1M count=12
12+0 records in
12+0 records out
12582912 bytes (13 MB, 12 MiB) copied, 0.279291 s, 45.1 MB/s
```

#### BlockSize 512B, from vramdisk1 to vramdisk6

```bash
root@cv3635969:~/lab2# dd if=/dev/vramdisk1 of=/dev/vramdisk6 count=26622
26622+0 records in
26622+0 records out
13630464 bytes (14 MB, 13 MiB) copied, 1.47519 s, 9.2 MB/s
```

#### BlockSize 512B, from vramdisk5 to vramdisk6

```bash
root@cv3635969:~/lab2# dd if=/dev/vramdisk5 of=/dev/vramdisk6 count=26622
26622+0 records in
26622+0 records out
13630464 bytes (14 MB, 13 MiB) copied, 1.54288 s, 8.8 MB/s
```

### Измерение скорости передачи данных при копировании файлов между разделами виртуального и реального жестких дисков

#### BlockSize 512B, from sda1 to vramdisk1

```bash
root@cv3635969:~/lab2# dd if=/dev/sda1 of=/dev/vramdisk1 count=40960
40960+0 records in
40960+0 records out
20971520 bytes (21 MB, 20 MiB) copied, 1.86704 s, 11.2 MB/s
```

#### BlockSize 512B, from sda1 to vrakdisk6

```bash
root@cv3635969:~/lab2# dd if=/dev/sda1 of=/dev/vramdisk6 count=26622
26622+0 records in
26622+0 records out
13630464 bytes (14 MB, 13 MiB) copied, 1.39647 s, 9.8 MB/s
```

#### BlockSize 1M, from sda1 to vramdisk6

```bash
root@cv3635969:~/lab2# dd if=/dev/sda1 of=/dev/vramdisk6 bs=1M count=12
12+0 records in
12+0 records out
12582912 bytes (13 MB, 12 MiB) copied, 0.174201 s, 72.2 MB/s
```

#### BlockSize 1M, from sda1 to vramdisk1

```bash
root@cv3635969:~/lab2# dd if=/dev/sda1 of=/dev/vramdisk1 bs=1M count=20
20+0 records in
20+0 records out
20971520 bytes (21 MB, 20 MiB) copied, 0.271226 s, 77.3 MB/s
```

#### BlockSize 50M

```bash
root@cv3635969:~/lab2# dd if=/dev/sda1 of=/dev/vramdisk bs=50M count=1
1+0 records in
1+0 records out
52428800 bytes (52 MB, 50 MiB) copied, 1.11541 s, 47.0 MB/s
```
