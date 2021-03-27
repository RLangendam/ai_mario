#include "tensorflow.hpp"

#pragma warning(push)
#pragma warning(disable : 4190)
#include <tensorflow/c/c_api.h>
#pragma warning(pop)

std::string tensorflow::get_version() { return TF_Version(); }