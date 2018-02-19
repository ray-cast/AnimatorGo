# About Testing

To disable compilation of test-related codes, invoke CMake with variable `OCTOON_NO_TEST` set `true`.

For DLL searching, test binaries are generated in `/path/to/octoon/lib/binaries`. However, test environment is copied to the CMake binary directory (where you call CMake to generate project). You should stay in binary directory to run tests properly.

## Write Tests

Test environment is prepared for integration test where system calls are required. The environment should be involved as seldom as possible. In case you use it, every thing in the environment should be kept untouched so that the tests can be run properly at a second time.
