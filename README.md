# Usb Script
This Parser/Scripting Language is distributed under LGPL2.1 and you are free to use this with your own project with a compatible License.

These are the currently implemented commands:
```c
// hold key 0 and 1 and release 0
hold 0,1;
release 0;

// press key 0,1,2,3,4,5
press 0,1,2,3,4,5; 

// move mouse pointer to x:100 y:101
move_abs 100,101;

// move mouse pointer +10 in x direction and +10 in y direction
move_rel 10,10;

// hold right mouse button
mouse_hold mouse_right;

// release right mouse button
mouse_release mouse_right;

// press all mouse buttons
mouse_press mouse_left,mouse_right,mouse_middle;

// Wait 1000 milliseconds
delay 1000;
```
