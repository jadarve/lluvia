# LLUVIA

# Dependencies

* Googletest

```
sudo zypper install googletest-devel
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
            "-I<VULKAN_PATH>"
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
