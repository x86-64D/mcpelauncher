#pragma once

#include <string>

namespace msa {
namespace client {

namespace ErrorCodes {

    static constexpr int NoSuchAccount = -100;
    static constexpr int AccountAlreadyExists = -101;
    static constexpr int MustShowUI = -102;

    static constexpr int TokenAcquisitionServerError = -110;

    static constexpr int InternalError = -200;
    static constexpr int InternalUIStartError = -201;

    static constexpr int OperationCancelled = -501;

}

}
}
