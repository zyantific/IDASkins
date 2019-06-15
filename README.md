IDA Skins
=========

Plugin providing advanced skinning support for IDA Pro utilizing [Qt
stylesheets](http://qt-project.org/doc/qt-4.7/stylesheet.html), similar to CSS.

## Important: This plugin is now deprecated
IDA 7.3 [added theming support](https://www.hex-rays.com/products/ida/7.3/index.shtml) directly into IDA itself and ships with a forked version of the `ISASkins dark` theme.
We will keep maintaining this plugin for a while for users of old IDA verions, but it will be shelved eventually. 
New themes should be developed for IDA directly.

## Screenshot

#### `VSCode dark` theme
![Screenshot 1](https://raw.githubusercontent.com/athre0z/ida-skins/master/media/screenshots/vscodetheme1.png)
![Screenshot 2](https://raw.githubusercontent.com/athre0z/ida-skins/master/media/screenshots/vscodetheme2.png)

#### `ISASkins dark` theme
![Screenshot](https://raw.githubusercontent.com/athre0z/ida-skins/master/media/screenshots/screencap1.png)

## Download
[Download the latest version from GitHub](https://github.com/athre0z/ida-skins/releases/latest)

## Installation
Copy the contents of the `plugins` directory into the `plugins` directory of
your IDA installation.

## Theming
Theming IDA using IDASkins works using [Qt
stylesheets](http://qt-project.org/doc/qt-4.8/stylesheet.html). For information
on the most important IDA-specific UI elements, take a look in the enclosed
default `stylesheet.qss`. **Pull-requests for new themes are very welcome!**
