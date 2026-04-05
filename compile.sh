#! /bin/bash

# sudo apt install libgtkmm-3.0-dev
g++ main.cpp -o lab3
g++ maingui.cpp -o lab3_gui $(pkg-config gtkmm-3.0 --cflags --libs)