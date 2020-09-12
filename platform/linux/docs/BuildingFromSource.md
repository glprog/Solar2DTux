##Building Solar2DTux From Source

**Step 1**

 Install the latest version of the Codelite IDE (version 14) for your distribution. You will find it in your software center. On Arch based distributions, you will find this package in the AUR repository.
 
 **Step 2**
 
 Run the `setup_dev.sh` script. This script will install all the prerequisites for building Solar2DTux.

**Step 3**

Launch Codelite and select g++ or gcc as your default compiler.

**Step 4**

Right click on the `Solar2DConsole` project and click `build`. You only need to do this the first time you are building Solar2DTux. After this, it will get built automatically every time you build the Simulator project.

**Step 5**

Right click on the `Solar2DBuilder` project and click `build`. You only need to do this the first time you are building Solar2DTux and again if you make any changes to `Solar2DBuilder`.

**Step 6**

Ensure the `Solar2DSimulator` is the active target (double left click it's title in the workspace view).

**Step 7**

Change the target from `debug` to `x64 template` and then click the `build > build project` menu item.

The `x64 template` is used to produce your final app distribution for Linux builds. If you make any source changes that you wish to reflect in release builds (via the simulator build options) you must also rebuild (clean build) the x64 template.

It is very important to do a clean for the `Solar2DSimulator` project when changing between targets `debug, release and x64 template`. The reason being that certain preprocessors are set depending on those targets, so a clean build is needed to ensure they are set correctly.

**Step 8**

Change the target back to `debug` and right click on the `Solar2DSimulator` project and click the `clean` option. Then, go to the `build > run` menu item and choose `build and run` and set that option as the default. 

**Conclusion**

The Solar2DSimulator should now build and run.
