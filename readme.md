# tsock

Groupe : Iker Saborit López - Tabatha Drumez

## Project Structure

The project is divided into three main modules:

* `cli.c/.h`: parses command-line arguments and builds a valid configuration.
* `app.c/.h`: contains the application logic, i.e. selecting the execution mode and handling message sending/receiving.
* `net.c/.h`: provides thin wrappers around socket system calls to isolate networking details and simplify the rest of
  the code.

## Design Decisions

We chose to separate CLI parsing, application logic, and networking calls to reduce coupling between responsibilities.
This separation brings several advantages:

- `main` stays very simple: it parses, prints the configuration, and delegates execution.
- Core logic does not directly depend on low-level socket API details.
- Network wrappers make errors and the behavior of each operation clearer.
- Maintenance is easier since each file has a well-defined responsibility.

Documentation is provided in the header files and focuses on how to use the functions rather than how they are
implemented. The goal is to keep the code as readable as possible, avoiding unnecessary comments about the internal
logic.

In cases where the behavior might not be immediately clear, or where “magic numbers” appear, we added comments to
clarify those parts.

## Why network wrappers?

System calls like `socket`, `bind`, `connect`, `accept`, `read`, and `write` are grouped inside `net.c` to avoid
spreading low-level
details throughout the codebase.

The goal is to centralize:

- Socket creation;
- `IPv4` address construction;
- send/receive operations;
- basic error handling.

This makes `app.c` more readable, as it mainly expresses the execution flow.

## Why a configuration structure?

Command-line parameters are stored in a tsock_config structure.

This choice allows us to:

- validate the configuration once in cli_parse;
- pass a single coherent object throughout the program;
- clearly document the parameters required for execution.

## Separate handling for TCP and UDP

TCP and UDP behave differently, so we implemented separate execution paths for each.

This avoids forcing an artificial abstraction between two protocols with distinct properties:

- UDP is connectionless and datagram-oriented;
- TCP is connection-oriented and stream-based.

## Message format

Each message starts with a 5-character message number, followed by a repeated pattern to fill the desired length.

This format makes it easy to:

- visualize message ordering;
- verify the actual transmitted length;
- quickly spot display or reception errors.

### Message count convention

In `PUITS` mode, the value `-1` means unlimited reception.

This convention is convenient because it allows expressing an “infinite” behavior without introducing an additional
boolean field in the configuration.

## Compilation

To build the project:

```
make
```

To clean generated files:

```
make clean
```

## Usage Examples

Run a TCP sink on port 5000:

```
./bin/tsock -p 5000
```

Run a TCP source sending 10 messages to localhost:

```
./bin/tsock -s 5000
```

Run a UDP source sending 20 messages of 40 bytes to my friend Tabatha:

```
./bin/tsock -s -u -n 20 -l 40 drumez 5000
```

#### Older README.md on the previous deliverables.