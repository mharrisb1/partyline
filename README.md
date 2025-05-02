# ⛷ partyline

Like [readline](https://tiswww.case.edu/php/chet/readline/rltop.html) and [linenoise](https://github.com/antirez/linenoise/tree/master) but worse.

Purely experimental and only created to be used in my (also experimental) shell, [tinyshell](https://github.com/mharrisb1/tinyshell).

## Key Bindings

| Key Sequence                | Action                                           |
| --------------------------- | ------------------------------------------------ |
| Printable characters        | Insert character at cursor                       |
| Enter                       | Accept line; return the buffer (NULL-terminated) |
| Backspace                   | Delete the character before the cursor           |
| Option + Backspace          | Delete word before cursor                        |
| Command + Backspace (MacOS) | Delete line                                      |
| Left Arrow                  | Move cursor left                                 |
| Right Arrow                 | Move cursor right                                |
| Home                        | Move cursor to start of line                     |
| End                         | Move cursor to end of line                       |
| Option + Left Arrow         | Move cursor one word to the left                 |
| Option + Right Arrow        | Move cursor one word to the right                |
| Ctrl+C (SIGINT)             | Abort and return NULL                            |
| Ctrl+D (EOT) / EOF          | End-of-file; return NULL                         |

## Running Example

[`example.c`](./example.c) contains a simple echo application that uses `partyline` as the line editor. To run:

```sh
make
./build/partyline_example
```

To clean, use:

```sh
make clean
```

## Features/Roadmap

> [!WARNING]
> There is a near zero chance I will get to all of this. This is just a feature matrix to help me know what needs to be built.

### Core Editing

- [x] Raw-mode terminal I/O
- [x] Read single bytes/UTF-8 codepoints
- [ ] Flicker-free screen redraw (off-screen buffer + diffing)
- [x] Basic cursor movement (←/→, start/end of line)
- [x] Insert/delete character and backspace
- [x] Line kill
- [x] Word movement (Option+←/→)
- [x] Delete word (Option+Backspace)

### History & Search

- [ ] In-memory history buffer
- [ ] ⬆︎/⬇︎ to navigate past entries
- [ ] Reverse incremental search (`ctrl+r`)
- [ ] Load/save history file

### Multi-Line & Wrapping

- [ ] Automatic soft wrapping
- [ ] Edit across wrapped lines
- [ ] Paste-block (multi-line) support

### Completions

- [ ] Pluggable completer API
- [ ] Tab/Shift-Tab to cycle
- [ ] Multi-column paging of suggestions

### Key-Binding & Modes

- [ ] Emacs-style defaults
- [ ] Optional Vi-mode with Normal/Insert keymaps
- [ ] User-configurable keymap API

### Unicode & Wide-Char Support

- [ ] Correct cursor position for wide glyphs
- [ ] Handle combining/zero-width characters

### Signals

- [x] SIGNINT (`ctrl+c`) to abort current line
- [ ] SIGWINCH to recompute width & refresh display
