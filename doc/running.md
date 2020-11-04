# Running

For executing Hyperion **server**.

## Overview

The compiled binary is designed to be executed from the command line as follows:

```bash
[executable_path] [arguments]
```

Where:

- execution_path: Path to hyperion executable
- arguments: Provided for [parameters](#parameters) in any order:

To stop hyperion, type `stop` into the hyperion executable and press enter.

For a step by step walkthrough, see [Detailed guide](#detailed-guide).

## Parameters

The following can also be obtained using `-h` or `--help` for the argument.

- **Argument identifier**: **Description**
- `-o`: **Save path**. Path to save received media. Relative to the working directory, defaults to working directory if unspecified
- `-p`: **Network port**. Network port to use, defaults to `34200`
- `--ipv4`: **Use ipv4 network protocol, otherwise ipv6**. If not defined, uses ipv6

For example to specify:

- **Output path**: /hyperion/media
- **Port**: 34560
- **ipv4 protocol**

```bash
-o /hyperion/media -p 34560 --ipv4
```

## Detailed guide

### Windows

Manual:

- Open command prompt
- Drag hyperion.exe into the command prompt to obtain its path
- Set arguments for parameters as necessary
- Hit enter to run the executable

Batch file:

In a file with the file extension `.bat` placed in the same directory as the executable:

```bat
hyperion.exe [arguments]
```

Where:

- arguments: Provided for [parameters](#parameters) in the order specified in [overview](#overview)

The above allows running the server **without** having to use a command prompt and manually typing in the save path each time.

### MacOS, Linux

Manual:

- Open a bash shell
- Change working directory as necessary
- Locate the binary `hyperion`
- Set arguments for parameters as necessary
- Run the command

Bash file:

In a file with the file extension `.sh` placed in the same directory as the executable:

```bash
#!/bin/bash
./hyperion [arguments]
```

Where:

- arguments: Provided for [parameters](#parameters) in the order specified in [overview](#overview)

The above allows running the server **without** having to use a command prompt and manually typing in the save path each time.
