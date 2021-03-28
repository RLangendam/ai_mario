# Find ROM offset for VBA

The screen for Mario1.gb starts at ROM offset 0x9800. Open the game and the memory viewer at the ROM section.
Copy the value from 0x9800 of cheat engine and search for it. For VBA 1.7.2 and 1.8.0 there will be 2 addresses of which the last one is correct.
Subtract 0x9800 from that address to get the absolute address of the start of ROM and use cheat engine again to look for the pointer having that address as value.
You should find a single pointer with a fixed offset in the VBA executable. This value can be used as the offset in the start_of_rom variable.