# Orion
[![C++](https://img.shields.io/badge/language-C%2B%2B-%23f34b7d.svg?style=plastic)](https://en.wikipedia.org/wiki/C%2B%2B) 
[![CS:GO](https://img.shields.io/badge/game-CS%3AGO-yellow.svg?style=plastic)](https://store.steampowered.com/app/730/CounterStrike_Global_Offensive/) 
![Windows](https://github.com/danielkrupinski/Osiris/workflows/Windows/badge.svg?branch=master&event=push)

Free open-source ManualMap loader for **Counter-Strike: Global Offensive** game. 

## Features

- [x] **ManualMap Injection**
- [x] **Load remote Dynamic-link library (DLL)**
- [x] **VAC Bypass method**
- [x] **Osiris injected by default**

## Build

1. Clone repository: `git clone https://github.com/Tris0n/csgo-loader`
2. Enter the directory: `cd csgo-loader`
3. Open `Orion.sln` in Microsoft Visual Studio 2022.
4. Change build configuration to `Release | x86` and press **Build solution**.

## FAQ
### How to use the injector

Open `Orion.exe` and select option 3 to inject Osiris using VAC Inhibitor, but if you want select another cheat, use option 1. If the VAC Inhibitor is not working, use option 2 to change it.

## Acknowledgments
*   [Zer0Mem0ry](https://github.com/Zer0Mem0ry) - by [ManualMap injector](https://github.com/Zer0Mem0ry/ManualMap) and great tutorials on reverse enginnering and game hacking
*   [zyhp](https://github.com/zyhp) - by the great VAC Bypass method used in [vac3_inhibitor](https://github.com/zyhp/vac3_inhibitor).