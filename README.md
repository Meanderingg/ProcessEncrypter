# ProcessEncrypter

A Linux-based C project that demonstrates word-level encryption by using multiple processes, pipes, and memory-mapped files. The project is designed as an operating-systems exercise focused on process communication and file manipulation.

## Overview

The repository contains a small encryption/decryption pipeline:

- `encr.c` reads a source file and encrypts each word using a randomized permutation.
- `decr.c` reconstructs the original content from the permutation metadata.
- `encryptor.c` is a small launcher that invokes the encryption or decryption logic in child processes.
- `makefile` compiles the binaries used by the project.

The generated encrypted representation is stored in a `.code` file, while the decrypted output is written to a `.decr` file.

## Project structure

- `encr.c` — encryption logic
- `decr.c` — decryption logic
- `encryptor.c` — process wrapper for running the encryption/decryption pipeline
- `makefile` — build script
- `*.code` / `*.decr` — example generated files
- `cp.c` �� helper file-copy utility used during development/testing

## Build

From the repository root:

```bash
make
```

This will build the binaries:

- `encryptor`
- `encr`
- `decr`

## Usage

### Encrypt a file

```bash
./encryptor example.txt
```

This produces an encrypted metadata file such as:

```bash
example.txt.code
```

### Decrypt a file

```bash
./encryptor example.txt.code example.txt
```

The project is structured around the idea of saving a sequence of per-word permutations, then reversing them during decryption.

## How it works

1. The source file is mapped into memory.
2. Words are detected and separated by whitespace.
3. For each word, a random permutation is generated.
4. The permutation is stored in a metadata file alongside the word index.
5. During decryption, the inverse permutation is applied to recover the original word order.
6. Multiple processes are used to handle word-level processing concurrently.

## Notes

This project is primarily educational and demonstrates:

- `fork()` and process creation
- inter-process communication using pipes
- file I/O and `mmap()`
- permutation-based data transformation
- Linux system programming patterns

## Example

```bash
make
./encryptor file.txt
./encryptor file.txt.code file.txt
```

## License

GPLV3
