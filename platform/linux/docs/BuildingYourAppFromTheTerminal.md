## How to build using Solar2DBuilder

You can build your Linux/Android apps via the terminal, instead of via the Solar2DSimulator. This is useful for continuous builds for instance.

Below, you will find explanations for each parameter, along with a real world example.

**Params**

* platform - (string) Valid values: `linux`, `android`
* appName - (string) The name of your app.
* appVersion - (string) The version of your app.
* dstPath - (string) The path that your app binary will be produced at.
* projectPath - (string) The path to your Solar2D project. Be sure to use double quotes if your path contains spaces.
* linuxtemplate - (string) The path to the linux template (for linux builds only) - If you installed Solar2DTux via our installers, this path is `/opt/Solar2D/Resources/template_x64.tgz` - Otherwise, you will find this file in `location_you_cloned_solar2d_tux_to/platform/linux/Solar2DSimulator/Resources/template_x64.tgz`.

Here is an example of a configuration file (use a .lua extension):

```
local params =
{
    platform='linux',
    appName='Fishies',
    appVersion='1.0',
    dstPath='/home/danny/Desktop',
    projectPath='/home/danny/Documents/Projects/Solar2D/SampleCode/Graphics/Fishies',
    linuxtemplate='/home/danny/Documents/Projects/Solar2D/Solar2DTux/platform/linux/Solar2DSimulator/Resources/template_x64.tgz',
}
return params
```

**Calling The Builder**

Here is an example call to execute the builder.

`./Solar2DBuilder build --lua "/home/danny/Documents/Projects/Solar2D/SampleCode/Graphics/Fishies/args.lua"`

You will find Solar2DBuilder in the following directory: `~/.local/share/Corona/Native/Corona/lin/bin/Solar2DBuilder`.