# SimpleShell

A lightweight custom shell implementation written in C that supports common shell operations and commands.

## Features

- **Interactive and Batch Modes**: Launch the shell in interactive mode or run a file of commands in batch mode
- **Built-in Commands**: Support for essential shell commands like `cd`, `pwd`, `exit`, `history`, and more
- **I/O Redirection**: Redirect input (`<`) and output (`>`) streams
- **Pipe Support**: Chain multiple commands with pipes (`|`)
- **Background Processing**: Run processes in the background using `&`
- **Command History**: Store and recall previous commands with the `history` command or `!n` syntax
- **Custom Path Management**: Set custom search paths for executable commands

## Installation

```bash
# Clone the repository
git clone https://github.com/your-username/simple-shell.git

# Navigate to the project directory
cd simple-shell

# Compile the shell
make

# Run the shell
./shell
```

## Usage

### Interactive Mode

Start the shell and enter commands at the prompt:

```bash
./shell
$> ls -la
$> pwd
$> cd /tmp
```

### Batch Mode

Run a file of commands:

```bash
./shell batch_commands.txt
```

## Supported Commands

### Built-in Commands

| Command | Description | Usage |
|---------|-------------|-------|
| `exit` | Exit the shell | `exit` |
| `cd` | Change directory | `cd [directory]` |
| `pwd` | Print working directory | `pwd` |
| `history` | Show command history | `history` |
| `path` | Set the command search path | `path [dir1] [dir2] ...` |
| `kill` | Send a signal to a process | `kill [-signal] pid` |

### Features

| Feature | Description | Example |
|---------|-------------|---------|
| I/O Redirection | Redirect input/output | `ls > output.txt` or `sort < input.txt` |
| Pipes | Chain commands | `ls -la | grep .txt | wc -l` |
| Background Processing | Run commands in background | `long_process &` |
| Command History | Run previous commands | `!5` (runs the 5th command in history) |

## Technical Details

### Shell Implementation

The shell follows this general flow:
1. Read input from user or batch file
2. Parse input to identify commands and special characters
3. Execute built-in commands directly
4. Search for external commands in specified paths
5. Handle I/O redirection, pipes, and background processes
6. Wait for processes to complete (unless running in background)

### Code Structure

The implementation includes the following key functions:

- `shellLoop()`: Main loop for interactive mode
- `batchMode()`: Processes commands from a file
- `parseInput()`: Tokenizes the command line
- `execCommand()`: Executes external commands
- `builtins()`: Handles built-in command execution
- `redirectIn()/redirectOut()`: Manages I/O redirection
- `pipes()`: Implements the pipe functionality
- `parallelProcess()`: Handles background processes

## Project Structure

```
simple-shell/
├── shell.c           # Main shell implementation
├── README.md         # This file
├── Makefile          # Build configuration
└── examples/         # Example batch files
    └── commands.txt  # Sample commands
```

## Limitations

- Limited support for complex quoting and escaping
- No support for shell scripting constructs (if/else, loops)
- No job control beyond basic background processing
- Single-core focused implementation

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is licensed under the MIT License - see the LICENSE file for details.
