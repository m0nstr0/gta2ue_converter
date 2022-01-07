# gta2ue_converter
GTA 3 &amp; GTA VC DFF files converter for Unreal Engine

## usage

```
  gta2ue_converter [-h|--help] [-d|--dff <dff file> -o|--output <output file>]

  -h, --help        print usage
  -d, --dff arg     input *.dff file
  -o, --output arg  output *.dffjson file
```

this application converts ```*.dff``` to ```*.json``` format.

the plugin for UE5 is under development and will be uploaded on GitHub alongside other tools ASAP.

## build

### windows 
use premake5 to generate vs2022 project

```
premake5.exe vs2022
```

Premake5 will place the solution file in the ```build``` directory and the executable file in the ```bin``` directory