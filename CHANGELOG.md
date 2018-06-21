Changelog
=========

## 1.0.0 -> 1.1.0:
  - improved overall UI look
  - fixed problems with bad contrasts
  - fixed scrollbar issues
  - moved style config into extra directory (IDADIR/skin)
  - added path constants `<IDADIR>` and `<SKINDIR>` for stylesheets

### 1.1.0 -> 1.2.0:
  - altered skin file structure to allow switching between several skins
  - added skin selection dialog
  - improved default skin

### 1.2.0 -> 1.3.0:
  - plugin now loads immediately when IDA is started
  - fonts in the stylesheet are now selected according to the font configured
    in IDA, thereby fixing font rendering issues when selecting fonts other
    than Consolas without adjusting the stylesheet
  - added skin "IDA default"
  - minor dark-theme fixes
  - switched to cmake build system

### 1.3.0 -> 1.4.0:
  - added object-inspector for easy finding of widget names and types for
    theme development
  - fixed infinite-loop of message boxes that could occur on first start
  - fixed issue with IDA installations with unaltered font-settings

### 1.4.0 -> 1.4.1:
  - added support for IDA versions 6.9 and higher (Qt5)

### 1.4.1 -> 2.0.0:
  - rewrote the entire plugin in Python
    - ditches the madness of cross platform building native IDA plugins
  - the theme selector now displays the path of the color theme
    recommended to use with the IDASkins theme (contributed by @tostercx)
  - updated the "IDASkins dark" CLR file to skin the output window correctly
  - use IDA's registry API instead of QSettings
  - dropped support for any IDA version older than 7.0
  - various minor bug-fixes