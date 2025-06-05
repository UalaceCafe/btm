# Turing Machine Lite

[![Ko-Fi](https://img.shields.io/static/v1?message=Buy%20me%20a%20coffee&logo=kofi&labelColor=ff5e5b&color=434B57&logoColor=white&label=%20)](https://ko-fi.com/ualacecafe)

Turing Machine Lite (TML) is a simulator written in _C_ of a minimalistic implementation of a Turing machine. It was designed as a personal learning tool for my Theory of Computation class. Because of that, it is supposed to be simple and fun to use, making it suitable for educational purposes for those who want a general understanding of how Turing machines work.

## Table of contents
- [Turing Machine Lite](#turing-machine-lite)
  - [Table of contents](#table-of-contents)
  - [Formal definition](#formal-definition)
  - [How to compile](#how-to-compile)
  - [How to run](#how-to-run)
  - [How to configure a TML machine](#how-to-configure-a-tml-machine)
  - [Interpreting transitions and computation steps](#interpreting-transitions-and-computation-steps)
  - [Halting](#halting)
  - [References](#references)

## Formal definition

A TML machine is a 7-tuple:

$$M = (\Sigma, Q, \Pi, q_0, F, \beta, \omicron)^*$$

where:

- $\Sigma$ is the input alphabet, consisting of _alphanumeric characters_ (i.e., `a`, `g`, `5`, `42`, etc.), the hash character `#` and the blank character ` `;
- $Q$ is the set of non-final states
  - $Q = \{ x | x \subseteq \mathbb{N} \}$
- $\Pi$ is the partial transition function, which maps a state and an input symbol to a new state, a symbol to write, and a direction to move the tape head;
- $q_0$ is the initial state
  - $q_0 = \{ 0 \}$
- $F$ is the set of final states
  - $F = \{ \mathbb{N} - Q \}$
- $\beta$ is the blank symbol, represented by the underscore character `_`;
- $\omicron$ is start-of-tape symbol, represented by the dollar sign `$`.

> [!NOTE]
> The definition of a TML machine is a variation of the definition used for a Turing Machine by RAMOS, Marcus Vinícius Midena (2010).

## How to compile

With `make` installed, run the command

```bash
make
```

in this root directory. An executable named `tml` will be generated in the `bin` directory.

Two special symbols are defined in the `Makefile`:
  - `STEPS`: optional. If defined, the simulator will print each steps of computation and the final state of the tape.
  - `TAPE_SIZE=x`: optional. For obvious reasons, the tape is finite. If this symbol is defined, the simulator will use a tape of size `x`. Otherwise, the tape will have double the size of the input + 1 for the start-of-tape symbol, which should always be considered when defining the tape size.

Running

```bash
make clean
```

will clean the `bin` directory.

## How to run

To run the simulator, run the `tml` binary found, by default, in the `bin` directory. The simulator accepts the following command line arguments:

```
tml [-h | --help] [<config_file> <input_file>]
```

where:
- `-h` or `--help`: prints the help message and exits;
- `<config_file>`: a file containing the TML machine configuration, which defines a set of states and transitions;
- `<input_file>`: a file containing the input to be processed by the TML machine. It is simply a sequence of characters, as per the definition of the input alphabet $\Sigma$.

For example, to run the $a^nb^n$ language example, you can use the following command:

```bash
tml ../examples/anbn/config ../examples/anbn/input
```

> [!TIP]
> Neither the configuration file nor the input file need to be in a specific file format, as long as they are plain text files.

## How to configure a TML machine

A TML machine is configured using a non-empty file that defines a set of states and its transistions.

Each line that defines a transition* must be in the **exact** following format:

```
qi: x y m qj
```

where:
- `qi`: the current state, which must be a natural number, zero included. Is a non-final state;
- `x`: the current symbol, which must either belong to the input alphabet $\Sigma$, be the blank symbol `_`, or the start-of-tape symbol `$`;
- `y`: the symbol to write on the tape, which must either belong to the input alphabet $\Sigma$, be the blank symbol `_`, or the start-of-tape symbol `$`;
- `m`: the direction to move the tape head, which must be one of the following characters:
  - `L`: move left;
  - `R`: move right;
- `qj`: the next state, which must be a natural number, zero included. It can be either a non-final state or a final state.

As per definition, `0` is always the initial state, and the final states are all the states that are not in the set of non-final states, i.e., every `qj` that is not defined as a `qi`.

> [!IMPORTANT]
> The configuration file may contain empty lines, but every non-empty line must not deviate from the format above. Comments are not supported.

> [!TIP]
> The current states of each transition do not need to be defined in any specific order, i.e., you may define transitions from state `1` before defining transitions from state `0`, for example.

> [!TIP]
> Identical transitions are allowed (i.e., no error will be emited if two transitions have the same current state and symbol), as the machine will always choose the first transition that matches. However, this is not recommended as it will increase the number of transitions the machine has to iterate through.

## Interpreting transitions and computation steps

After running a simulation, the program will print a summary of the configuration of the machine in the format:

```
Machine configuration: {
  Non-final states: [...]
  Non-final states count: x
  Final states: [...]
  Final states count: y
  Transitions: [
    ...
  ]
  Transitions count: z
}
```

and in the `Transitions` section you will see something that looks like this, for example:

```
(0 -> 1): (1, A, ->)
(3 -> 4): (B, _, <-)
```
This is simply another way of representing the transitions defined in the configuration file. Each line represents a transition in the form:

$$(q_i \rightarrow q_j): (x, y, m)$$

Note that the movement of the tape head is represented as `<-` for `L` and `->` for `R`.

Also, if the `STEPS` symbol was defined during compilation, the program will print each step of computation, showing the current state, the tape head position, and the contents of the tape. For example:

```
004. ($A,2,aBb_____)
```

This means that the simulation is currently at step `004`, the current state is `2`, the contents of the tape before the tape head are `$A`, and the contents of the tape at the tape head and forward are `aBb_____`, so the tape head is currently on the `a` character.

To help with the visualization of the tape, at the beginning and, if `STEPS` is defined, at end of simulation, its contents are printed.

## Halting

The machine will halt and **reject** if any of the following conditions are met:
- There is no transition for the current state and symbol;
- The tape head moved to before the leftmost position (i.e. the first position of the tape);
- The tape head moved past the rightmost position (i.e. the last position of the tape).

The machine will halt and **accept** if the following condition is met:
- The current state is a final state.

Otherwise, the machine will continue to run.

## References

- [Marcus Vinícius Midena Ramos](http://lattes.cnpq.br/7833733286842741)
