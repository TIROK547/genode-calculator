# calcirok

A calculator built as a daily C++ warm-up. Targets **Genode OS** on the **NOVA** microkernel with a **Qt5** GUI, but also builds and runs on any Linux host for fast iteration.

```
Language  C++17
GUI       Qt5 Widgets
Build     CMake 3.20+
Target    Genode OS / NOVA microkernel
Repo      github.com/TIROK547/calcirok
```

---

## Running on Ubuntu (quickstart)

```bash
sudo apt update && sudo apt install -y build-essential cmake qtbase5-dev
tar -xzf calcirok.tar.gz && cd calcirok
cmake -B build -DGENODE_BUILD=OFF && cmake --build build
./build/calcirok
```

> **Ubuntu 20.04 or older?** Replace `qtbase5-dev` with `qt5-default`.

---

## Project layout

```
calcirok/
├── CMakeLists.txt                  # Dual-mode: host build or Genode cross
├── cmake/
│   └── genode_toolchain.cmake      # x86_64-pc-elf cross toolchain + NOVA flags
├── src/
│   ├── main.cpp                    # Entry point
│   ├── calculator_engine.{h,cpp}   # Pure math — no Qt widget dependency
│   ├── main_window.{h,cpp}         # Qt5 Widgets UI
│   └── calculator.{h,cpp}          # Reserved: scientific mode
└── run/
    └── calcirok.run                # Genode boot image descriptor
```

---

## Building

### Host build (Linux / macOS)

No Genode toolchain needed — just Qt5 and CMake.

```bash
cmake -B build -DGENODE_BUILD=OFF
cmake --build build
./build/calcirok
```

### Genode / NOVA target

**1. Build the Genode cross-toolchain** (one-time, ~15 min):
```bash
git clone https://github.com/genodelabs/genode.git
cd genode
./tool/tool_chain x86
```

**2. Create a build directory:**
```bash
./tool/create_builddir x86_64 BUILD_DIR=/tmp/genode-build
```

**3. Link this repo into the Genode tree:**
```bash
ln -s /path/to/calcirok /path/to/genode/repos/calcirok
```

**4a. CMake cross-build:**
```bash
cmake -B build_genode \
      -DGENODE_BUILD=ON \
      -DGENODE_DIR=/path/to/genode \
      -DCMAKE_TOOLCHAIN_FILE=cmake/genode_toolchain.cmake
cmake --build build_genode
```

**4b. Or use Genode's native make + QEMU:**
```bash
cd /tmp/genode-build
make run/calcirok KERNEL=nova
```

---

## Architecture

```
┌──────────────────────────────────────────────┐
│  NOVA hypervisor microkernel                 │
├──────────────────────────────────────────────┤
│  Genode core  │  Timer  │  FB drv  │  Input  │
├───────────────┴──────────┬───────────────────┤
│  Nitpicker (compositor)  │                   │
├──────────────────────────┘                   │
│  calcirok                                    │
│   ┌──────────────────┐  ┌─────────────────┐  │
│   │ CalculatorEngine │◄─│ MainWindow      │  │
│   │  (pure C++ math) │  │ (Qt5 Widgets)   │  │
│   └──────────────────┘  └─────────────────┘  │
└──────────────────────────────────────────────┘
```

`CalculatorEngine` has no widget dependency — the math layer can be unit-tested independently of Qt or Genode.

---

## Keyboard shortcuts

| Key | Action |
|-----|--------|
| `0`–`9`, `.` | Digit input |
| `+` `-` `*` `/` | Operator |
| `Enter` | `=` (evaluate) |
| `Escape` | AC (clear all) |
| `Backspace` | CE (clear entry) |
| `%` | Percent |
