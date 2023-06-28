#define _WINSOCK_DEPRECATED_NO_WARNINGS
/*
#include <iostream>
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/rand.h>

#define LISTEN_PORT "8080"

int main()
{
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();

    // TLS 서버 초기화
    SSL_CTX* ctx = SSL_CTX_new(TLS_server_method());
    if (!ctx)
    {
        std::cout << "TLS 서버를 초기화하는 데 실패했습니다." << std::endl;
        return 1;
    }

    // TLS 인증서 및 개인 키 로드
    if (SSL_CTX_use_certificate_file(ctx, "D:\\projectcert\\certificate.crt", SSL_FILETYPE_PEM) <= 0)
    {
        std::cout << "TLS 인증서를 로드하는 데 실패했습니다." << std::endl;
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(ctx);
        return 1;
    }
    if (SSL_CTX_use_PrivateKey_file(ctx, "D:\\projectcert\\private.key", SSL_FILETYPE_PEM) <= 0)
    {
        std::cout << "TLS 개인 키를 로드하는 데 실패했습니다." << std::endl;
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(ctx);
        return 1;
    }

    // TLS 연결 설정
    SSL* ssl;
    BIO* bio = BIO_new_ssl_connect(ctx);
    BIO* acceptBio = BIO_new_accept(LISTEN_PORT);
    BIO_set_accept_bios(acceptBio, bio);

    // 클라이언트 연결 대기
    if (BIO_do_accept(acceptBio) <= 0)
    {
        std::cout << "클라이언트 연결 대기에 실패했습니다." << std::endl;
        ERR_print_errors_fp(stderr);
        BIO_free_all(acceptBio);
        SSL_CTX_free(ctx);
        return 1;
    }

    // 클라이언트 연결 수락
    if (BIO_do_accept(acceptBio) <= 0)
    {
        std::cout << "클라이언트 연결 수락에 실패했습니다." << std::endl;
        ERR_print_errors_fp(stderr);
        BIO_free_all(acceptBio);
        SSL_CTX_free(ctx);
        return 1;
    }

    // 클라이언트와 TLS 연결 수립
    bio = BIO_pop(acceptBio);
    ssl = SSL_new(ctx);
    SSL_set_bio(ssl, bio, bio);

    if (SSL_accept(ssl) <= 0)
    {
        std::cout << "TLS 연결 수립에 실패했습니다." << std::endl;
        ERR_print_errors_fp(stderr);
        BIO_free_all(bio);
        SSL_CTX_free(ctx);
        return 1;
    }

    // 메시지 수신
    char buffer[2048];
    int bytesRead = SSL_read(ssl, buffer, sizeof(buffer));
    std::string message(buffer, bytesRead);
    std::cout << "클라이언트 메시지: " << message << std::endl;

    // 응답 전송
    std::string response = "안녕하세요, 클라이언트!";
    if (SSL_write(ssl, response.c_str(), response.length()) <= 0)
    {
        std::cout << "응답 전송에 실패했습니다." << std::endl;
        ERR_print_errors_fp(stderr);
        BIO_free_all(bio);
        SSL_CTX_free(ctx);
        return 1;
    }

    // TLS 연결 종료
    BIO_free_all(bio);
    SSL_CTX_free(ctx);

    return 0;
}
*/

/*1:1 Server
#include <iostream>
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <winsock2.h>

// TLS 서버 포트 번호
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
    SSL_CTX* ctx = SSL_CTX_new(TLS_server_method());

    // 인증서와 개인 키 파일 경로 설정
    std::string certFile = "D:\\projectcert\\certificate.crt";
    std::string keyFile = "D:\\projectcert\\private.key";

    // 인증서 로드
    if (SSL_CTX_use_certificate_file(ctx, certFile.c_str(), SSL_FILETYPE_PEM) <= 0)
    {
        std::cerr << "인증서 로드 실패." << std::endl;
        ERR_print_errors_fp(stderr);
        return 1;
    }

    // 개인 키 로드
    if (SSL_CTX_use_PrivateKey_file(ctx, keyFile.c_str(), SSL_FILETYPE_PEM) <= 0)
    {
        std::cerr << "개인 키 로드 실패." << std::endl;
        ERR_print_errors_fp(stderr);
        return 1;
    }

    // TCP 소켓 생성
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET)
    {
        std::cerr << "소켓 생성 실패." << std::endl;
        return 1;
    }

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(SERVER_PORT);

    // 소켓 바인딩
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
    {
        std::cerr << "바인딩 실패." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // 클라이언트 연결 대기
    if (listen(serverSocket, 5) == SOCKET_ERROR)
    {
        std::cerr << "연결 대기 실패." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // 클라이언트 연결 수락
    sockaddr_in clientAddress{};
    int clientAddrLength = sizeof(clientAddress);
    SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddrLength);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cerr << "연결 수락 실패." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // OpenSSL SSL 구조체 생성
    SSL* ssl = SSL_new(ctx);
    SSL_set_fd(ssl, clientSocket);

    // TLS 연결 수립
    if (SSL_accept(ssl) <= 0)
    {
        std::cerr << "TLS 연결 수립 실패." << std::endl;
        ERR_print_errors_fp(stderr);
        closesocket(clientSocket);
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // TLS 연결 성공
    std::cout << "TLS 연결이 수립되었습니다." << std::endl;

    // 데이터 송수신 및 처리
    // ...

    while (true) {
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
            std::cout << "Client 로부터 수신된 데이터: " << buffer << std::endl;
        }
        Sleep(1000);
    }

    

    // 연결 종료
    SSL_shutdown(ssl);
    SSL_free(ssl);
    closesocket(clientSocket);
    closesocket(serverSocket);

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
#include <openssl/bio.h>
#include <openssl/err.h>
#include <winsock2.h>
#include <thread>
#include <vector>

// TLS 서버 주소 및 포트 번호
#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_PORT 443

void HandleClient(SSL* ssl)
{
    // 클라이언트와의 통신 처리
    // ...
    while (true) {
        /*
        std::string message;
        std::cout << "client에 보낼 내용을 입력하세요: ";
        std::cin >> message;
        int messageLength = message.length();

        int bytesWritten = SSL_write(ssl, message.c_str(), messageLength);
        if (bytesWritten <= 0) {
            std::cerr << "데이터 전송 실패." << std::endl;
            ERR_print_errors_fp(stderr);
            // 오류 처리 로직 추가
        }*/

        // 데이터 수신
        const int bufferSize = 15;
        char buffer[bufferSize];

        int bytesRead = SSL_read(ssl, buffer, bufferSize);
        if (bytesRead <= 0) {
            //std::cerr << "데이터 수신 실패." << std::endl;
            ERR_print_errors_fp(stderr);
            // 오류 처리 로직 추가
        }
        else {
            // 수신된 데이터 출력
            //std::cout << "Client 로부터 수신된 데이터: " << buffer << std::endl;
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
}

