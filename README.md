OpenCL Runtime���û��谲װ��
====
�û��Ϳ����߶���Ҫ���ú���Ӧ��Runtime��������OpenCL�з�����Ӧ���豸����ϣ����OpenCL��ʹ���ĸ��豸������Ҫ��װ�ĸ���Ӧ�̵�Runtime��
�����ȷ��װ���Ծ��޷��ҵ��豸���μ����ġ�OpenCL���г�ĳ���豸����

### Apple:
��Mac OS X v10.6֮���ṩ����ϵͳ�����֧�֡��μ���
[About OpenCL for OS X](https://developer.apple.com/library/mac/documentation/Performance/Conceptual/OpenCL_MacProgGuide/Introduction/Introduction.html)

### Intel:
��Ҫ���ⰲװ[OpenCL��������](https://software.intel.com/en-us/articles/opencl-drivers)��
��Xeon Phi��ʹ��OpenCL����Ҫ��װ[MPSS](https://software.intel.com/en-us/articles/intel-manycore-platform-software-stack-mpss)��

### AMD:
������Catalyst�����С�

### nVidia:
�������Կ������С�

OpenCL SDK���������谲װ��
====
�Կ����߶��ԣ�SDKֻ��Ҫ��װһ�����ɡ����鰴�����ó����Ե�ƽ̨����װ��ֻҪ�����ĳ����в�ʹ�ù�Ӧ���ṩ����չ���ܣ��κ�SDK���ӱ���ĳ��򶼿�����ȫ��֧��OpenCL��ƽ̨������������

### Apple:
Mac OS X v10.7֮����XCode�п���ֱ��ʹ��OpenCL��

### Intel:
[Intel SDK for OpenCL Applications](https://software.intel.com/en-us/vcsource/tools/opencl-sdk)

### AMD:
[AMD APP SDK](http://developer.amd.com/tools-and-sdks/opencl-zone/opencl-tools-sdks/amd-accelerated-parallel-processing-app-sdk/)

### nVidia:
[nVidia CUDA Toolkit](https://developer.nvidia.com/cuda-downloads)

OpenCL���г�ĳ���豸
====
OpenCL API�����õĶ�̬���ӿ⣨opencl.dll������һ��OpenCLʵ�֣���������Khronos Group����Ϊ��ICD����ICD���ϵͳ�����ж�ȡ��Ӧ����Ϣ��Ȼ�����ε��ù�Ӧ�����ṩ��OpenCLʵ�֣���ʹ��ͬ��Ӧ�̵Ĳ�ͬʵ���ܹ���һ̨�����й��档

### Windows��
ICD����ͼ��ȡע�����`HKEY_LOCAL_MACHINE\SOFTWARE\Khronos\OpenCL\Vendors`�е�����`DWORD`ֵΪ0��ֵ����Ȼ��ͨ��`LoadLibraryA`ϵͳ�������μ������ǡ����磺

```HKEY_LOCAL_MACHINE\SOFTWARE\Khronos\OpenCL\Vendors:
```        nvopencl.dll                     dword:00000000
```        C:\path\to\intel\intelocl32.dll  dword:00000000

ICD�����μ���nvopencl.dll��intelocl32.dll��Ȼ��������ǵ���������������
```        clIcdGetPlatformIDsKHR
```        clGetPlatformInfo
```        clGetExtensionFunctionAddress
�����һ��������ʧ�ܣ�����Ϊ������һ��OpenCLʵ�֣���������

Linux:
ICD����ͼ��ȡ`/etc/OpenCL/vendors/<��Ӧ��>.icd`�����Ҵ��ж�ȡ`.so`�ļ�����
�ɹ����غ󣬵��õ�����������Windows��ͬ��

�����ȷ��װĳ�豸��Runtime֮��OpenCL�Ծ�δ�г����豸���������Ҫ���ICD���ã���Ҫʱ���б����ֶ���ӹ�Ӧ��ʵ�֡�

����ICD�ĸ���ϸ�ڲ��ģ�
[cl_khr_icd](http://www.khronos.org/registry/cl/extensions/khr/cl_khr_icd.txt)