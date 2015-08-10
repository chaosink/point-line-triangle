# point-line-triangle
A demo about points, lines, triangles and mouse cursor.

## Compiling
```Shell
make
```

## Usage
```
Usage   : point-line [OPTION] WAV_file

Options : -a        draw focus automaticly and ignore the mouse cursor
          -b        don't draw lines between nearby points
          -c        draw power circle around focus
          -d        remove window decoration
          -f        enable fullscreen
          -h        show this help message and exit
          -i        set input WAV file
          -m        don't draw lines between points and mouse cursor
          -n NUM    set point number
          -p        print time information in terminal
          -s SPEED  set particle speed
          -t        draw triangles around focus
          -v        don't draw vertexes(points)
          -w SIZE   set window size(for example, 1920x1080)

```

Without any options, it will run in resolution 1024x768(not fullscreen) with window decoration, drawing 200 points and lines between nearby points as well as lines between points and mouse cursor, not printing time information, and particle speed is 1.0.

## Dependencies
[OpenGL - The Industry's Foundation for High Performance Graphics](https://www.opengl.org/)

[GLFW - An OpenGL library](http://www.glfw.org/)

[GLEW - The OpenGL Extension Wrangler Library](http://glew.sourceforge.net/)

[GLM - OpenGL Mathematics](http://glm.g-truc.net/0.9.6/index.html)

[ALSA - Advanced Linux Sound Architecture](http://www.alsa-project.org/main/index.php/Main_Page)

## Examples & Screenshots
```Shell
./point-line -d -f -w 1920x1080 -n 200
```

![1.png](screenshots/1.png)

- - -

```Shell
./point-line -df -w1920x1080 -n1000
```

![2.png](screenshots/2.png)

- - -

```Shell
./point-line -dfw1920x1080 -n2000 -m
```

![3.png](screenshots/3.png)

- - -

```Shell
./point-line -dfw1920x1080 -n4000 -m
```

![4.png](screenshots/4.png)

- - -

```Shell
./point-line -d -f -w 1920x1080 -n 200 -b
```

![5.png](screenshots/5.png)

- - -

```Shell
./point-line -d -f -w 1920x1080 -n 4000 -b
```

![6.png](screenshots/6.png)

- - -

```Shell
./point-line -d -f -w 1920x1080 -n 4000 -b
```

![7.png](screenshots/7.png)

- - -

```Shell
./point-line -d -f -w 1920x1080 -n 100000 -b -m
```

![8.png](screenshots/8.png)
