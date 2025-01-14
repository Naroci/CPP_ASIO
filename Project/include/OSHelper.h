#pragma once
#include <iostream>
#include <string>

class OSHelper {
    public: 

        /**
        * An Enum specifying all Operating-Systems.
        */
        enum OperatingSystem {
            MacOS,
            Linux,
            Windows,
            Unspecified
        };

        
        /**
        * @return an Enum of OperatingSystem for the current OS.
        */
        static OperatingSystem GetCurrentOperatingSystem();

        /**
        * Returns the name of the current operating system.
        *
        * @return the operating system name as a string value.
        */
        static std::string GetCurrentOperatingSystemName();
};