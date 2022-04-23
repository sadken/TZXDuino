## TZXDuino

Even though this repo is a fork of the hugely out of date github repo, it contains the latest source that I found, from https://www.facebook.com/Arduitape

For now, the master branch is identical to upstream Arduitape TZXDuino source - apart from one small difference - which is that I don't change the filenames for each new version.  I figured that GitHub tags + releases achieve that already in a better and more scalable way.

I also have my own separate enhancements which I am working on in the branch(es) that you can find in this same repo.  My aim is to synchronise my changes with the latest Arduitape, and combine into one place.

## MY BRANCHES (could be in flux)

### sdfat_and_seeeduino_xiao
This branch contains my changes to support the Seeeduino Xiao M0 device, which I am using for a work-in-progress TZXDuino-like device.  Among other things this branch also incorporates changes to support single-pin buttons via ADC, and upgrades TZXDuino to support the latest SdFat library (versions 2.x), along with enhancements off the back of that (better SD support, more subdirectories, less memory usage).  It also includes a few other unrelated enhancements, like an improved text scroll, reduced firmware+ram size in Menu code, and a slightly reorganized userconfig.h (partly to separate out the "device platform type" stuff from the "peripherals" stuff).

While I continue to work on the Seeeduino Xiao M0 device, I do plan to merge the other changes into master, as those are pretty much complete.

## Credits
Code and Testing: Andrew Beer & Duncan Edwards
