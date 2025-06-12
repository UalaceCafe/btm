![BTM](assets/logo.png)

[![Ko-Fi](https://img.shields.io/static/v1?message=Buy%20me%20a%20coffee&logo=kofi&labelColor=ff5e5b&color=434B57&logoColor=white&label=%20)](https://ko-fi.com/ualacecafe)

## Table of contents
- [Table of contents](#table-of-contents)
- [What is BTM](#what-is-btm)
- [Formal definition](#formal-definition)
- [Definition vs formal definition](#definition-vs-formal-definition)
- [How to compile](#how-to-compile)
- [How to run](#how-to-run)
- [How to configure a BTM machine](#how-to-configure-a-btm-machine)
- [Interpreting transitions and computation steps](#interpreting-transitions-and-computation-steps)
- [Halting](#halting)
- [References](#references)

## What is BTM

BTM is a simulator written in _C_ for a programmable bounded tape Turing Machine (also called a linear bounded automaton, or LBA). It was designed as a personal learning tool for my Theory of Computation class. As such, it is supposed to be simple and fun to use, making it suitable for educational purposes for those who want a general understanding of how Turing machines and other automata work.

## Formal definition

A BTM machine is a 8-tuple:

$$M = (\Sigma, Q, \Pi, q_0, F, \beta, <, >)^*$$

where:

- $\Sigma$ is the input alphabet, consisting of _alphanumeric characters_ (i.e., `a`, `g`, `5`, `42`, etc.), the hash character `#`, the dollar sign `$` and the asterisk character `*`;
- $Q$ is the set of non-final states
  - $Q = \{ x | x \subseteq \mathbb{N} \}$
- $\Pi$ is the partial transition function, which maps a state and an input symbol to a new state, a symbol to write, and a direction to move the tape head;
- $q_0$ is the initial state
  - $q_0 = \{ 0 \}$
- $F$ is the set of final states
  - $F = \{ \mathbb{N} - Q \}$
- $\beta$ is the blank symbol, represented by the underscore character `_`;
- $<$ is the start-of-tape symbol, represented by the less than character `<`.
- $>$ is the end-of-tape symbol, represented by the greater than character `>`.

> [!NOTE]
> The definition of a BTM machine is a variation of the definitions used by _RAMOS, Marcus Vinícius Midena (2010)_ for a Turing Machine and for a Linear Bounded Automaton.

## Definition vs formal definition

As per the definition, a bounded tape Turing Machine satisfies the following conditions:

1. Its tape includes two special symbols, serving as left and right endmarkers;
2. Its transitions may not print other symbols over the endmarkers;
3. Its transitions may neither move to the left of the left endmarker nor to the right of the right endmarker;

BTM, however, does not enforce the second condition: the simulator won't prevent the user from writing symbols over the endmarkers, including the endmarkers themselves. They are not, however, considered part of the input alphabet $\Sigma$ and may only be written (and read) as part of a transition.

Also, while the tape is finite, its size can be chosen arbitrarily large - as will be seen in the [How to compile](#how-to-compile) section - so blank symbols will be added to the tape if needed, up to the end-of-tape symbol (right endmark).

## How to compile

With `make` installed, run the command

```bash
make
```

in this root directory. An executable named `btm` will be generated in the `bin` directory.

Two special symbols are defined in the `Makefile`:
  - `STEPS`: optional. If defined, the simulator will print each steps of computation and the final state of the tape.
  - `TAPE_SIZE=x`: optional. For obvious reasons, the tape is finite. If this symbol is defined, the simulator will use a tape of size `x`. Otherwise, the tape will have double the size of the input + 2 for the start-of-tape and the end-of-tape symbols, which should always be considered when defining the tape size.

Running

```bash
make clean
```

will clean the `bin` directory.

## How to run

To run the simulator, run the `btm` binary found, by default, in the `bin` directory. The simulator accepts the following command line arguments:

```
btm [-h | --help] [<config_file> <input_file>]
```

where:
- `-h` or `--help`: prints the help message and exits;
- `<config_file>`: a file containing the BTM machine configuration, which defines a set of states and transitions;
- `<input_file>`: a file containing the input to be processed by the BTM machine. It is simply a sequence of characters, as per the definition of the input alphabet $\Sigma$.

For example, to run the $a^nb^n$ language example, you can use the following command:

```bash
btm ../examples/anbn/config ../examples/anbn/input
```

> [!TIP]
> Neither the configuration file nor the input file need to be in a specific file format, as long as they are plain text files.

## How to configure a BTM machine

A BTM machine is configured using a non-empty file that defines a set of states and its transistions.

Each line that defines a transition* must be in the **exact** following format:

```
qi: x y m qj
```

where:
- `qi`: the current state, which must be a natural number, zero included. Is a non-final state;
- `x`: the current symbol, which must either belong to the input alphabet $\Sigma$, be the blank symbol `_`, the start-of-tape symbol `<` or the end-of-tape symbol `>`;
- `y`: the symbol to write on the tape, which must either belong to the input alphabet $\Sigma$, be the blank symbol `_`, the start-of-tape symbol `<` or the end-of-tape symbol `>`;
- `m`: the direction to move the tape head, which must be one of the following characters:
  - `L`: move left;
  - `R`: move right;
- `qj`: the next state, which must be a natural number, zero included. It can be either a non-final state or a final state.

As per definition, `0` is always the initial state, and the final states are all the states that are not in the set of non-final states, i.e., every `qj` that is not defined as a `qi`.

> [!IMPORTANT]
> The configuration file may contain empty lines, but every non-empty line must not deviate from the format above. Comments are not supported.

> [!TIP]
> The current states for each transition do not need to be defined in any specific order, i.e., you may define transitions from state `1` before defining transitions from state `0`, for example.

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
004. (<A,2,aBb____>)
```

This means that the simulation is currently at step `004`, the current state is `2`, the contents of the tape before the tape head are `<A`, and the contents of the tape at the tape head and forward are `aBb____>`, so the tape head is currently on the `a` character.

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
