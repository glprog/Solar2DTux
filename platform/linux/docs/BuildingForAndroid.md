##Setting up the Android build environment.

**Step 1**

 Install the latest version of Android studio for your distribution. You will find it in your software center. On Arch based distributions, you will find this package in the AUR repository.
 
 __IMPORTANT__: The snap version of Android Studio __WILL NOT WORK__. Please ensure that you install the native version for your Linux distribution. 
 
 If a non-snap version of Android Studio is not available in your package manager, please download the zip versoin of Android Studio directly from their website and extract it to ~/android-studio. You can then launch it via `cd ~/android-studio/bin &&  .studio.sh`.

**Step 2**

After installation, launch Android Studio and install everything is requires you to install before proceeding.

**Step 3**

After step 2 is complete, Android Studio will show you a welcome popup. Click `New Project` > `No Activity` and then click `Finish`. Wait for the status bar in the bottom right corner to finish installing/downloading, then proceed to step 4.

**Step 4**

Open the `Android SDK Manager` from within Android Studio and install `Api level 28` (Android 9 Pie). Be sure to accept the license agreements prior to installation.

**Step 5**

You can now close Android Studio and forget it exists. Your Android development environment is now fully set up and you will be able to build for Android from the Solar2DTux `File > Build > Android` menu item.