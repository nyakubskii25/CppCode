<h1 align="center">CppCode</h1>

<p align="center">
  <img src="https://img.shields.io/github/contributors/nyakubskii25/CppCode?style=for-the-badge" alt="Contributors">
  <img src="https://img.shields.io/github/repo-size/nyakubskii25/CppCode?style=for-the-badge" alt="Repo size">
  <img src="https://img.shields.io/github/last-commit/nyakubskii25/CppCode?style=for-the-badge" alt="Last commit">
</p>

Hello, we are CppCode!

We are a team of four school students who like to learn by building things. Our project is a console game written in C++: a maze with treasures. The player looks for the exit and collects coins, keys, pickaxes and magic artifacts on the way. Doors open when you walk into them, locked doors need a key, and in the dark levels you see only the cells that are close to you.

The game has a campaign with 5 levels and 3 bonus levels, a free play mode and a table with the best scores. The code is split into many modules and uses dynamic memory for the maze, the inventory, the message log and the scores.

## 🚀 Languages and Libraries

<img src="https://skillicons.dev/icons?i=cpp" alt="C++" height="48">

C++ with the standard library. The Windows console is used for the keyboard (`conio.h`) and for colors.

## 🛠️ Used Tools

<img src="https://skillicons.dev/icons?i=visualstudio,git,github" alt="Visual Studio, Git, GitHub" height="48">
<img src="https://img.shields.io/badge/PowerPoint-B7472A?style=for-the-badge&logo=microsoftpowerpoint&logoColor=white" alt="PowerPoint" height="38">

## 📄 Documents

- [Documentation]([Documentation/CppCode_Documentation.docx](https://github.com/nyakubskii25/CppCode/blob/main/CppCode_Documentation.docx))
- [Presentation](https://github.com/nyakubskii25/CppCode/blob/main/CppCode.pptx) <img src="https://img.shields.io/badge/-PPTX-B7472A?logo=microsoftpowerpoint&logoColor=white" alt="PowerPoint">

## 🧑‍💻 Team Members

| Name | Role | Class | Github |
|------|------|-------|--------|
| Nikita Yakubskii | Scrum Trainer | 🟨 XA | [nyakubskii25](https://github.com/nyakubskii25) |
| Ivan Kolozenko | BackEnd Developer | 🟨 XA | [ISKolozenko24](https://github.com/ISKolozenko24) |
| Vladislav Shapovalov | FrontEnd Developer | 🟦 XG | [VShapovalov24](https://github.com/VShapovalov24) |
| Oleksandr Bondarenko | QA Engineer | 🟨 XA | [OVB2442](https://github.com/OVB2442) |

## 🔑 Access

The game works **only on Windows** and needs Visual Studio with the "Desktop development with C++" workload.

Open cmd and clone our repo by typing:

```
git clone https://github.com/nyakubskii25/CppCode.git
```

Then open the folder `maze project`, double-click `maze project.sln`, choose `Debug` and `x64`, and press `Ctrl + F5` to build and run the game.

## 🎮 How to play

| Key | Action |
|-----|--------|
| Arrow keys or W A S D | Move |
| B and a direction | Break a wall with a pickaxe |
| I | Inventory |
| C | Compass hint (needs the Golden Compass) |
| M | Show the whole level (needs the Ancient Map) |
| H | Help |
| Q | Leave the level |

| Symbol | Meaning |
|--------|---------|
| `@` | You |
| `o` | Coin: points |
| `k` | Key: opens a locked door |
| `p` | Pickaxe: breaks a wall |
| `*` | Artifact: a magic power and 100 points |
| `+` | Door: opens when you walk into it |
| `L` | Locked door: needs a key |
| `E` | The exit |

Artifacts: **Crystal Lens** (see further in the dark), **Ancient Map** (shows the whole level), **Golden Compass** (points to the exit), **Lucky Ring** (coins give double points).

## 🗂️ Project structure

```
CppCode/
├── maze project/
│   ├── main.cpp          – the main menu and the game loop
│   ├── types.h           – constants, enums and structures
│   ├── grid.h            – dynamic two-dimensional arrays (templates)
│   ├── platform.h/.cpp   – keyboard, colors, clearing the screen
│   ├── maze.h/.cpp       – the maze, moving on the grid, BFS
│   ├── generator.h/.cpp  – levels and the maze generator
│   ├── inventory.h/.cpp  – the inventory (a growing dynamic array)
│   ├── artifacts.h/.cpp  – the artifacts and their powers
│   ├── messagelog.h/.cpp – the message log (a linked list)
│   ├── scores.h/.cpp     – the table of the best scores
│   ├── game.h/.cpp       – the rules of the game
│   ├── ui.h/.cpp         – everything the player sees
│   └── maze project.sln  – Visual Studio solution (and project files)
├── Documentation/
├── .gitignore
└── README.md
```

## 📚 More

The full description of the game, the levels, the algorithms and the dynamic memory is in the [documentation]([Documentation/CppCode_Documentation.docx](https://github.com/nyakubskii25/CppCode/blob/main/CppCode_Documentation.docx)).
