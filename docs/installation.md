# Installation Instructions

There are two ways to install and run the application:

---

## 1. Using the Installer (Recommended)

This is the easiest method and suitable for most users.

### Steps:
1. Go to the GitHub Releases page.
2. Download the latest installer.
3. Run the installer and follow the on-screen instructions.
4. A shortcut will be created on your Start Menu.

> ⚠️ Note: The VR features require a connected VR headset (e.g., Oculus Rift, HTC Vive).

### What's Included in the Installer:
- Qt runtime DLLs
- VTK libraries
- OpenVR SDK runtime
- Uninstaller

---

## 2. Building from Source (For Developers)

Use this method if you want to modify or debug the application.

### Requirements:
- **CMake** >= 3.20
- **Qt** >= 6.4
- **VTK** >= 9.2
- **OpenVR SDK**
- **Visual Studio** (Windows)


**2. Building from Source (for developers):**

   -   Ensure you have the required dependencies installed (CMake, Qt, VTK, OpenVR SDK, Visual Studio on Windows).
   -   Use CMake to generate the build files.
   -   Or alternatively compile the application through the CmakeLists.txt file selecting an above recommended version of Qt.