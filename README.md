<div align="center">

# NUCLEO8

[![CI](https://github.com/gabrielaleonel/NUCLEO8/actions/workflows/ci.yml/badge.svg)](https://github.com/gabrielaleonel/NUCLEO8/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![.NET 8](https://img.shields.io/badge/.NET-8.0-purple.svg)](https://dotnet.microsoft.com/download/dotnet/8.0)
[![C](https://img.shields.io/badge/Language-C-orange.svg)](https://en.wikipedia.org/wiki/C_(programming_language))

🇺🇸 [English](#english-version) | 🇧🇷 [Português](#português)

</div>

---

# Português

Um **emulador de hardware híbrido** construído com um núcleo de emulação em C puro e uma camada de interface em C#, comunicando-se via P/Invoke.

Atualmente emula a máquina virtual **CHIP-8** com suporte completo a opcodes, debugger em tempo real, save/load states e renderização cross-platform via Avalonia UI.

```

## Sobre o Projeto

Este projeto foi criado como uma forma prática de aplicar e consolidar os conceitos
aprendidos nas minhas aulas de programação em C# e C. Em vez de apenas seguir
tutoriais, quis construir algo real e complexo o suficiente para colocar em prática:

- **Fundamentos de C**: gerenciamento manual de memória, ponteiros, structs,
  operações bit a bit, controle de fluxo de baixo nível e construção de uma
  biblioteca nativa do zero.
- **Fundamentos de C#**: programação orientada a objetos, arquitetura de aplicações,
  uso do ecossistema .NET e interoperabilidade com código nativo via P/Invoke.
- **Conceitos de programação de sistemas**: como uma CPU executa instruções, como
  memória e registradores funcionam, e como duas linguagens/runtimes diferentes
  podem se comunicar em baixo nível.

Construir um emulador foi uma ótima escolha para esse objetivo, pois exige lidar
com restrições reais (performance, organização de memória, tratamento preciso de
instruções) em vez de exercícios abstratos — mesmo sendo um projeto viável como
estudante.

## Funcionalidades

### Implementado (CHIP-8)

- Conjunto completo de instruções CHIP-8 (35 opcodes)
- Framebuffer monocromático 64x32 com escala de pixels 8x
- Timers de delay e som a 60 Hz
- Mapeamento de 16 teclas
- Painel de debug em tempo real (registradores, stack, memória, PC)
- Save/load state (serialização binária)
- Cross-platform: Windows, Linux, macOS
- Seletor de ROM com suporte a arrastar e soltar
- Controles de pausa/reset
- Testes unitários para todos os opcodes (C e C#)

### Roteiro

- [ ] Extensões Super-CHIP (SCHIP) (128x64, novos opcodes)
- [ ] Suporte a XO-CHIP
- [ ] Desassemblador de ROM no debugger
- [ ] Breakpoints e execução passo a passo
- [ ] Emulação de som (buzzer via saída de áudio)
- [ ] Suporte a NES (CPU 6502 + PPU) - [Ver Roadmap do NES](docs/NES_ROADMAP.md)
- [ ] Build para WebAssembly (versão no navegador)

## Arquitetura

```
Arquivo ROM
    |
    v
+-------------------+     P/Invoke     +-------------------+
|   Núcleo (C)      | <==============> |   App (C#)        |
|                   |                  |                   |
| chip8.dll/.so     |                  | Avalonia UI       |
| - CPU (opcodes)   |                  | - Renderizador    |
| - Memória (4KB)   |                  | - Debugger        |
| - Display (64x32) |                  | - Save/Load       |
| - Timers (60Hz)   |                  | - Input           |
| - Input (16 teclas)|                 | - Menu            |
+-------------------+                  +-------------------+
```

Consulte [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) para diagramas detalhados.

## Pré-requisitos

- **Compilador C**: GCC, Clang ou MSVC
- **CMake**: 3.20 ou posterior
- **.NET SDK**: 8.0 ou posterior
- **SO**: Windows 10+, Linux (Ubuntu 20.04+), macOS 12+

## Como Compilar

### Compilar o Núcleo C

**Linux / macOS:**
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

**Windows (MSVC):**
```batch
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

**Windows (GCC/MinGW) sem CMake:**
```bash
gcc -shared -o build\chip8.dll ^
  core\src\chip8.c core\src\cpu.c core\src\memory.c ^
  core\src\display.c core\src\timers.c core\src\input.c ^
  core\src\fontset.c -Icore\include -DCHIP8_SHARED_EXPORTS -std=c11 -O2
```

A biblioteca compartilhada estará em:
- Windows: `build/core/Release/chip8.dll`
- Linux: `build/core/libchip8.so`
- macOS: `build/core/libchip8.dylib`

### Compilar a Aplicação C#

```bash
cd app
dotnet build ChipSharp.sln --configuration Release
```

### Executar

```bash
cd app/ChipSharp.App
dotnet run
```

## Como Usar

### Carregando uma ROM

1. Execute a aplicação
2. Vá em **Arquivo > Abrir ROM...**
3. Selecione um arquivo `.ch8`, `.c8` ou `.rom`
4. A emulação inicia automaticamente

### Controles

O CHIP-8 usa um teclado hex de 16 teclas. O NUCLEO8 mapeia para seu teclado:

```
Tecla CHIP-8:    Teclado:
  1 2 3 C         1 2 3 4
  4 5 6 D         Q W E R
  7 8 9 E         A S D F
  A 0 B F         Z X C V
```

| Ação | Tecla |
|------|-------|
| Pausar/Retomar | Menu > Emulação > Pausar/Retomar |
| Reiniciar | Menu > Emulação > Reiniciar |
| Ativar Debugger | Menu > Depurar > Ativar Debugger |
| Salvar Estado | Menu > Arquivo > Salvar Estado |
| Carregar Estado | Menu > Arquivo > Carregar Estado |

### Debugger

Abra via **Depurar > Ativar Debugger**. O painel mostra:
- **Registradores**: V0-VF em hexadecimal
- **Estado**: PC, registrador I, SP, timers de delay/som
- **Stack**: Conteúdo atual da pilha de chamadas
- **Memória**: 16 bytes a partir do PC atual

### Save/Load State

Salve o estado completo do emulador em um arquivo `.sav` e restaure depois. Isso captura todos os registradores, memória, timers e estado do display.

## Testes

### Testes Unitários C (Framework Unity)

```bash
cmake -B build -DBUILD_TESTS=ON
cmake --build build
cd build
ctest --output-on-failure
```

### Testes Unitários C# (NUnit)

```bash
cd app
dotnet test ChipSharp.sln --verbosity normal
```

## Estrutura do Projeto

```
ChipSharp/
├── .github/workflows/ci.yml   # Pipeline CI do GitHub Actions
├── core/                       # Motor de emulação em C
│   ├── CMakeLists.txt          # Configuração de build
│   ├── include/chip8/
│   │   ├── chip8.h             # API pública (alvo do P/Invoke)
│   │   ├── chip8_types.h       # Estruturas de dados principais
│   │   └── chip8_export.h      # Macros de exportação cross-platform
│   └── src/
│       ├── chip8.c             # Ciclo de vida, carregamento de ROM, serialização
│       ├── cpu.c               # Ciclo fetch-decode-execute
│       ├── memory.c            # Gerenciamento de RAM 4KB
│       ├── display.c           # Framebuffer 64x32, XOR draw de sprites
│       ├── timers.c            # Timers de delay e som
│       ├── input.c             # Entrada de 16 teclas
│       └── fontset.c           # Fonte hex embutida (0-F)
├── tests/                      # Testes unitários C
│   ├── CMakeLists.txt          # Config Unity + CTest
│   ├── unity/                  # Framework de testes Unity
│   └── test_chip8.c            # 34+ testes de opcodes
├── bindings/                   # Camada P/Invoke C#
│   └── ChipSharp.Bindings/
│       └── Chip8Native.cs      # Declarações LibraryImport
├── app/                        # Aplicação C#
│   ├── ChipSharp.sln           # Arquivo de solution
│   ├── ChipSharp.App/          # Aplicação Avalonia UI
│   │   ├── Views/
│   │   │   ├── MainWindow.axaml      # Janela principal + menu
│   │   │   ├── EmulatorView.axaml    # Renderizador de display
│   │   │   └── DebugView.axaml       # Painel de debugger
│   │   └── Program.cs
│   └── ChipSharp.Tests/        # Testes de integração C#
├── roms/                       # Arquivos de ROM
├── docs/                       # Documentação
│   ├── ARCHITECTURE.md         # Diagramas de arquitetura
│   ├── OPCODES.md              # Referência completa de opcodes
│   └── NES_ROADMAP.md          # Roadmap futuro para NES
├── CMakeLists.txt              # Config CMake raiz
├── README.md                   # Este arquivo
└── LICENSE                     # Licença MIT
```

## Roteiro

- [x] Núcleo CHIP-8 em C (todos os 35 opcodes)
- [x] API pública em C para P/Invoke
- [x] Bindings C# com LibraryImport
- [x] Avalonia UI com renderização de display
- [x] Painel de debugger
- [x] Serialização de save/load state
- [x] Builds cross-platform (Windows + Linux)
- [x] Pipeline CI (GitHub Actions)
- [x] Testes unitários (C + C#)
- [ ] Suporte Super-CHIP / SCHIP
- [ ] Suporte XO-CHIP
- [ ] Execução passo a passo e breakpoints
- [ ] Desassemblador de ROM
- [ ] Saída de som (buzzer)
- [ ] Emulação de CPU do NES (6502)
- [ ] Emulação de PPU do NES
- [ ] Build para WebAssembly

## Contribuindo

Contribuições são bem-vindas! Veja como começar:

1. Faça um fork do repositório
2. Crie uma branch de feature: `git checkout -b feature/minha-feature`
3. Faça suas alterações
4. Execute os testes: `ctest` (C) e `dotnet test` (C#)
5. Faça commit com mensagens claras
6. Faça push e abra um Pull Request

### Configuração de Desenvolvimento

```bash
# Clone
git clone https://github.com/gabrielaleonel/NUCLEO8.git
cd NUCLEO8

# Build completo
cmake -B build -DBUILD_TESTS=ON
cmake --build build
cd build && ctest && cd ..

cd app && dotnet build && cd ..

# Executar
cd app/ChipSharp.App && dotnet run
```

## Licença

Este projeto está licenciado sob a licença MIT - veja o arquivo [LICENSE](LICENSE) para detalhes.

## Créditos

- [Cowgod's CHIP-8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM) - Referência principal de opcodes
- [Wikipedia - CHIP-8](https://en.wikipedia.org/wiki/CHIP-8) - Visão geral do hardware
- [Timendus CHIP-8 Test Suite](https://github.com/Timendus/chip8-test-suite) - Testes de conformidade de opcodes
- [Mastering CHIP-8](https://github.com/mattmikolay/chip8/wiki) - Análise aprofundada de instruções
- [NESdev Wiki](https://www.nesdev.org/wiki/) - Referência para implementação futura do NES
- [Avalonia UI](https://avaloniaui.net/) - Framework de UI .NET cross-platform
- [Unity Test Framework](https://github.com/ThrowTheSwitch/Unity) - Testes unitários em C

---

<a id="english-version"></a>

# 🇬🇧 English

A **hybrid hardware emulator** built with a pure C emulation core and a C# UI layer, communicating via P/Invoke.

Currently emulates the **CHIP-8** virtual machine with full opcode support, a real-time debugger, save states, and cross-platform rendering via Avalonia UI.

## Screenshot

<!-- Replace with actual screenshot or GIF -->
<!-- ![NUCLEO8 Screenshot](docs/screenshot.png) -->
```
+------------------------------------------+
| File  Emulation  Debug                  |
|------------------------------------------|
|                                          |
|    +------------------------------+     |
|    |                              |     |
|    |     CHIP-8 Display           |     |
|    |     64x32 (scaled 8x)        |     |
|    |                              |     |
|    +------------------------------+     |
|                                          |
|    Status: Running: test.ch8       60FPS |
+------------------------------------------+
```

## About This Project

This project was created as a hands-on way to apply and reinforce concepts learned
in my C# and C programming courses. Rather than just following tutorials, I wanted
to build something real and complex enough to put into practice:

- **C fundamentals**: manual memory management, pointers, structs, bitwise operations,
  low-level control flow, and building a native library from scratch.
- **C# fundamentals**: object-oriented programming, application architecture,
  .NET ecosystem usage, and interoperability with native code via P/Invoke.
- **Systems programming concepts**: how a CPU executes instructions, how memory
  and registers work, and how two different languages/runtimes communicate at
  a low level.

Building an emulator was a great choice for this goal, because it requires dealing
with real constraints (performance, memory organization, precise instruction handling)
rather than abstract exercises — while still being a viable student project.

## Features

### Implemented (CHIP-8)

- Complete CHIP-8 instruction set (35 opcodes)
- 64x32 monochrome framebuffer with 8x pixel scaling
- Delay and sound timers at 60 Hz
- 16-key input mapping
- Real-time debugger panel (registers, stack, memory, PC)
- Save/load state (binary serialization)
- Cross-platform: Windows, Linux, macOS
- ROM file picker with drag-and-drop support
- Pause/resume and reset controls
- Unit tests for all opcodes (C and C#)

### Roadmap

- [ ] Super-CHIP (SCHIP) extensions (128x64, new opcodes)
- [ ] XO-CHIP support
- [ ] ROM disassembler in the debugger
- [ ] Breakpoints and step-by-step execution
- [ ] Sound emulation (buzzer via audio output)
- [ ] NES support (6502 CPU + PPU) - [See NES Roadmap](docs/NES_ROADMAP.md)
- [ ] WebAssembly build (browser version)

## Architecture

```
ROM File
    |
    v
+-------------------+     P/Invoke     +-------------------+
|   Core (C)        | <==============> |   App (C#)        |
|                   |                  |                   |
| chip8.dll/.so     |                  | Avalonia UI       |
| - CPU (opcodes)   |                  | - Renderer        |
| - Memory (4KB)    |                  | - Debugger        |
| - Display (64x32) |                  | - Save/Load       |
| - Timers (60Hz)   |                  | - Input           |
| - Input (16 keys) |                  | - Menu            |
+-------------------+                  +-------------------+
```

See [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) for detailed diagrams.

## Prerequisites

- **C compiler**: GCC, Clang, or MSVC
- **CMake**: 3.20 or later
- **.NET SDK**: 8.0 or later
- **OS**: Windows 10+, Linux (Ubuntu 20.04+), macOS 12+

## Building

### Build the C Core

**Linux / macOS:**
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

**Windows (MSVC):**
```batch
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

**Windows (GCC/MinGW) without CMake:**
```bash
gcc -shared -o build\chip8.dll ^
  core\src\chip8.c core\src\cpu.c core\src\memory.c ^
  core\src\display.c core\src\timers.c core\src\input.c ^
  core\src\fontset.c -Icore\include -DCHIP8_SHARED_EXPORTS -std=c11 -O2
```

The shared library will be at:
- Windows: `build/core/Release/chip8.dll`
- Linux: `build/core/libchip8.so`
- macOS: `build/core/libchip8.dylib`

### Build the C# Application

```bash
cd app
dotnet build ChipSharp.sln --configuration Release
```

### Run

```bash
cd app/ChipSharp.App
dotnet run
```

## Usage

### Loading a ROM

1. Launch the application
2. Go to **File > Open ROM...**
3. Select a `.ch8`, `.c8`, or `.rom` file
4. The emulation starts automatically

### Controls

The CHIP-8 uses a 16-key hex keypad. NUCLEO8 maps it to your keyboard:

```
CHIP-8 Key:    Keyboard:
  1 2 3 C       1 2 3 4
  4 5 6 D       Q W E R
  7 8 9 E       A S D F
  A 0 B F       Z X C V
```

| Action | Key |
|--------|-----|
| Pause/Resume | Menu > Emulation > Pause/Resume |
| Reset | Menu > Emulation > Reset |
| Toggle Debugger | Menu > Debug > Toggle Debugger |
| Save State | Menu > File > Save State |
| Load State | Menu > File > Load State |

### Debugger

Open via **Debug > Toggle Debugger**. The panel shows:
- **Registers**: V0-VF in hexadecimal
- **State**: PC, I register, SP, delay/sound timers
- **Stack**: Current call stack contents
- **Memory**: 16 bytes starting at current PC

### Save/Load State

Save the complete emulator state to a `.sav` file and restore it later. This captures all registers, memory, timers, and display state.

## Testing

### C Unit Tests (Unity Framework)

```bash
cmake -B build -DBUILD_TESTS=ON
cmake --build build
cd build
ctest --output-on-failure
```

### C# Unit Tests (NUnit)

```bash
cd app
dotnet test ChipSharp.sln --verbosity normal
```

## Project Structure

```
ChipSharp/
├── .github/workflows/ci.yml   # GitHub Actions CI pipeline
├── core/                       # C emulation engine
│   ├── CMakeLists.txt          # Build configuration
│   ├── include/chip8/
│   │   ├── chip8.h             # Public API (P/Invoke target)
│   │   ├── chip8_types.h       # Core data structures
│   │   └── chip8_export.h      # Cross-platform export macros
│   └── src/
│       ├── chip8.c             # Lifecycle, ROM loading, serialization
│       ├── cpu.c               # Fetch-decode-execute cycle
│       ├── memory.c            # 4KB RAM management
│       ├── display.c           # 64x32 framebuffer, sprite XOR draw
│       ├── timers.c            # Delay and sound timers
│       ├── input.c             # 16-key input handling
│       └── fontset.c           # Built-in hex font (0-F)
├── tests/                      # C unit tests
│   ├── CMakeLists.txt          # Unity + CTest config
│   ├── unity/                  # Unity test framework
│   └── test_chip8.c            # 35+ opcode tests
├── bindings/                   # C# P/Invoke layer
│   └── ChipSharp.Bindings/
│       └── Chip8Native.cs      # LibraryImport declarations
├── app/                        # C# application
│   ├── ChipSharp.sln           # Solution file
│   ├── ChipSharp.App/          # Avalonia UI application
│   │   ├── Views/
│   │   │   ├── MainWindow.axaml      # Main window + menu
│   │   │   ├── EmulatorView.axaml    # Display renderer
│   │   │   └── DebugView.axaml       # Debugger panel
│   │   └── Program.cs
│   └── ChipSharp.Tests/        # C# integration tests
├── roms/                       # ROM files (gitignored content)
├── docs/                       # Documentation
│   ├── ARCHITECTURE.md         # System architecture diagrams
│   ├── OPCODES.md              # Complete opcode reference
│   └── NES_ROADMAP.md          # Future NES support plan
├── CMakeLists.txt              # Root CMake config
├── README.md                   # This file
└── LICENSE                     # MIT License
```

## Roadmap

- [x] CHIP-8 core in C (all 35 opcodes)
- [x] Public C API for P/Invoke
- [x] C# bindings with LibraryImport
- [x] Avalonia UI with display rendering
- [x] Debugger panel
- [x] Save/load state serialization
- [x] Cross-platform builds (Windows + Linux)
- [x] CI pipeline (GitHub Actions)
- [x] Unit tests (C + C#)
- [ ] Super-CHIP / SCHIP support
- [ ] XO-CHIP support
- [ ] Step-by-step execution & breakpoints
- [ ] ROM disassembler
- [ ] Sound output (buzzer)
- [ ] NES CPU (6502) emulation
- [ ] NES PPU emulation
- [ ] WebAssembly build

## Contributing

Contributions are welcome! Here's how to get started:

1. Fork the repository
2. Create a feature branch: `git checkout -b feature/my-feature`
3. Make your changes
4. Run tests: `ctest` (C) and `dotnet test` (C#)
5. Commit with clear messages
6. Push and open a Pull Request

### Development Setup

```bash
# Clone
git clone https://github.com/gabrielaleonel/NUCLEO8.git
cd NUCLEO8

# Build everything
cmake -B build -DBUILD_TESTS=ON
cmake --build build
cd build && ctest && cd ..

cd app && dotnet build && cd ..

# Run
cd app/ChipSharp.App && dotnet run
```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Credits

- [Cowgod's CHIP-8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM) - Primary opcode reference
- [Wikipedia - CHIP-8](https://en.wikipedia.org/wiki/CHIP-8) - Hardware overview
- [Timendus CHIP-8 Test Suite](https://github.com/Timendus/chip8-test-suite) - Opcode conformance testing
- [Mastering CHIP-8](https://github.com/mattmikolay/chip8/wiki) - In-depth instruction analysis
- [NESdev Wiki](https://www.nesdev.org/wiki/) - Reference for future NES implementation
- [Avalonia UI](https://avaloniaui.net/) - Cross-platform .NET UI framework
- [Unity Test Framework](https://github.com/ThrowTheSwitch/Unity) - C unit testing
