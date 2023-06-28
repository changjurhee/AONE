/*
#include <iostream>
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/rand.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT "8080"

int main()
{
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();

    // TLS 클라이언트 초기화
    SSL_CTX* ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx)
    {
        std::cout << "TLS 클라이언트를 초기화하는 데 실패했습니다." << std::endl;
        return 1;
    }

    // TLS 연결 설정
    SSL* ssl = SSL_new(ctx);
    BIO* bio = BIO_new_ssl_connect(ctx);
    BIO_set_conn_hostname(bio, SERVER_IP ":" SERVER_PORT);
    BIO_get_ssl(bio, &ssl);
    SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);

    // TLS 연결 수립
    if (BIO_do_connect(bio) <= 0)
    {
        std::cout << "TLS 연결 수립에 실패했습니다." << std::endl;
        ERR_print_errors_fp(stderr);
        BIO_free_all(bio);
        return 1;
    }

    // TLS 인증서 검증
    if (SSL_get_verify_result(ssl) != X509_V_OK)
    {
        std::cout << "TLS 인증서 검증에 실패했습니다." << std::endl;
        BIO_free_all(bio);
        return 1;
    }

    // 메시지 전송
    std::string message = "안녕하세요, 서버!";
    if (BIO_write(bio, message.c_str(), message.length()) <= 0)
    {
        std::cout << "메시지 전송에 실패했습니다." << std::endl;
        ERR_print_errors_fp(stderr);
        BIO_free_all(bio);
        return 1;
    }

    // 응답 수신
    char buffer[2048];
    int bytesRead = BIO_read(bio, buffer, sizeof(buffer));
    std::string response(buffer, bytesRead);
    std::cout << "서버 응답: " << response << std::endl;

    // TLS 연결 종료
    BIO_free_all(bio);
    SSL_CTX_free(ctx);

    return 0;
}
*/
#define _WINSOCK_DEPRECATED_NO_WARNINGS

/*
#include <iostream>
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <winsock2.h>

// TLS 서버 주소 및 포트 번호
#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 8080


int main()
{
    // Winsock 초기화
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "Winsock 초기화 실패." << std::endl;
        return 1;
    }

    // OpenSSL 초기화
    SSL_library_init();
    SSL_CTX* ctx = SSL_CTX_new(TLS_client_method());

    // 서버에 연결할 소켓 생성
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cerr << "소켓 생성 실패." << std::endl;
        WSACleanup();
        return 1;
    }

    // 서버 주소 설정
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
    serverAddress.sin_port = htons(SERVER_PORT);

    // 서버에 연결
    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
    {
        std::cerr << "서버 연결 실패." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // OpenSSL SSL 구조체 생성
    SSL* ssl = SSL_new(ctx);
    SSL_set_fd(ssl, clientSocket);

    // TLS 연결 수립
    if (SSL_connect(ssl) <= 0)
    {
        std::cerr << "TLS 연결 수립 실패." << std::endl;
        ERR_print_errors_fp(stderr);
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // TLS 연결 성공
    std::cout << "TLS 연결이 수립되었습니다." << std::endl;

    // 데이터 송수신 및 처리
    // ...
    // 데이터 전송

    while(true) {
        //const char* message = "Hello, Server !";
        //int messageLength = strlen(message);

        std::string message;
        std::cout << "서버에 보낼 내용을 입력하세요: ";
        std::cin >> message;
        int messageLength = message.length();

        int bytesWritten = SSL_write(ssl, message.c_str(), messageLength);
        if (bytesWritten <= 0) {
            std::cerr << "데이터 전송 실패." << std::endl;
            ERR_print_errors_fp(stderr);
            // 오류 처리 로직 추가
        }

        // 데이터 수신
        const int bufferSize = 15;
        char buffer[bufferSize];

        int bytesRead = SSL_read(ssl, buffer, bufferSize);
        if (bytesRead <= 0) {
            std::cerr << "데이터 수신 실패." << std::endl;
            ERR_print_errors_fp(stderr);
            // 오류 처리 로직 추가
        }
        else {
            // 수신된 데이터 출력
            std::cout << "Server 로부터 수신된 데이터: " << buffer << std::endl;
            for (int i = 0; i < bytesRead; i++) {
                std::cout << buffer[i];
            }
            std::cout << std::endl;
        }

        Sleep(1000);
    }

    // 연결 종료
    SSL_shutdown(ssl);
    SSL_free(ssl);
    closesocket(clientSocket);

    // Winsock 정리
    WSACleanup();

    // OpenSSL 정리
    SSL_CTX_free(ctx);
    EVP_cleanup();
    ERR_free_strings();

    return 0;
}
*/
#include <iostream>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <openssl/crypto.h>
#include <openssl/x509.h>

//#define SERVER_IP "142.250.206.238"
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT "443"

void init_openssl()
{
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
}

void cleanup_openssl()
{
    EVP_cleanup();
}

SSL_CTX* create_context()
{
    const SSL_METHOD* method;
    SSL_CTX* ctx;

    //method = SSLv23_client_method(); // ssl-tls 중 가장 높은 버전을 사용
    method = TLS_client_method();
    ctx = SSL_CTX_new(method);
    if (!ctx) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    return ctx;
}

void configure_context(SSL_CTX* ctx)
{
    // 암호화 스위트 설정
    if (SSL_CTX_set_cipher_list(ctx, "AES128-SHA256") != 1) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
}

int main()
{
    SSL_CTX* ctx;
    SSL* ssl;
    BIO* bio;
    int ret;

    init_openssl();
    ctx = create_context();
    configure_context(ctx);

    bio = BIO_new_ssl_connect(ctx);
    if (!bio) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    // 서버 IP 및 포트 설정
    BIO_set_conn_hostname(bio, SERVER_IP ":" SERVER_PORT);

    // TLS 연결 수립
    ret = BIO_do_connect(bio);
    if (ret <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    // TLS 핸드셰이크 수행
    if (BIO_do_handshake(bio) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    // TLS 연결 성공
    std::cout << "TLS 연결이 수립되었습니다." << std::endl;

    // 데이터 송신
    const char* data = "Hello, Server!check";
    int data_len = strlen(data);
    ret = BIO_write(bio, data, data_len);
    if (ret <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    // 데이터 수신
    char buffer[1024];
    ret = BIO_read(bio, buffer, sizeof(buffer) - 1);
    if (ret <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
    buffer[ret] = '\0';

    std::cout << "서버로부터 받은 데이터: " << buffer << std::endl;

    // 연결 종료
    BIO_free_all(bio);
    SSL_CTX_free(ctx);
    cleanup_openssl();

    return 0;
}