# 📂 CAD Viewer Application Project Structure 📂

## 🌳 Project File Structure 🌳
├── CMakeLists.txt

├── Icons/

├── icons.qrc

├── main.cpp

├── mainwindow.cpp/h/ui

├── ModelPart.cpp/h

├── ModelPartList.cpp/h

├── colourdialog.cpp/h/ui

├── optiondialog.cpp/h/ui

├── vrrenderthread.cpp/h


## 📄 File Descriptions 📄

**`CMakeLists.txt`**
- Used to compile build files and links libraries if using without the installer executable.

**`Icons/`**
- Stores the resource file of the Icons.

**`icons.qrc`**
- Resource file containing the application's icons.

**`main.cpp`**
- Main file for the application.

**`mainwindow.cpp/h/ui`**
- Responsible for the GUI implementation and linking the behaviour of the button presses to other corresponding actions in the code.

**`ModelPart.cpp/h`**
- Represents a single CAD part/model loaded from an STL or CAD file.

**`ModelPartList.cpp/h`**
- Manages a list of ModelPart objects, providing functionality to handle multiple loaded models.

**`colourdialog.cpp/h/ui`**
- Modal dialog used to change the colour of a selected model allowing for user selection of colours.

**`optiondialog.cpp/h/ui`**
- Used for filter settings or toggling different settings.

**`vrrenderthread.cpp/h`**
- Handles VR mapping and projection into VR software.