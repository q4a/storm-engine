#include <cstdarg>
#include <cstdint>
#include <cstdlib>
#include <ostream>
#include <new>

extern "C" {

wchar_t *get_stash_path();

wchar_t *get_logs_path();

wchar_t *get_save_data_path();

wchar_t *get_screenshots_path();

} // extern "C"
