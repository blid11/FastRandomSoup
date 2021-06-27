# FastRandomSoup

This program is an attempt to recreate the program outlined by Carter Bays in his paper "The Discovery of a New Glider for the Game of Three-Dimensional Life". In the original program, a 7x7x7 centre area is used to generate "random soups" with a 20% density of cells. The centre area had originally been placed in the centre of a 23x23x23 grid. Cellular automaton simulations were ran using the 3D-life rule 5766, and any objects that hit the border of the simulation were placed back at the centre to determine if they were truly stable gliders. In 1989, Bays was capable of running 4 million of these simulations per day on a DEC 3100 RISC workstation. Bays found that approximately 1 in 4 million of these experiments produced a glider with a period of 8 and between 16 and 22 cells, depending on its phase. This glider was rarer than the common glider, which occurs at a rate of 1 in every 2100 experiments. 

Although Bays wrote another paper in which he describes the discovery of several additional 3D-life rules through the use of a modern CPU, there are no indications as to whether or not he carried out further searches within the rule 5766, which he considered to be possibly the richest 3D-life rule. I was curious to see if any additional gliders could be found within this rule, and so I wrote the following program in C++. The program generates random soups in a 9x9x9 centre area, which is placed at the centre of a 32x32x32 environment. 32x32x32 is 32768, so I encoded the address of each cell as a short. I used a lookup table to quickly search the precalculated 3D Moore neighbourhood addresses of each cell. I loop over the array, and if the value of a cell is 0 (it is dead), information about its neighbours is not used to update the next generation (it is skipped). I noticed a large speedup when using the AMD Optimizing C/C++ Compiler, which has a compiler flag "-reduce-array-computations=2". If I did implement this correctly, then the compiler is skipping over all the dead cells encoded by the array. Regardless, the program does run approximately 2 billion simulations per day when running on 12 virtual machines on an AMD 3900X CPU. 

An object is deemed "new" if its signature does not match the signatures of the two known gliders discovered by Bays. In my effort to pursue new projects, I have not corrected the FindSignature class, which has a different signature format than that used by Bays (my most recent projects Six_Action_Env and Collider use the correct signature notation). The FindSignature class does successfully classify the gliders as the most common (GliderV) or the second most common (GliderE). The total number of GliderV, GliderE and new objects is tallied and presented as output after the program has run. 

The coordinates for any new objects generated by the simulations were placed in a text file "newObject.txt". Possible new objects were placed at the centre of a larger grid (75x75x75) using the class DetectFalsePositive where the precalculated neighbours were stored by the class LrgLookup. 

When I wrote the following program I was not aware that symmetric random soups could increase the frequency of the appearance of gliders, and it would be a simple addition to this program. 

Below is an image of program compilation and use via the Windows Subsystem for Linux in Visual Studio Code. The program should be compatible with most C++ standards and compilers, however it runs fastest by installing the AMD Optimizing C/C++ compiler in Linux/Ubuntu, and using the following flags for compulation: 
clang++ -O3 -march=znver2 -flto -mllvm -reduce-array-computations=2 Fixed.cpp -o Fixed
One can then run the program using ./Fixed, where it will ask for the number of simulations to run. 

![image](https://user-images.githubusercontent.com/82735611/123555672-25b8eb80-d755-11eb-88a0-50bc4d90a88f.png)
