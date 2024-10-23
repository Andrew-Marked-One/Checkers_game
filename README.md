### Project #6
Timeline: 2024.2.23 - 2024.7.8

This is a checkers game with a bot that uses the MiniMax algorithm with alpha-beta pruning to choose the best move, considering how far it has searched. It's the biggest project I've undertaken yet. Here are some things, but not all, that I've learned/improved during this project:
- Multithreading
- Templates
- Concepts
- Move semantics
- Ranges
- Lambdas
- Data structures
- Standard algorithms and containers
- Debugging
- Best practices

The more I learned, the more refactoring I needed to do. Since the first prototype to this version, I improved the speed by more than 50 times and, believe it or not, improved readability. The fact that I was reinventing the wheel meant the design ended up more complex than it should be. There's much more that can be done to improve the speed, for example: hashing game states, getting rid of recursion and implementing while loops with stacks, redrawing elements on the window only when needed, and rendering in a separate thread to make it smoother. But I think 10 moves is good enough, and I better move on to another project because I've already spent too much time on refactoring.

### Controls
- Mouse Left: Select cell
- Escape: Quit

### Used DLLs
- sfml-system-2.dll
- sfml-window-2.dll
- sfml-graphics-2.dll
  
### Learning Resources
- [Intro to Artificial Intelligence](https://www.youtube.com/watch?v=AzUZiUz-Wpc&list=PL_xRyXins84-dTmpL68AKv7UFAEvIeIr1&index=1) by [Dave Churchill](https://www.youtube.com/c/DaveChurchill), Associate Professor of Computer Science at Memorial University in Newfoundland, Canada.
- [Tutorials for SFML 2.6](https://www.sfml-dev.org/tutorials/2.6/), by [SFML](https://www.sfml-dev.org/index.php), Multimedia Library.
- [Data Structures](https://www.youtube.com/watch?v=B31LgI4Y4DQ), by [freeCodeCamp.org](https://www.youtube.com/@freecodecamp), YouTube educator.
- [Back to Basics](https://www.youtube.com/watch?v=IK4GhjmSC6w&list=PLb4X-oQu9_OGEZaQF4fzbEoREgzXY-DJd), by [CppCon](https://www.youtube.com/@CppCon), YouTube educator.

### Demo
[Watch demo.](https://www.youtube.com/watch?v=HgmxzhawZ4w)
