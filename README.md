# Low Level Unified VIsion Algorithms (LLUVIA)

# Dependencies

* [Vulkan LunarG framework 1.0.65.0 installed](https://vulkan.lunarg.com/). The `VULKAN_SDK` environment variable should be set to point to the SDK location. Example:
```
    export VULKAN_SDK=~/vulkan/VulkanSDK/1.0.65.0/x86_64
```
* CMake 3.5 or above

# Build

```
git clone https://github.com/jadarve/lluvia.git
mkdir lluvia
cd lluvia
cmake ..
make
make test
```

For debugging failed tests, you can enable the [standard validation layers](https://vulkan.lunarg.com/doc/view/1.0.65.0/windows/layers.html) provided by the LunarG SDK.

```
export VK_INSTANCE_LAYERS=VK_LAYER_LUNARG_standard_validation
```

# Generate Eclipse CDT project

```
mkdir lluvia_eclipse
cd lluvia_eclipse
cmake -G"Eclipse CDT4 - Unix Makefiles" -D CMAKE_BUILD_TYPE=Debug ../lluvia
```

# Create SublimeText project

## Requirements

* Easy CLang Complete package

Create a `lluvia.sublime-project` file and copy-paste the following code

```
{
    "folders" : [
        {
            "path" : ".",
            "folder_exclude_patterns" : [
                "build"
            ]
        }
        
    ],
    "settings" : {
        "ecc_common_flags": [
            "-I$project_base_path/core/include/",
            "-I<VULKAN_SDK>/include"
        ],
        "easy_clang_complete_verbose": true,
        "show_type_info": false,
        "flags_sources": [
            {
               "file": "CMakeLists.txt",
               "search_in": "."
            },
        ],
    },
    "build_systems" : [

    ]
}
```
