# Orion
[![C++](https://img.shields.io/badge/language-C%2B%2B-%23f34b7d.svg?style=plastic)](https://en.wikipedia.org/wiki/C%2B%2B) 
[![CS:GO](https://img.shields.io/badge/game-CS%3AGO-yellow.svg?style=plastic)](https://store.steampowered.com/app/730/CounterStrike_Global_Offensive/) 
![Windows](https://github.com/danielkrupinski/Osiris/workflows/Windows/badge.svg?branch=master&event=push)

Free open-source ManualMap loader for **Counter-Strike: Global Offensive** game.

## Features

- [x] **ManualMap Injection**
- [x] **Load remote Dynamic-link library (DLL)**
- [x] **Load local Dynamic-link library (DLL)**
- [x] **VAC Bypass method**
- [x] **Osiris injected by default**

## Build

1. Clone repository: `git clone https://github.com/Tris0n/csgo-loader`
2. Enter the directory: `cd csgo-loader`
3. Open `Orion.sln` in Microsoft Visual Studio 2022.
4. Change build configuration to `Release | x86` and press **Build solution**.

## FAQ
### How to use the injector

Launch `Orion.exe` with administrator permissions.

- Option 1 - Select custom cheat
- Option 2 - Select VAC Inhibitor (Use if VAC inhibitor is not working)
- Option 3 - Auto-inject cheat and VAC Inhibitor (If you haven't selected any of the previous options, the loader will inject the Osiris cheat)

When injecting, the loader will close csgo and your steam and open it again using VAC Inhibitor and inject the cheat automatically.

### Recommendations

* If you use the option to load the DLL remotely from a URL, it is recommended that you use Discord to host it
* Do not keep changing the VAC Inhibitor DLL, because if you load a DLL other than the VAC Inhibitor the VAC modules will not be disabled, and this can cause a permanent ban.

## Screenshot
![Orion loader](https://github.com/Tris0n/csgo-loader/assets/93105314/8c952b87-2c46-4554-be38-53f46fa0ac98)

## Acknowledgments
*   [Zer0Mem0ry](https://github.com/Zer0Mem0ry) - by [ManualMap injector](https://github.com/Zer0Mem0ry/ManualMap) and great tutorials on reverse enginnering and game hacking.
*   [zyhp](https://github.com/zyhp) - by the great VAC Bypass method used in [vac3_inhibitor](https://github.com/zyhp/vac3_inhibitor).
