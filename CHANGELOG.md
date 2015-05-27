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
