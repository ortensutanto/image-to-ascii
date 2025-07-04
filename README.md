# Image_to_ascii
Simple C program that converts images into ASCII on the terminal

## Requirements
**To Build**
- C Compiler
## How to use
1. Clone the repo OR download the release executable
2. Run using `./image_to_ascii -f <filepath>`

**Available Parameters**
- `-f` filepath of the image to convert. If not given program will ask for path
- `-w` width of the text. Defaults to 80
- `-t` type of brightness. Examples can be seen below
- `-k` amount of clusters for color clustering. Defaults to 5. The lower the faster
- `-c` whether the ASCII text will be colored or not. 0 is colored, 1 is non colored. Defaults to 0

### Original Image
![original image](media/furina.jpg)
### Colored Type 1
![colored type 1](media/colored1.png)
### Colored Type 2
![colored type 2](media/colored2.png)
### Colored Type 3
![colored type 3](media/colored3.png)
### Monochrome Type 1
![monochrome type 1](media/nocolor1.png)
### Monochrome Type 2
![monochrome type 2](media/nocolor2.png)
### Monochrome Type 3
![monochrome type 3](media/nocolor3.png)
