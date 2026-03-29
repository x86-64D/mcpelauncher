#include <fake-jni/fake-jni.h>
#include "main_activity.h"
#include <log.h>
#include "lib_http_client.h"
#include <utility>
#include <jnivm/bytebuffer.h>
#include <curl/curl.h>

class HttpClientWebSocket : public FakeJni::JObject {
    FakeJni::JLong owner = 0;

public:
    DEFINE_CLASS_NAME("com/xbox/httpclient/HttpClientWebSocket")
    HttpClientWebSocket(FakeJni::JLong owner);
    ~HttpClientWebSocket();
    void connect(std::shared_ptr<FakeJni::JString>, std::shared_ptr<FakeJni::JString>);
    void addHeader(std::shared_ptr<FakeJni::JString>, std::shared_ptr<FakeJni::JString>);
    FakeJni::JBoolean sendMessage(std::shared_ptr<FakeJni::JString>);
    FakeJni::JBoolean sendBinaryMessage(std::shared_ptr<jnivm::ByteBuffer>);
    void disconnect(int id);

private:
    void* curl;
    std::mutex curlMu;
    void* jvm;
    bool connected = false;
    struct curl_slist* header = nullptr;
    void sendOpened();
    void sendClosed();
};
