# DRIVER SENTINEL
### **Version 1.0.0 - beta**

**DriverSentinel** is a desktop application for analyzing and inspecting Windows driver files (**.sys**).  
It allows users to quickly check general information, the Portable Executable (PE) structure, imported functions, and security-related data such as digital signatures.  
Additionally, it integrates a **disassembly engine** powered by **Capstone**, enabling low-level code inspection directly from the interface.

The project is developed in **C++**, using the **Qt** framework for the graphical interface, **LIEF** for parsing the PE format, and **Capstone** for disassembly.
![Welcome Screen](https://github.com/user-attachments/assets/839515ca-b619-4e64-9764-eb5436be30d9)   
![Section](https://github.com/user-attachments/assets/61be3085-be0c-4076-80c5-2a859fb51be8)   
![Imports](https://github.com/user-attachments/assets/d2e04416-9a15-4b51-a8f2-af1b5ae8b4a8)   
![Asm](https://github.com/user-attachments/assets/6dd0bbad-326a-4c6c-b5d0-9bc713d90452)   
![Rsc](https://github.com/user-attachments/assets/34ec2e16-3a0c-4bcf-93fc-0b0857e4a512)   

## Features

- **General Information**  
  Displays basic metadata such as file path, size, timestamps, and version details.

- **Certificates**  
  Shows information about the driver’s digital signature: signer, issuer, and validity period.

- **Debug**  
  Parses and displays debug-related data directories.

- **Disassembler (Capstone)**  
  Enables low-level inspection of machine code directly from the driver.

- **DOS Header**  
  Displays details of the MS-DOS header at the beginning of the file.

- **Exception**  
  Parses exception handling structures defined in the PE format.

- **File Header**  
  Shows COFF file header information such as machine type, number of sections, and characteristics.

- **Function Info**  
  Provides details about imported functions, DLL dependencies, and other related data.

- **Optional Header**  
  Displays fields from the optional header, including entry point, image base, and subsystem.

- **Relocation**  
  Lists relocation entries used for address fixing during driver loading.

- **Resource**  
  Displays embedded resources (icons, version info, etc.).

- **Rich Header**  
  Decodes and presents the hidden "Rich Header" often found in PE files.

- **Sections**  
  Lists all PE sections with detailed information (name, virtual size, raw size, permissions).

- **Strings**  
  Extracts and displays ASCII and Unicode strings from the driver binary.   

> **Disclaimer**  
> DriverSentinel is currently in **beta**.  
> The project may still contain bugs or incomplete features that have not yet been identified.  
> Use it at your own risk and do not rely on it for production or critical security analysis.   

### 💰 Donate via PayPal  
You can support TuneScape by sending a donation through PayPal:  
[![Donate via PayPal](https://img.shields.io/badge/Donate%20via%20PayPal-00457C?logo=paypal&logoColor=white&style=for-the-badge)](https://www.paypal.com/donate/?hosted_button_id=MW4VMJ8YHSZF2)

Or simply scan the QR code below:  

![Kod QR](https://github.com/user-attachments/assets/a9c86292-1220-4e7e-b7b2-6e7415075220)

### 🛠️ Other Ways to Support  
If you can't donate, you can still help by:  
- 🔍 Starring the project on GitHub to increase its visibility.  
- 🐞 Providing feedback and reporting bugs to improve DriverSentinel.  
- 📂 Sharing the app with friends and colleagues interested in reverse engineering.  

Thank you for your support! 🧩🔐   

## Contributing

Contributions are very welcome! 🎉  
If you’d like to improve DriverSentinel, feel free to open issues, submit pull requests, or suggest new features.  
Bug reports and code reviews are especially appreciated, since the project is still in beta and may contain issues I haven’t caught yet.  

## Architecture

DriverSentinel follows a clear **MVC-like structure**:

- **Data file** – stores and represents the raw driver information.  
- **Model** – structures the parsed data for use in the UI.  
- **Controller** – manages the logic, connects the model with the views, and handles parsing operations.  
- **MainWindow** – loads and orchestrates controllers, providing the main interface for the user.  

```text
┌──────────┐    ┌────────┐    ┌──────────┐    ┌────────────┐
│ Data File│ →  │ Model  │ ↔  │Controller│ ↔  │MainWindow  │   
└──────────┘    └────────┘    └──────────┘    └────────────┘

## Planned Features

- Hex Editor – a built-in viewer to inspect and edit the raw bytes of driver files.   



