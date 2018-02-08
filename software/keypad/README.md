# keypad
The reusable yet simple-to-use keypad library for your bare metal uP project!

# Author's notes
A generic and lightweight keypad library which allows debouncing and management of simple events.
It is so generic that it has no dependencies and can be used with a variety of input sources like physical buttons, soft-buttons, touch-buttons and everything else that can be digitalized in a boolean value.

# Highlights
- Lightweight: no scientific data yet but works pretty well.
- Suitable for physical GPIO buttons, soft-buttons, touch-buttons
- Simple and generic API: timer flag in and boolean in -> event-type out.
- Cross-Platform due to standard C and careful coding

# Usage

- Clone the repository and/or add it to your project as submodule, as required.
- Define the available buttons, depending on application requirements and design. The keypad_buttons_template.h contains the template. The personalized file shall be named keypad_buttons.h and reside somewhere where the compiler is able to find it.
- Commit your button definition in your project.

# MISRA
The code should (almost) follow MISRA rules with some exeptions. Please be aware that I did NOT run an analyzer tool yet, hence there is no guarantee the code actually is. The fact is the code has been compiled without warnings nor strange behavior on a 64-bit Linux machine

# Dependencies

None in particular. GNU-GCC compiler tested (AVR and AMD64).
