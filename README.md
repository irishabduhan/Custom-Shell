# Custom-Shell

A simple Unix-like shell implemented in C, demonstrating basic process management and command execution.

## Features

- **Built-in Commands**: Supports `pwd`, `cd`, `mkdir`, `help`, and `exit`.
- **External Commands**: Executes system commands using `execvp`.
- **Background Execution**: Run commands in the background with `&`.
- **Piping**: Supports single pipe (`|`) for command chaining.
- **Command Parsing**: Tokenizes input and handles basic command structures.

## Installation

### Prerequisites
- GCC compiler
- Unix-like environment (Linux, macOS, or WSL on Windows)

### Compilation
Compile the shell using GCC:

```bash
gcc shell.c -o shell
```

## Usage

Run the shell:

```bash
./shell
```

The shell prompt will appear as `shell>> `. Enter commands at the prompt.

### Examples

- **Built-in commands**:
  ```
  shell>> pwd
  /current/directory
  shell>> cd /tmp
  shell>> mkdir testdir
  shell>> help
  Supported commands:
  pwd
  cd <dir>
  mkdir <dir>
  ls <flags>
  exit
  help
  ```

- **External commands**:
  ```
  shell>> ls -l
  shell>> echo Hello World
  ```

- **Background execution**:
  ```
  shell>> sleep 10 &
  ```

- **Piping**:
  ```
  shell>> ls | grep txt
  ```

- **Exit the shell**:
  ```
  shell>> exit
  ```

## Limitations

- Supports only single pipes, not multiple pipes.
- No I/O redirection (`<`, `>`, `>>`).
- No command history or editing features.
- Basic error handling.
- No support for complex shell features like variables, loops, or scripting.

## Contributing

Feel free to fork and contribute improvements!
