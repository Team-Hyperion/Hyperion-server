# Running

For executing Hyperion **server**.

## Parameters

- **Save path**: Path to save received media. Relative to the working directory, defaults to working directory if unspecified

The parameters above are referenced by this document in angled brackets `<>`. For example: to reference the parameter `Unicorn rainbow` is `<unicorn_rainbow>`.

## Overview

The compiled binary is designed to be executed from the command line as follows:

```bash
[executable_path] <save_path>
```

### Windows

Manual:

- Open command prompt
- Drag hyperion.exe into the command prompt to obtain its path
- Set arguments for parameters as necessary
- Hit enter to run the executable

Batch file:

In a file with the file extension `.bat` placed in the same directory as the executable:

```bat
hyperion.exe <save_path>
```

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
./hyperion <save_path>
```

The above allows running the server **without** having to use a command prompt and manually typing in the save path each time.
