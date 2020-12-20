#include "Configuration.hpp"
#include <iostream>

Configuration::Configuration(int argc, char** argv, const char* shortOptions, struct option* longOptions):
    title(NULL),
    description(NULL),
    icon("dialog-information"),
    options(),
    values(),
    timeoutMinutes(0),
    optIndex(0),
    mIsValid(false)
{
    int valIndex = 0;
    int index(1);

    while (1)
    {
        int c = getopt_long(argc, argv, shortOptions, longOptions, NULL);
        if (c == -1)
        {
            break;
        }

        switch (c)
        {
            case 'o':
                if (optIndex >= MAX_OPTIONS)
                {
                    std::cerr << "Too many options specified. Only 3 allowed." << std::endl;
                    break;
                }
                options[optIndex++] = optarg;
                index += 2;
                break;
            case 'v':
                if (valIndex >= MAX_OPTIONS)
                {
                    std::cerr << "Too many values specified. Only 3 allowed." << std::endl;
                    break;
                }
                values[valIndex++] = optarg;
                index += 2;
                break;
            case 't':
                timeoutMinutes = std::stoull(optarg);
                index += 2;
                break;
            case 'i':
                icon = optarg;
                index += 2;
                break;
            default:
                return;
        }
    }

    if (index + 2 == argc)
    {
        mIsValid = true;

        title = argv[index];
        description = argv[index + 1];
    }

    if (optIndex > valIndex)
    {
        std::cerr << "The number of options and values should be the same. Using the lesser." << std::endl;
        optIndex = valIndex;
    }
}
