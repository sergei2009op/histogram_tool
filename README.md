# Task

- Create a command line tool that can calculate a red, green and blue histogram for an image and write to an output file in the format described below.
- It should be multi-threaded and should ultilize all processing cores without using third-party libraries such as Intel TBB or OpenMP.

# Output format

- An ASCII text CSV file containing one line for each colour band in the image (in the order red, green, blue). Each line is a comma separated list of the pixel count for each value (256 per line).
