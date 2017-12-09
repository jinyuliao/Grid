// You should place include statements to your module's private header files here.  You only need to
// add includes for headers that are used in most of your module's source files though.
#include "IGridRuntime.h"

#define LOG_INFO(FMT, ...) UE_LOG(GridRuntime, Display, (FMT), ##__VA_ARGS__)
#define LOG_WARNING(FMT, ...) UE_LOG(GridRuntime, Warning, (FMT), ##__VA_ARGS__)
#define LOG_ERROR(FMT, ...) UE_LOG(GridRuntime, Error, (FMT), ##__VA_ARGS__)
