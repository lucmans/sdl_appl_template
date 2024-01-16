#pragma once


namespace Quit {

// set and get quit status
bool poll_quit();
void set_quit();
void reset_quit();

// call at application start to interpret SIGINT and SIGTERM as quit condition
void set_signal_handlers();

}  // namespace Quit
