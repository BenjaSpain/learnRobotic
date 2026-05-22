# Intermediate (WiP)
[Intermediate](https://docs.ros.org/en/jazzy/Tutorials/Intermediate.html)

## Managing Dependencies with rosdep (30')
### What is `rosdep`?
- Open-source command-line utility for identifying and installing dependencies to build or install packages
- Use `apt` (Debian/Ubuntu) or `dnf` (Fedora/RHEL) to install dependencies

### About `package.xml` files
- `package.xml` is the file where `rosdep` find the set of dependencies
- Dependencies in `package.xml` generally referred as `rosped keys`
- Some relevant tags:
    - `<depend>`
        - Dependencies for build and run time
        - Notes:
            - `C++` packages: If doubt, use this tag for dependencies
            - `Python` packages: Never use this tag, use `<exec_depend>`
    - `<build_depend>`:
        - Dependencies only required for build, not for executions
        - Can create problems if package exports a header that include header from this dependency. In this case use `<build_export_depend>`
    - `<build_export_depend>`:
        - If export a header that includes a header from a dependency, it will be needed by other packages that `<build_depend>` on your package
        - Library packages referenced by libraries you export should normally specify `<depend>`, because they are also needed at execution time
    - `<exec_depend>`:
        - Dependencies while running package for *shared libraries*, *excutables*, *Python modules*, *launch scripts* and others
    - `<test_depend>`:
        - Dependencies needed **only** by test.

### How does `rosdep` work?
1) `rosdep` check `package.xml` files in current path or specific package to find `rosdep keys` stored within
2) Find appropiate ROS packages or software library in various package managers 
3) Install its
- Central index is known as `rosdistro`

### What keys to put in my `package.xml`?
- If package is **ROS-Based** AND released into ROS ecosystem
    - Just use name of the package
    - * List of all released ROS packages https://github.com/ros/rosdistro in your given ROS distribution (*`<distro>/distribution.yaml`*)
- If package is **non-ROS Based**
    - System dependencies need to find keys for a particular library
    - In teneral, 2 files of interes:
        - [rosdep/base.yaml](https://github.com/ros/rosdistro/blob/master/rosdep/base.yaml): ´apt´ system dependencies
        - [rosdep/python.yaml](https://github.com/ros/rosdistro/blob/master/rosdep/python.yaml): Python dependencies
- Steps:
1) Search for your library key in these files and find the name
2) Put key in a `package.xml` file

### What if library is not in `rosdistro`?
- You can add it by Pull request to rosdistro
- [Instructions to rosdistro contributions](https://github.com/ros/rosdistro/blob/master/CONTRIBUTING.md#rosdep-rules-contributions)

### How use `rosdep` tool?
- `rosdep` installation:
    - `rosdep` is packaged along ROS. Add package to system (recommended): `apt-get install python3-rosdep`
        ```bash
        # Init workspace and move to root
        cd ~/learnRobotic/ && source ros2_env_conf.sh && cd ros2_ws
        # Install rosdep package
        sudo apt-get install python3-rosdep
        ```
    - To use `rosdep` outside ROS. Install it directly:  `pip install rosdep`
- `rosdep` operation:
    - First time:
        ```bash
        # Initialize rosdep
        sudo rosdep init
        # Update local cached rosdistro index
        rosdep update
        ```
    - Run `rosdep` to install dependencies:
        ```bash
        rosdep install --from-paths src -y --ignore-src
        ```
