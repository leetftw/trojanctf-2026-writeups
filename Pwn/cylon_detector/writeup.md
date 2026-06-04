# TrojanCTF 2026 - Pwn - cylon_detector
Writeup by Anis Errais

Note: I did not save the screenshots for this challenge, as the challenge kept failing near the end of the competition, and I was in a hurry to finish it. Therefore, I do not remember the names of the binaries, and I do not have the actual flag string anymore. I did have the source code for the cylon counter saved, and I remembered what I did during the CTF, so I used that as a point of reference for writing this guide.

### The challenge
The goal is to read a file (crew list) via SSH without having permission to do so. There is a cylon counter binary, which is owned by the same user as the crew and has the `setuid` flag. It counts the number of lines equal to `CYLON` in the crew list file.

There is a second validator on the server which validates the crew list and returns a 0 or a 1. The cylon counter binary calls the validator binary after it reads the crew list. The validator binary does not have the `setuid` flag.

### The solution
It is immediately clear that the goal is to perform some sort of `setuid` attack to read the crew list. Looking at the source code for the cylon counter reveals some important information:
```c
int main() {
    int fd, admin = 0; 
    char c, content[MAX_SIZE + 1] = {0};
    pid_t frk;
    if ((fd = open(DATA, O_RDONLY)) == -1) { 
        printf("Fail...\n"); exit(1); 
    }
    ...
    (read the file)
    ...
    setreuid(getuid(), getuid());
    ...
    execve(DATA_VALIDATOR_BIN, argv, envp);
    ...
}
```

Unfortunately, the binary calls `setreuid(getuid(), getuid());` after it reads the file, so it is not just possible to overwrite the validator with our own code to read the file, since the privileges will be lost before the `execve` call. 

However, upon further inspection of the provided source code, the cylon counter binary **does not close the file descriptor** before calling the validator.

Since the file descriptor is not closed, the validator binary still has access to read the already opened file, even though the process has dropped permissions. 

We can leverage this fact by overwriting the validator binary with our own code. The server has gcc installed, so we can compile directly on the server. We first create a simple C program that lists the available file descriptors (see [test_fds.c](test_fds.c)).

We can compile this and run the cylon counter binary to see which file descriptors are still open:
```sh
$ gcc list_fds.c -o validator
$ ./cylon_counter
Start reading...Done !
Open file descriptors:
fd 0 -> /dev/pts/1
fd 1 -> /dev/pts/1
fd 2 -> /dev/pts/1
fd 3 -> /home/cylon/crew_list
fd 4 -> /proc/28315/fd
Number of cylons detected in crew list: 1
```
Our validator binary has an open file descriptor `3` for the crew list file. We can use this to read the contents of the file and extract the flag. We create a new C program that seeks to the beginning of the file (since the `cylon_counter` binary has already read it) and dumps the file contents to stdout (see [dump_fd3.c](dump_fd3.c)).

We can compile this and run the `cylon_counter` binary again to get the flag:
```sh
$ gcc dump_fd3.c -o validator
$ ./cylon_counter
Start reading...Done !
{contents of crew list file, which contains the flag}
Number of cylons detected in crew list: 1
```