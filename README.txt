My program contain no file heirarchy, as I didn't think it necessary, considering there is only the single .cpp file, a single .h file and a single texture image.

The program should execute in all Linux Distro's, however, I'm unsure about whether or not it is able to run on Mac OS. The g++ compiler is obviously mandatory to compile the code, as well as OpenGl. The following command is required to compile the project: 
"g++ -o Project ConsoleApplication1.cpp -lX11 -lGL -lpthread -lpng -Wno-deprecated"
to execute the resulting 'Project' file you simply need to enter "vblank_mode=0 ./Project".

The controls and use can be fairly confusing, so the following should be noted:
3D SPACE:
Keys 'W' and 'S' - Control navigation forward and backward respectively
Keys 'A' and 'D' - Control the camera view.
Keys 'UP' and 'DOWN' - Control the cameras elevation up and down the y axis
Keys 'LEFT' and 'RIGHT' - Control navigation by strafing left and right respectively
Key 'E' - Whenever E is pressed, the user will enter the virtual terminal, onscreen guide is provided.

TERMINAL INTERACTION:
The terminal accepts only numerical and both UPPERcase and LOWERcase letter characters, and the ENTER key.
The virtual terminal accepts ONLY the following commands:

add X Y - where X and Y are Integers
	outputs the sum on screen

mul X Y - where X and Y are Integers
	outputs the product on screen

sub X Y - where X and Y are Integers
	outputs the difference on screen

div X Y - where X and Y are Integers
	outputs the quotient on screen

mod X Y - where X and Y are Integers
	outputs the remainder on screen

ls - lists the actual files within the directory the program has been launched out of.

exit
	exits the terminal and puts the camera back in 3D SPACE.
