# MyTerminal

MyTerminal is a lightweight Qt6-based terminal emulator built using Qt Widgets and QProcess.

It launches your system shell in interactive mode and provides a simple graphical
interface for executing commands.

No Electron. No VTE dependency. Just Qt.

---

## 🔗 Repository

https://github.com/swastikchatt-gh/probable-doodle

---

## ✨ Features

- Uses your default system shell (`$SHELL`)
- Interactive shell mode (`-i`)
- Monospace system fixed font
- Separate output and input areas
- Copy / Paste support
- Basic shell status reporting
- Environment variable setup (`TERM=xterm-256color`)
- Clean Qt Widgets UI

---

## 🖥️ Built With

- C++17
- Qt6 (Widgets module)
- QProcess
- QTextEdit
- QLineEdit
- QFontDatabase
- CMake

---

## 📦 Requirements

- Qt6 (Widgets module)
- CMake 3.16+
- GCC / Clang with C++17 support
- Linux (uses `/bin/sh` fallback if `$SHELL` not set)

---

## 🔧 Build Instructions (CMake Only)

```bash
git clone https://github.com/swastikchatt-gh/probable-doodle.git
cd probable-doodle

mkdir build
cd build

cmake ..
make
./MyTerminal
```

---

## ⌨️ Keyboard Shortcuts

| Action  | Shortcut |
|----------|----------|
| Copy     | Ctrl + C |
| Paste    | Ctrl + V |
| Quit     | Ctrl + Q |

---

## ⚙️ How It Works

- Reads `$SHELL` environment variable
- Falls back to `/bin/sh` if not set
- Launches shell with `-i` for interactive mode
- Captures `stdout` and `stderr`
- Writes user input directly to shell via `QProcess::write`
- Displays output in real-time

---

## ⚠️ Limitations

- Not a full PTY-based terminal emulator
- No true ANSI escape sequence rendering
- No tab completion integration at GUI level
- No multi-tab support
- No scrollback optimization
- No advanced terminal emulation (like xterm/vte)

This is a minimal shell frontend — not a full terminal replacement.

---

## 🚀 Planned Improvements

- PTY-based backend
- ANSI color parsing
- Proper terminal buffer handling
- Command history navigation (↑ / ↓)
- Multi-tab support
- Configurable fonts and themes
- Dark mode
- Scrollback limit control

---

## 📜 License

This project is licensed under **SLIC**.

See the LICENSE file for details.

---

## 👨‍💻 Author

Swastik Chatterjee

---

A simple terminal emulator.  
Built because we can.
