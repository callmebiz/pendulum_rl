# Visual Studio Community Setup - Pendulum ML Project

## Project Location
**Path:** `E:\Projects\pendulum_rl`

---

## Initial Setup Steps Completed

### 1. Created Project Structure
```powershell
cd E:\Projects\pendulum_rl
mkdir src, assets, assets\shaders, external, data
git init
```

### 2. Added All Source Files
- CMakeLists.txt (root)
- Shader files in `assets/shaders/`
- All .h and .cpp files in `src/`

### 3. Downloaded GLAD Manually
- Downloaded from: https://glad.dav1d.de/
- Settings used:
  - Language: C/C++
  - Specification: OpenGL
  - Profile: Core
  - API gl: Version 4.3+
  - Generate a loader
- Extracted to: `E:\Projects\pendulum_rl\external\glad\`

### 4. Added Git Submodules
```powershell
cd E:\Projects\pendulum_rl
git submodule add https://github.com/glfw/glfw.git external/glfw
git submodule add https://github.com/g-truc/glm.git external/glm
git submodule add https://gitlab.com/libeigen/eigen.git external/eigen
git submodule add https://github.com/ocornut/imgui.git external/imgui
```

---

## Visual Studio Community Configuration

### Opening the Project
1. **File → Open → Folder**
2. Selected: `E:\Projects\pendulum_rl`
3. VS detected CMakeLists.txt automatically
4. CMake generation ran automatically

### Common Issues Fixed

#### Issue 1: CMakeLists.txt Typo
**Error:** `Unknown CMake command "ecmake_minimum_required"`

**Fix:** 
- Opened CMakeLists.txt
- Line 1 had typo: `ecmake_minimum_required`
- Changed to: `cmake_minimum_required(VERSION 3.15)`
- Saved file (Ctrl+S)
- CMake auto-regenerated

#### Issue 2: Wrong Target Selected
**Error:** Tried to build `example_allegro5` instead of `PendulumML`

**Fix:**
- Changed platform from **Win32** to **x64** (top toolbar dropdown)
- Clicked dropdown next to green ▶ button
- Selected **PendulumML.exe**

#### Issue 3: Couldn't Find PendulumML in Solution Explorer
**Fix:**
1. In Solution Explorer, clicked **"Switch between solutions and available views"** icon
2. Selected **"CMake Targets View"**
3. Now could see PendulumML executable
4. Right-clicked **PendulumML** → **Set as Startup Item**

---

## Current Configuration (Working)

### Build Settings
- **Configuration:** Debug
- **Platform:** x64
- **Startup Item:** PendulumML.exe

### Project Structure in VS
```
📁 pendulum_rl/
├── 📁 src/
│   ├── main.cpp
│   ├── Shader.h / Shader.cpp
│   ├── Renderer.h / Renderer.cpp
│   ├── Cart.h / Cart.cpp
│   ├── Pendulum.h
│   ├── SinglePendulum.h / SinglePendulum.cpp
│   ├── DoublePendulum.h / DoublePendulum.cpp
│   └── InputController.h / InputController.cpp
├── 📁 assets/
│   └── 📁 shaders/
│       ├── basic.vert
│       └── basic.frag
├── 📁 external/
│   ├── glad/
│   ├── glfw/
│   ├── glm/
│   ├── eigen/
│   └── imgui/
└── CMakeLists.txt
```

### Build Output Location
**Executable:** `E:\Projects\pendulum_rl\out\build\x64-Debug\bin\PendulumML.exe`

---

## How to Build & Run

### Building
- **Method 1:** Press **Ctrl+Shift+B** (Build All)
- **Method 2:** Build → Build All
- **Method 3:** Click green ▶ button (builds and runs)

### Running
- **With Debugging:** Press **F5**
  - Can set breakpoints
  - Slower startup
  - Stops at errors
  
- **Without Debugging:** Press **Ctrl+F5**
  - Faster
  - Better for quick testing
  - Console window stays open after exit

### Expected Output
Two windows appear:
1. **Console Window** - Shows debug output:
   ```
   OpenGL Version: ...
   GLSL Version: ...
   === Controls ===
   A/D: Move cart left/right
   SPACE: Toggle single/double pendulum
   R: Reset simulation
   ESC: Quit
   ```

2. **Graphics Window** - Shows pendulum simulation
   - Cart on rail
   - Pendulum (yellow or yellow+blue)
   - ImGui panel with stats

---

## VS Community Keyboard Shortcuts (Useful)

### Building & Running
- **Ctrl+Shift+B** - Build All
- **F5** - Start Debugging
- **Ctrl+F5** - Start Without Debugging
- **Shift+F5** - Stop Debugging

### Editing
- **Ctrl+K, Ctrl+C** - Comment selection
- **Ctrl+K, Ctrl+U** - Uncomment selection
- **Ctrl+K, Ctrl+D** - Format document
- **F12** - Go to definition
- **Alt+F12** - Peek definition
- **Ctrl+.** - Quick actions (add include, etc.)

### Navigation
- **Ctrl+,** - Search files/symbols
- **Ctrl+Tab** - Switch between open files
- **Ctrl+F** - Find in file
- **Ctrl+Shift+F** - Find in all files

### View
- **Ctrl+Alt+O** - Output window
- **Ctrl+\, E** - Error List
- **Ctrl+Alt+L** - Solution Explorer

---

## Important Windows in VS

### Solution Explorer (Right side)
Shows your project files and structure.
- Switch between "Folder View" and "CMake Targets View"

### Output Window (Bottom)
- Switch between different output types:
  - **Build** - Compilation messages
  - **CMake** - CMake generation messages
  - **Debug** - Runtime debug output

### Error List (Bottom)
Shows compilation errors and warnings.

---

## CMake Regeneration

CMake automatically regenerates when you:
- Save CMakeLists.txt
- Add/remove files listed in CMakeLists.txt
- Change external dependencies

To **manually regenerate:**
1. Right-click project in Solution Explorer
2. Select "Delete Cache and Reconfigure"

---

## Cleaning the Build

If things get weird:

### Method 1: Rebuild All
- **Build → Rebuild All**

### Method 2: Delete Build Folder
1. Close VS
2. Delete `E:\Projects\pendulum_rl\out\` folder
3. Reopen VS
4. CMake regenerates from scratch

### Method 3: CMake Cache
- Right-click project → **Delete Cache and Reconfigure**

---

## Adding New Files to Project

When you create new .cpp files:

1. **Add file to your src/ folder**
2. **Edit CMakeLists.txt:**
   ```cmake
   add_executable(PendulumML
       src/main.cpp
       src/Shader.cpp
       src/YourNewFile.cpp  # <-- Add here
       # ... other files
   )
   ```
3. **Save CMakeLists.txt**
4. VS auto-regenerates CMake
5. Build normally

---

## Current Working Controls

### Simulation Controls
- **A** - Move cart left
- **D** - Move cart right
- **SPACE** - Toggle single ↔ double pendulum
- **R** - Reset simulation
- **ESC** - Quit

### ImGui Panel
- Shows FPS
- Shows cart position, velocity, force
- Shows pendulum angles
- **Friction slider** - Adjust friction in real-time

---

## Next Steps - Phase 2

Once comfortable with Phase 1:
1. Implement Dormand-Prince RK8 solver
2. Improve physics accuracy
3. Energy conservation testing
4. Compare with chaos video quality

---

## Troubleshooting Reference

### CMake Won't Generate
- Check CMakeLists.txt for typos
- Verify all external libraries exist
- Run `git submodule update --init --recursive`

### Can't Find PendulumML.exe
- Switch to CMake Targets View
- Check if CMake generation succeeded
- Look in Output → CMake for errors

### Build Errors
- Check Error List window
- Verify all .cpp files are in CMakeLists.txt
- Make sure platform is x64, not Win32

### Program Runs But Black Screen
- Check console for OpenGL errors
- Verify shaders copied to bin/assets/shaders/
- Check Output → Debug for error messages

### ImGui Not Showing
- Make sure imgui is built and linked
- Check that ImGui initialization succeeded (console output)

---

## Project Status

**Phase 1 Complete:**
- CMake project configured
- All dependencies installed
- VS Community properly set up
- Program compiles and runs
- Both single and double pendulum working
- Manual control functional
- Ready for Phase 2!

---

## Backup & Version Control

### Important to Commit to Git:
```bash
git add src/
git add assets/
git add CMakeLists.txt
git add .gitmodules
git commit -m "Initial Phase 1 implementation"
```

### Don't Commit (Already in .gitignore):
- `out/` folder (build artifacts)
- `.vs/` folder (VS settings)
- CMake cache files

---

## Quick Reference Card

**Open Project:** File → Open → Folder → `E:\Projects\pendulum_rl`

**Select Target:** Dropdown next to ▶ → PendulumML.exe

**Build:** Ctrl+Shift+B

**Run:** F5 (debug) or Ctrl+F5 (fast)

**Edit Files:** Solution Explorer → src → double-click file

**Find Files:** Ctrl+, then type filename

**Regenerate CMake:** Right-click project → Delete Cache and Reconfigure

---

Last Updated: [Current Date]
Project Status: Phase 1 Working