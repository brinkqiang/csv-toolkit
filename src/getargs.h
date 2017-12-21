using std::deque;
using std::string;

namespace shuffle {
    // int getargs(int argc, char* argv[], vector<string>& args, vector<string>& flags);
    int getargs(int argc, char* argv[], deque<string>&args, deque<string>&flags) {
        /**
        * Parse command line arguments
        * Return 1 if there is a parsing error
        **/

        bool quote_escape = false;
        std::string this_arg;

        // Skip first argument --> it's the program name
        for (int i = 1; i < argc; i++) {
            this_arg = std::string(argv[i]);

            if (quote_escape) {
                // Quote Escape Handling
                if (this_arg.back() == '"') {
                    quote_escape = false;
                    this_arg = this_arg.substr(0, this_arg.size() - 1);
                }
                else {
                    if (i + 1 == argc) {
                        // End of arguments but no end of quote escape in sight
                        return 1;
                    }
                }

                // Append current string to previous argument string
                args.back() = args.back() + " " + this_arg;
            }
            else if (this_arg.front() == '-') {
                size_t begin = 1;
                if (this_arg[1] == '-')
                    begin++;

                // Option flag
                flags.push_back(this_arg.substr(begin, this_arg.size()));
            }
            else {
                if (this_arg[0] == '"') {
                    quote_escape = true;
                    this_arg = this_arg.substr(1); // Trim off quote
                }

                args.push_back(this_arg);
            }
        }

        return 0;
    }
}