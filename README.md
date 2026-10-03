# DriverSentinel

### Version 0.0.1-beta

🌐 **Webpage:** [DriverSentinel Homepage](https://grzesiekkedzior.github.io/DriverSentinel/)

**DriverSentinel** is a desktop application for static analysis and inspection of
Windows Portable Executable (PE) files.

The application is designed to inspect PE structures, imports, sections,
resources, certificates, strings, relocations, debug information and other
binary metadata.

DriverSentinel also integrates a **Capstone-based disassembly engine**, allowing
low-level inspection of machine code directly from the graphical interface.

The project is written in **C++20** and uses:

- **Qt 6** for the graphical user interface
- **LIEF** for parsing Portable Executable files
- **Capstone** for disassembly

![Welcome Screen](https://github.com/user-attachments/assets/839515ca-b619-4e64-9764-eb5436be30d9)

---

## Supported PE Formats

DriverSentinel analyzes Windows PE files.

It has been tested with:

- `.exe` – Windows executables
- `.dll` – Dynamic-link libraries
- `.sys` – Windows kernel drivers

Other PE/COFF-based formats may also be compatible, including:

- `.scr` – Windows screen savers
- `.ocx` – ActiveX controls
- `.efi` – UEFI applications and modules

Support for these additional formats has not yet been fully validated.
Some information may be unavailable or displayed differently depending on the
specific PE/COFF format.

DriverSentinel identifies files based primarily on their PE structure rather
than only on their file extension.

---

## Features

### General Information

Displays general file information such as:

- file path
- file size
- timestamps
- version information

### Certificates

Displays information about digital signatures, including:

- signer
- issuer
- certificate validity period

### Debug Information

Parses and displays information stored in the PE Debug Directory.

### Disassembler

Uses **Capstone** to provide low-level inspection of machine code directly from
the application.

### DOS Header

Displays information from the MS-DOS header located at the beginning of a PE
file.

### Exception Information

Parses exception handling structures stored in the PE Exception Directory.

### File Header

Displays COFF File Header information, including:

- machine type
- number of sections
- characteristics

### Function and Import Information

Displays information about:

- imported DLLs
- imported functions
- function metadata

### Optional Header

Displays important PE Optional Header fields, including:

- entry point
- image base
- subsystem
- alignment information

### Relocations

Displays relocation blocks and relocation entries used when an image cannot be
loaded at its preferred base address.

### Resources

Displays embedded PE resources such as:

- icons
- version information
- other resource entries

### Rich Header

Decodes and displays the undocumented Microsoft **Rich Header** commonly found
in PE files produced by Microsoft development tools.

### Sections

Displays detailed information about PE sections, including:

- section name
- virtual address
- virtual size
- raw size
- permissions and characteristics

### Strings

Extracts and displays ASCII and Unicode strings found inside the analyzed
binary.

---

## Typical Use Cases

DriverSentinel can be used for:

- PE file inspection
- Windows driver analysis
- static binary analysis
- reverse engineering
- malware analysis
- inspection of executable metadata
- examining imports and dependencies
- investigating suspicious PE files
- learning the internal structure of Portable Executable files

DriverSentinel is intended primarily as a static analysis and research tool.

---

## Architecture

DriverSentinel follows an **MVC-like architecture**:

- **Data** – stores parsed information from the analyzed PE file
- **Model** – exposes structured data to the graphical interface
- **Controller** – performs parsing operations and connects models with views
- **MainWindow** – coordinates controllers and provides the main user interface

```text
┌───────────┐    ┌────────┐    ┌────────────┐    ┌────────────┐
│   Data    │ →  │ Model  │ ↔  │ Controller │ ↔  │ MainWindow │
└───────────┘    └────────┘    └────────────┘    └────────────┘
```

---

## Requirements

DriverSentinel currently uses:

- C++20
- Qt 6
- LIEF
- Capstone
- CMake 3.16 or newer

The application is currently developed primarily for Windows.

---

## Building

Clone the repository:

```bash
git clone https://github.com/grzesiekkedzior/DriverSentinel.git
cd DriverSentinel
```

Make sure that **Qt 6**, **LIEF** and **Capstone** are installed and available to
CMake.

Configure the project:

```bash
cmake -S . -B build
```

Build it:

```bash
cmake --build build
```

Depending on your environment, paths to **LIEF** and **Capstone** may need to be
configured manually.

The current CMake configuration has primarily been tested on Windows.

---

## Planned Features

Future development may include:

- built-in Hex Editor for inspecting raw file bytes
- extended PE analysis
- additional reverse engineering features
- improved support for additional PE/COFF-based formats
- further malware-analysis-oriented inspection capabilities

---

## Disclaimer

> **DriverSentinel is currently in beta.**
>
> The project may contain bugs, incomplete functionality or parsing errors.
> Do not rely on DriverSentinel as the sole source of information for production
> systems, incident response or critical security analysis.
>
> Unknown or potentially malicious files should only be analyzed in an
> appropriately isolated environment.

---

## Contributing

Contributions are welcome.

If you would like to improve DriverSentinel, you can:

- report bugs
- suggest new features
- open issues
- submit pull requests
- review existing code
- improve documentation

Bug reports and code reviews are especially useful while the project is still
in beta.

---

## Support the Project

If you find DriverSentinel useful, you can support the project by:

- ⭐ starring the repository on GitHub
- 🐞 reporting bugs
- 💡 suggesting improvements
- 📂 sharing the project with people interested in reverse engineering,
  malware analysis or PE internals
- contributing code or documentation

### Donate via PayPal

You can also support development through PayPal:

[![Donate via PayPal](https://img.shields.io/badge/Donate%20via%20PayPal-00457C?logo=paypal&logoColor=white&style=for-the-badge)](https://www.paypal.com/donate/?hosted_button_id=MW4VMJ8YHSZF2)

Or scan the QR code:

![PayPal QR Code](https://github.com/user-attachments/assets/a9c86292-1220-4e7e-b7b2-6e7415075220)

---

## License

See the repository license for details.