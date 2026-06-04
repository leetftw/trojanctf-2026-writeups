# TrojanCTF 2026 - Misc - Concealed message
Writeup by Anis Errais

### The challenge
The challenge simply provides a text file to analyze and extract a flag from.

### The solution
When you open the file normally, it just looks like some Latin text. However, when using some code editors, or simply by running a hexdump, you will see that there are many hidden characters in the file:
```sh
❯ hexdump -C text.txt
...
000002d0  73 69 62 69 6c 69 73 20  73 63 72 69 70 74 61 20  |sibilis scripta |
000002e0  65 73 74 2e 20 41 62 20  6f 63 74 6f 20 62 69 74  |est. Ab octo bit|
000002f0  69 73 20 61 64 20 62 69  6e 61 72 69 75 6d 2c 20  |is ad binarium, |
00000300  65 74 20 64 65 6e 69 71  75 65 20 61 64 20 69 6e  |et denique ad in|
00000310  76 69 73 69 62 69 6c 65  2e 0a e2 80 8b e2 80 8c  |visibile........|
00000320  e2 80 8b e2 80 8c e2 80  8b e2 80 8c e2 80 8b e2  |................|
00000330  80 8b e2 80 8b e2 80 8c  e2 80 8c e2 80 8c e2 80  |................|
...
000006b0  e2 80 8c e2 80 8b e2 80  8b e2 80 8c e2 80 8b e2  |................|
000006c0  80 8b e2 80 8b e2 80 8c  e2 80 8c e2 80 8c e2 80  |................|
000006d0  8c e2 80 8c e2 80 8b e2  80 8c 0a 45 73 74 20 61  |...........Est a|
000006e0  75 74 65 6d 20 6e 61 74  75 73 20 62 65 61 74 61  |utem natus beata|
000006f0  65 20 65 6e 69 6d 2e 20  55 74 20 69 73 74 65 20  |e enim. Ut iste |
...
```
The hidden characters are all either `0xe2 0x80 0x8b` or `0xe2 0x80 0x8c`. These are UTF-8 encodings for the zero-width space and non-joiner characters, respectively. 

You can extract these characters and convert them to binary, where the space is a `0` and the non-joiner is a `1`. This will give you a binary string, which can be converted to ASCII to get the flag. 

```python
with open("text.txt", "rb") as f:
    data = f.read()
    
binary_string = ""
for i in range(0x31A, 0x6DA, 3):
    if data[i:i+3] == b'\xe2\x80\x8b':
        binary_string += "0"
    elif data[i:i+3] == b'\xe2\x80\x8c':
        binary_string += "1"

print(bytes.fromhex(hex(int(binary_string, 2))[2:]))
```

Running this script gives us the flag:
```sh
❯ python extract_flag.py
b'Trojan{0ut_0f_51ght_but_n0t_0ut_0f_m1nd}'
```