# TUI File Chooser

A simple TUI application for browsing and selecting files.

# Usage

This application can be used to both browse files and to open them using command substitution. The selected file will be outputted to stdout when you press SPACE.

## Command

The command takes two positional arguments:
  - Name: the initial label to show at the top of the file chooser (defaults to "Pick file")
  - Current directory: the directory to begin in (defaults to current directory)

### Examples

<br>

View files:
```sh
tfc "Pick a file" ~
```
    
<br>

Open files:
```sh
vi "$(tfc "File to edit")" # Defaults to current directory
```

<br>

## Controls

| Key | Action |
| --- | ------ |
| Escape | Exit text input, Exit file chooser |
| Backspace | Exit text input (if empty), Up directory |
| Enter | Enter text input, Open directory |
| Left | Up directory |
| Right | Open directory |
| Up | Up in directory |
| Down | Down in directory |
| '?' | Start search (if not inputting) |
| '=' | Create new file (if not inputting) |
| '+' | Create new directory (if not inputting) |
| Space | Exit and output current file (if not inputting) |
