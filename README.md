# Interactive Iterated Function Systems
This program draws Iterated Function System fractals interactively. It is largely based on the documentation provided by Michael Barnsley in his 1988 book Fractals Everywhere.

You will find ready-to-run binaries in the folder bin/release. Make sure to get the .dll's for the program to run. Or look for the .zip file in the Downloads section on github. It may be somewhat out of date, but it has the whole package and will generally be stable.

## USAGE:
Left-click to control transformation rectangles. You will need to add one to begin with, so try clicking in the lower-left, upper-left, and lower-right areas of the window.
Right-click to add another transformation.

- 'd' deletes the transformation currently highlighted under the cursor.
- Spacebar renders the fractal at a higher resolution, without the transformation rectangles.
- 's' saves the current set of transformations in the file "boxes.txt" - do not modify this file if you want to open it again! It also saves the transformation numbers to the file "transformations.txt"
- 'o' loads "boxes.txt" in the same directory to recover a previous fractal.
Use your mouse to edit existing transformations. Click-drag to move the various parts of each transformation box around.

## CHANGELOG:
2012-04-29: Added primitive save and open functionality

## COMPILING:
Open up "Fractal Class.cbp" in Code::Blocks and compile. Everything should be set up in the project file. This project uses SDL and the SDL_gfx libraries. There may be a couple Windows dependencies. Everything else should be contained in any standard C++ compiler.
