#!/bin/bash

if [ ! -f "workspace.py" ]; then
    echo "error: can not find the file or directory"
    exit 1
fi

if ! command -v python3 &> /dev/null; then
    echo "error: can not find the python3"
    exit 1
fi

clear
python3 workspace.py
if [ $? -eq 0 ]; then
    echo "excute success!"
else
    echo "execute failed!"
    exit 1
fi
