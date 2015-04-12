#define TEMPEST_VERSION 0.1
//STL
#include <stdint.h>
#include <functional>
#include <string>
#include <vector>
#include <unordered_map>
#include <atomic>
#include <mutex>
#include <utility>
#include <algorithm>
#include <memory>
#include <sstream>

//cryptopp
#include <cryptopp/cryptlib.h>
#include <cryptopp/serpent.h>
#include <cryptopp/twofish.h>
#include <cryptopp/aes.h>
#include <cryptopp/osrng.h>
#include <cryptopp/zlib.h>
#include <cryptopp/secblock.h>
#include <cryptopp/pwdbased.h>
#include <cryptopp/modes.h>
#include <cryptopp/hex.h>
#include <cryptopp/sha.h>
#include <cryptopp/files.h>
#include <cryptopp/filters.h>

//miscellaneous
#include "pugixml.hpp"
#ifdef _WIN32
#include <Windows.h>
#endif