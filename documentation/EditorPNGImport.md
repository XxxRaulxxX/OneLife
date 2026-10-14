# PNG artwork importer: community testing

This branch adds direct PNG artwork importing to the 2HOL editor. Feedback
is welcome before a pull request is submitted to the official project.

## Build using the recommended scripts

Use the normal [miniOneLifeCompile instructions](https://github.com/risvh/miniOneLifeCompile#usage),
which are recommended in [2HOL's build documentation](https://github.com/twohoursonelife/OneLife#building).
For Windows, set up Ubuntu in WSL as described there. Run the commands below
in that Linux shell, from an empty parent folder on your Windows drive.

Clone this testing branch first so the standard repository setup keeps it:

```sh
git clone --branch editor-png-import https://github.com/XxxRaulxxX/OneLife.git OneLife
git clone https://github.com/risvh/miniOneLifeCompile.git miniOneLifeCompile
cd miniOneLifeCompile
./getDependencies.sh
./cloneRepos.sh
./applyFixesAndOverride.sh
```

Build the Windows editor through the unchanged recommended script:

```sh
./editor.sh 5
```

Leave `PLATFORM_OVERRIDE` empty or set to `5` for a Windows build, as
documented by miniOneLifeCompile. The script calls the project's configure
and makeEditor scripts. Its output is `../output/EditOneLife.exe`; run it
from the output folder. If switching from a previous client or another
platform build, use miniOneLifeCompile's documented cleanup step first.

The normal source folders are:

```text
parent/
  OneLife/
  minorGems/
  OneLifeData7/
  miniOneLifeCompile/
  output/
```

No changes to minorGems are required.

## What to test

1. Click **Load PNG File** and choose a PNG. Transparent areas should appear
   over white/gray squares; opaque white should stay white. Loading does
   not automatically select the artwork.
2. Black Threshold, Paper Threshold, their D buttons, and Solid should be
   disabled. Saturation and Shadow start at zero and remain adjustable.
3. Use **+** and **-** to zoom. The PNG view changes;
   the controls and saved artwork keep their original size. Zoom ranges
   from 6.25% to 1600%. Use Shift if your keyboard needs it to type +.
   Zoom shortcuts are ignored while typing in a text field.
4. Drag to select artwork. The selected sprite should contain no squares.
   Enter a Tag before saving through the normal editor controls.
5. The anchor stays fixed on screen when the sheet moves or zooms. Hold
   **C** and click/drag to move it. As in the original editor, the current
   selection keeps its captured anchor; a new selection uses the new point.
6. Click the **X** beside Load PNG File. It should clear the loaded artwork,
   hide the X, and restore the original import controls. It does not delete
   the source PNG.
7. Check cancel and invalid-file handling: the loaded PNG should remain.
8. Check the original Sprite, Lines, and Overlay import workflows. A
   successful original import restores their previous scan settings.

The pickers are also moved 20 units down, and Overlay Import is aligned
with Sprite Import. Sprite storage still uses the original TGA format,
transparent-border trimming, texture padding, and tag requirement.

Windows is the main testing target. Linux and macOS chooser implementations
are included but have not been fully verified. Existing texture overlays
combined with PNG zoom have not been exhaustively tested.

## Report feedback

Open an issue on this fork with your operating system, the branch commit,
steps to reproduce the problem, and what you expected versus what happened.
For compilation problems, include the failed command and relevant error
lines. For artwork problems, include a small example PNG if you can share it.
