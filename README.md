OpenCL Runtime（用户需安装）
----
用户和开发者都需要配置好相应的Runtime，才能在OpenCL中发现相应的设备。你希望在OpenCL中使用哪个设备，就需要安装哪个供应商的Runtime。
如果正确安装后仍旧无法找到设备，参见后文“OpenCL不列出某个设备”。

###### Apple:
从Mac OS X v10.6之后提供操作系统级别的支持。参见：
[About OpenCL for OS X](https://developer.apple.com/library/mac/documentation/Performance/Conceptual/OpenCL_MacProgGuide/Introduction/Introduction.html)

###### Intel:
需要额外安装[OpenCL驱动程序](https://software.intel.com/en-us/articles/opencl-drivers)。
在Xeon Phi上使用OpenCL还需要安装[MPSS](https://software.intel.com/en-us/articles/intel-manycore-platform-software-stack-mpss)。

###### AMD:
附带在Catalyst驱动中。

###### nVidia:
附带在显卡驱动中。

OpenCL SDK（开发者需安装）
----
对开发者而言，SDK只需要安装一个即可。建议按你最擅长调试的平台来安装。只要开发的程序中不使用供应商提供的扩展功能，任何SDK连接编译的程序都可以在全部支持OpenCL的平台上正常工作。

###### Apple:
Mac OS X v10.7之后在XCode中可以直接使用OpenCL。

###### Intel:
[Intel SDK for OpenCL Applications](https://software.intel.com/en-us/vcsource/tools/opencl-sdk)

###### AMD:
[AMD APP SDK](http://developer.amd.com/tools-and-sdks/opencl-zone/opencl-tools-sdks/amd-accelerated-parallel-processing-app-sdk/)

###### nVidia:
[nVidia CUDA Toolkit](https://developer.nvidia.com/cuda-downloads)

OpenCL不列出某个设备
----
OpenCL API所调用的动态链接库（opencl.dll）并非一份OpenCL实现，它来自于Khronos Group，称为“ICD”。ICD会从系统设置中读取供应商信息，然后依次调用供应商们提供的OpenCL实现，以使不同供应商的不同实现能够在一台机器中共存。

###### Windows：
ICD会试图读取注册表项`HKEY_LOCAL_MACHINE\SOFTWARE\Khronos\OpenCL\Vendors`中的所有`DWORD`值为0的值名，然后通过`LoadLibraryA`系统调用依次加载它们。例如：

    HKEY_LOCAL_MACHINE\SOFTWARE\Khronos\OpenCL\Vendors:
            nvopencl.dll                     dword:00000000
            C:\path\to\intel\intelocl32.dll  dword:00000000

ICD会依次加载nvopencl.dll、intelocl32.dll，然后调用它们的以下三个函数：

            clIcdGetPlatformIDsKHR
            clGetPlatformInfo
            clGetExtensionFunctionAddress

如果任一函数调用失败，则认为它不是一个OpenCL实现，抛弃它。

###### Linux:
ICD会试图读取`/etc/OpenCL/vendors/<供应商>.icd`，并且从中读取`.so`文件名。
成功加载后，调用的三个函数与Windows相同。

如果正确安装某设备的Runtime之后，OpenCL仍旧未列出该设备，你可能需要检查ICD设置，必要时向列表中手动添加供应商实现。

关于ICD的更多细节参阅：
[cl_khr_icd](http://www.khronos.org/registry/cl/extensions/khr/cl_khr_icd.txt)

Windows系统运行后掉驱动
----

Windows使用一种称为TDR的机制对显示芯片进行保护。当显卡超过2秒未响应时TDR会将显卡驱动强行重启，将显示设备上执行的Kernel抛弃。

使用[TDR Manipulator](https://forums.geforce.com/default/topic/694754/geforce-drivers/tdr-manipulator-v1-1-02-23-2014-/)小程序将TDR机制暂时关闭。

请注意，TDR是操作系统的保护机制，长期关闭有可能对硬件造成损害。
