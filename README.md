IDA Skins
=========

Plugin providing advanced skinning support for the Qt version of IDA Pro utilizing [Qt stylesheets](http://qt-project.org/doc/qt-4.7/stylesheet.html), similar to CSS.

## Screenshot
![Screenshot 1](https://raw.githubusercontent.com/athre0z/ida-skins/master/media/screenshots/screencap1.png)

Screenshot above shows the enclosed `stylesheet.css` in combination with the [idaConsonance](https://github.com/eugeneching/ida-consonance) theme.

## Binary distribution
[Download latest binary version from github](https://github.com/athre0z/ida-skins/releases/latest)

## Compliation
While the Qt makefile supports unix-like operating systems in theory, compilation has not been tested on any platform other than windows. In order to successfully compile the plugin, three environment variables need to be set:

- `IDADIR` The path of your IDA installation
- `IDAQTDIR` The path of the Qt installation matching your IDA version
- `IDASDK` The path to your IDA SDK

On windows, you may want to use `make_vs_proj.py` to generate a Visual Studio project file for you.

## Installation
Place `IDASkins.plX` into the `plugins` directory of your IDA installation. The theme files (the `skin` directory) needs to be copied to the root of your IDA installation.

## Theming
Theming IDA using IDASkins works using [Qt stylesheets](http://qt-project.org/doc/qt-4.7/stylesheet.html). For information on the most important IDA-specific UI elements, take a look in the enclosed default `stylesheet.css`.
