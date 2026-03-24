# .fdp file format specification

  

## More about the format:

<span style="color: red;">__it's little endian hehehehe__</span>

  

## <span style="color: blue;">The header:</span>

| Type | Description |
| ---- | ----------- |
| str  | Must be "FDProj". If it's not this, isn't an .fdp file |
| int  | Version major |
| int  | Version minor |
| unsigned long  | Length of animation in frames |
| int  | Animation width |
| int  | Animation height |
| int  | Animation FPS |
| str  | Animation display name, terminating with byte 0 |
| str  | Animation description, terminating with byte 0 |
| str  | Animation author, terminating with byte 0 |
| [col](#Color) | BG color |
| int | Palette length |
  

## <span style="color: cyan;">Palette</span>

For each color in the palette, this is the structure:

| Type | Description |
| ---- | ----------- |
| unsigned long | color index |
| [col](#Color) | color |
  

## The actual format

The .fdp file format uses this structure per frame to describe frames:

| Type | Description |
|-------|------------|
| [colorarr](#color-arrays) | Colors (each pixel) |

  

## Color Arrays

An array variant which is used to describe palette colors:

It's just continuous unsigned longs which are palette color indices.

Also the origin for colors in frames is the top-left corner.

## Color

| Type | Description |
| ---- | ----------- |
| unsigned char | r |
| unsigned char | g |
| unsigned char | b |

  

# <span style="color: red;">The END</span>

| Type | Description |
| ---- | ----------- |
| str  | A constant of EOP (end of project)

