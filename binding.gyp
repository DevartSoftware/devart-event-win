{
    "targets": [
        {
            "target_name": "win-event",
            "conditions": [
                [
                    "OS=='win'", {
                        "sources": ["./src/winEvent.cpp"],
                        "include_dirs": ["<!@(node -p \"require('node-addon-api').include\")"],
                        "libraries": [],
                        "dependencies": ["<!(node -p \"require('node-addon-api').gyp\")"],
                        "defines": ['NAPI_DISABLE_CPP_EXCEPTIONS']
                    }
                ]
            ]
        }
    ]
}