void SendToMsgHandle(SSL* ssl) {
    while (true) {

        std::string message;
        std::cout << "client에 보낼 내용을 입력하세요: ";
        std::cin >> message;
        int messageLength = message.length();

        int bytesWritten = SSL_write(ssl, message.c_str(), messageLength);
        if (bytesWritten <= 0) {
            std::cerr << "데이터 전송 실패." << std::endl;
            ERR_print_errors_fp(stderr);
            // 오류 처리 로직 추가
        }
        Sleep(1000);
    }
    SSL_shutdown(ssl);
    SSL_free(ssl);
}

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
    SSL_CTX* ctx = SSL_CTX_new(TLS_server_method());

    // 인증서 및 개인 키 로드
    if (SSL_CTX_use_certificate_file(ctx, "D:\\projectcert\\certificate.crt", SSL_FILETYPE_PEM) != 1 ||
        SSL_CTX_use_PrivateKey_file(ctx, "D:\\projectcert\\private.key", SSL_FILETYPE_PEM) != 1)
    {
        std::cerr << "인증서 또는 개인 키 로드 실패." << std::endl;
        ERR_print_errors_fp(stderr);
        WSACleanup();
        return 1;
    }

    // 서버에 바인딩할 소켓 생성
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET)
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

    // 서버에 바인딩
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR)
    {
        std::cerr << "바인딩 실패." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // 클라이언트의 연결을 수락할 준비
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "수신 대기 실패." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "서버가 시작되었습니다. 클라이언트의 연결을 대기 중..." << std::endl;

    std::vector<std::thread> clientThreads;
    std::vector<std::thread> sendToClientThreads;

    while (true)
    {
        // 클라이언트의 연결을 수락
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET)
        {
            std::cerr << "클라이언트 연결 수락 실패." << std::endl;
            closesocket(serverSocket);
            WSACleanup();
            return 1;
        }

        // OpenSSL SSL 구조체 생성
        SSL* ssl = SSL_new(ctx);
        SSL_set_fd(ssl, clientSocket);

        // TLS 연결 수립
        if (SSL_accept(ssl) <= 0)
        {
            std::cerr << "TLS 연결 수립 실패." << std::endl;
            ERR_print_errors_fp(stderr);
            closesocket(clientSocket);
            SSL_free(ssl);
            continue;
        }

        // 클라이언트와의 통신을 처리할 스레드 생성
        std::thread clientThread(HandleClient, ssl);
        std::thread sendToClientThread(SendToMsgHandle, ssl);
        clientThread.detach(); // 스레드를 백그라운드에서 동작하도록 설정
        sendToClientThread.detach();

        // 클라이언트 스레드를 저장하여 추후에 조작 가능
        clientThreads.push_back(std::move(clientThread));
        sendToClientThreads.push_back(std::move(sendToClientThread));
    }

    // 클라이언트 스레드를 모두 종료시키기 위해 대기
    for (auto& clientThread : clientThreads)
    {
        if (clientThread.joinable())
            clientThread.join();
    }

    // 클라이언트 스레드를 모두 종료시키기 위해 대기
    for (auto& sendToClientThread : sendToClientThreads)
    {
        if (sendToClientThread.joinable())
            sendToClientThread.join();
    }

    // 서버 소켓 및 OpenSSL 정리
    closesocket(serverSocket);
    SSL_CTX_free(ctx);
    EVP_cleanup();
    ERR_free_strings();

    // Winsock 정리
    WSACleanup();

    return 0;
}
