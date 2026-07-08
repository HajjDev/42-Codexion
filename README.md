*This project has been created as part of the 42 curriculum by cel-hajj.*

---

## Description

Codexion is a multithreaded simulation of coders sharing USB dongles, inspired by the dining philosophers problem. Each coder sits in a circular co-working space around a shared Quantum Compiler. To compile, a coder needs to hold two dongles simultaneously — one in each hand — which are shared with their neighbors. Once done compiling, they put the dongles back, debug, refactor, and try again.

The simulation ends either when all coders have compiled the required number of times, or when one of them burns out from going too long without compiling. The goal is to prevent deadlocks, starvation, and burnout through proper thread synchronization and scheduling.

---

## Instructions

**Build:**
```
make
```

**Run:**
```
./codexion nb_coders time_burnout time_compile time_debug time_refactor nb_compiles cooldown scheduler
```

All time values are in milliseconds.

| Argument | Description |
|---|---|
| `nb_coders` | Number of coders (= number of dongles) |
| `time_burnout` | Max time (ms) a coder can go without compiling before burning out |
| `time_compile` | Time (ms) to compile (coder holds both dongles during this) |
| `time_debug` | Time (ms) spent debugging after compiling |
| `time_refactor` | Time (ms) spent refactoring after debugging |
| `nb_compiles` | Required number of compiles per coder before the sim ends |
| `cooldown` | Time (ms) a dongle must rest before it can be used again (0 = no cooldown) |
| `scheduler` | `fifo` or `edf` |

**Example:**
```
./codexion 5 800 200 100 150 3 100 fifo
```

**Cleanup:**
```
make clean    # remove .o files
make fclean   # remove .o files and binary
make re       # full rebuild
```

---

## Blocking cases handled

### Deadlock prevention — Coffman's conditions

The four Coffman conditions for deadlock are:
1. **Mutual exclusion** — each dongle is protected by its own mutex, so only one coder can hold it at a time.
2. **Hold and wait** — a coder holds the first dongle while waiting for the second.
3. **No preemption** — once a coder holds a dongle, nobody can take it away.
4. **Circular wait** — this is the one we break.

To eliminate circular wait, coders use resource ordering: odd-numbered coders take their right dongle first, then their left; even-numbered coders do the opposite. This breaks the cycle that would otherwise form in a circular arrangement, preventing deadlock entirely.

### Starvation prevention

Access to each dongle is managed through a priority queue (min-heap). Every coder that wants a dongle pushes an entry into that dongle's queue and then waits. The dongle is only granted to the coder at the front of the queue, so:
- With **fifo**, the queue is ordered by request timestamp — first come, first served.
- With **edf**, the queue is ordered by `last_compile_time + time_to_burnout`, meaning the coder closest to burnout gets priority.

EDF guarantees liveness: as a coder gets closer to burning out, their priority increases, so they will always eventually get access before the deadline — provided the parameters are feasible.

### Cooldown handling

After a dongle is released, it must rest for `dongle_cooldown` milliseconds before any coder can use it again. This is enforced in the wait loop: if a coder is at the front of the queue and the dongle is free, but the cooldown hasn't passed yet, we use `pthread_cond_timedwait` to wake up exactly when the cooldown expires, rather than busy-waiting or sleeping a fixed amount.

### Precise burnout detection

A dedicated monitor thread runs independently and checks every coder's `last_compiled_time` in a tight loop with a 1ms sleep. If any coder's elapsed time since their last compile (or since the start of the simulation, if they haven't compiled yet) reaches `time_to_burnout`, the monitor logs the burnout and stops the simulation. The burnout message is always printed within the required 10ms window.

The `last_compiled_time` and `compiles_done` fields are marked `volatile` and additionally protected by each coder's `data_mutex`: the coder thread locks it when writing these fields in `compile()`, and the monitor thread locks it when reading them in `check_coders()`. This guarantees the monitor always sees a consistent, up-to-date pair of values instead of relying on `volatile` alone.

### Log serialization

All output goes through a shared `print_mutex`. Every `printf` that logs a state change locks this mutex first and unlocks it immediately after, so two messages can never interleave on the same line.

---

## Thread synchronization mechanisms

### Mutexes

- **`dongle->mutex`** — one per dongle. Protects `in_use`, `last_used_time`, and the dongle's priority queue. Any coder that wants to read or modify a dongle's state must hold this lock.
- **`sim->print_mutex`** — shared across all threads. Every log message locks this before calling `printf` and releases it immediately after.
- **`sim->stop_mutex`** — protects the `sim_stop` flag. Both the monitor (which sets it) and coder threads (which read it via `sim_is_stopped()`) go through this mutex, eliminating the data race on the stop signal.
- **`coder->data_mutex`** — one per coder. Protects `last_compiled_time` and `compiles_done`. The coder thread locks it when updating both fields in `compile()`; the monitor thread locks it when reading them in `check_coders()`.

### Condition variables

- **`dongle->cond`** — one per dongle. Coders call `pthread_cond_wait` or `pthread_cond_timedwait` on this while waiting for their turn. When a dongle is released, `pthread_cond_broadcast` wakes all waiters so they can re-check the queue ordering.
- When the simulation stops, `broadcast_all_dongles` broadcasts on every dongle's condition variable, ensuring no thread stays blocked in `pthread_cond_wait` after the sim ends.

### Race condition prevention

- The `sim_stop` flag is always read through `sim_is_stopped()`, which locks `stop_mutex` before reading. Direct reads of `sim_stop` are avoided to prevent compiler caching or CPU reordering issues.
- Lock ordering is consistent: `stop_mutex` is always released before any `dongle->mutex` is acquired, so there is no lock-order inversion between them.
- `volatile` on `compiles_done` and `last_compiled_time` prevents the compiler from caching these values in registers, ensuring the monitor thread always sees fresh data from coder threads.

### Communication between coders and the monitor

Coders write to `last_compiled_time` and `compiles_done` under `data_mutex`, and the monitor reads them under the same lock. The monitor, when it detects a burnout or completion, sets `sim_stop = 1` under `stop_mutex` and then broadcasts all dongle condition variables. Coder threads check `sim_is_stopped()` at each step of their loop and exit cleanly if the simulation has been stopped.

---

## Resources

- [POSIX Threads Programming — Blaise Barney (LLNL)](https://hpc-tutorials.llnl.gov/posix/)
- *The Art of Multiprocessor Programming* — Herlihy & Shavit (reference for priority queues and lock-free structures)
- `pthread_cond_timedwait(3)` man page — used to implement precise cooldown wake-ups
- [Dining Philosophers Problem — Wikipedia](https://en.wikipedia.org/wiki/Dining_philosophers_problem) — background on the classic problem this is based on
- [Coffman conditions — Wikipedia](https://en.wikipedia.org/wiki/Deadlock#Coffman_conditions) — used to analyze and prevent deadlock

**AI usage:** Claude Code (Anthropic) was used during this project to help identify edge cases in the thread synchronization logic (data races on `sim_stop`, missing stop checks before `debug` and `refactor`, dongle not being released on early exit), and to fix Norm violations (function line counts, function counts per file). All generated suggestions were reviewed, understood, and adapted manually. The core design, architecture, and implementation decisions were made independently.
