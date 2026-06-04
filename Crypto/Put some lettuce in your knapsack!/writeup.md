# TrojanCTF 2026 - Crypto - Put some lettuce in your knapsack!
Writeup by Anis Errais

### The challenge
You are provided with two files, an encryption algorithm and an encrypted message. The goal is to decrypt the message without knowing the original private key.

The encrypted message contains of two parts, separated by a newline:
- A list of decimal 32-bit integers, which represent the encrypted bytes
- A second list of decimal 32-bit integers, which represent the public key

### The solution
The encryption algorithm is a simple knapsack cipher. The public key is a list of integers that represent the weights of the items in the knapsack. The encrypted message is a list of integers that represent the total weight of the items in the knapsack for each byte of the original message.

This is equivalent to the subset sum problem: Given a set of integers (the public key) and a target integer (the encrypted byte) find a subset of the integers that sum up to the target integer.

To solve the problem, we create a python script to brute-force:
```python
from itertools import product

# Encrypted data
cts = [1935432887,936367467,3093640827,4457339060,3594566143,2779578959,936367467,3389071102,4457339060,2871800354,3703132970,3389071102,936367467,4457339060,3739280090,2985074000,3372725670,2871800354,2025372869,2779578959,1690573557,936367467,2048706533,2618519580,4457339060,2727086407,1803532396,3703132970,4440993628,1072659970,2650274688,1476657072,2544925030,2766765503,2650274688,1476657072,2544925030,2650274688,2766765503,2452703635,4480672724,2650274688,2436358203,4480672724,2872115161,3412404766,3259766831,2436358203,1896068598,2911479450,3259766831,4807548048]
# Public key
w = [1564505,112958839,936367467,959701131,1068267958,976046563,609492143,754206090]

# For every possible combination of bits, calculate the sum of the corresponding weights.
lookup = {}
for bits in product([0,1], repeat=len(w)):
    c = sum(b * wi for b, wi in zip(bits, w))
    lookup[c] = bits

# Decode: For each encrypted byte, look up the corresponding bits in the lookup table.
plaintext = ""
for c in cts:
    bits = lookup[c]
    byte = int("".join(map(str, bits)), 2)
    plaintext += chr(byte)

print(plaintext)
```

Running the brute-force script gives us the flag:
```sh
❯ python ./bruteforce.py
I love going outside! Trojan{P1CKN1CK1NG_1S_4W3S0M3}
```