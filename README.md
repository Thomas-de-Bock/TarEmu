 # Tar
 An Atari2600 Emulator written in C++
 
 # Usage
 ## Normal Use
 Tar.exe can be run from the command-line with the ROM path as argument.
 Other switches and options:
 - `rom` followed by the ROM path.
 - `noCrop`, displays the usually cropped out virtual VBLANK, HBLANK and VSYNC "sections".
 - `dump`, dumps the memory and CPU state after every instruction. Used to compare states for debugging purposes.
 
 Example use: `./Tar.exe -dump -rom C:/path/to/rom -noCrop`
 ## Debug use
 The Tar emulator implements Tom Harte's 6502 Json instruction test (https://github.com/TomHarte/ProcessorTests/tree/main/6502), which can be accessed with the `--jsonTest` flag.
 Other switches and options:
 - `ignoreStatus`, does not check for mismatches in the processor status register.
 - `ignoreCycles`, does not check for mismatches in the instruction cycle times.
 - `ignoreRegisters`, does not check for mismatches in the processor registers.
 - `ignoreMemory`, does not check for mismatches in memory.
 - `noBCD`, skips all ADC and SBC tests where decimal mode is enabled.
Example use: `./Tar.exe --jsonTest 0xFD 500 -ignoreCycles -ignoreMemory`
 
 # Building
 Tar can be built in Visual Studio 2019 or later. It is recomended to build in Release mode no matter what as the SFML debug dll's are not included anyway.
 After building, the `tables` and `noises` folders have to be moved into the build folder, along with the SFML dll's. 
 The `tests` folder can also be put in the same directory if you plan on using the CPU instruction tester.
